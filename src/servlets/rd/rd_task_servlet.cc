#include "rd_task_servlet.h"
#include "rd_helper.h"
#include "rd_macros.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/task_manager.h"
#include "../../permission.h"

namespace blog {
namespace servlet {

RdTaskServlet::RdTaskServlet()
    : BlogLoginedServlet("RdTaskServlet") {
}

int32_t RdTaskServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    auto method = request->getMethod();
    if (method == chen::http::HttpMethod::GET) {
        do {
            int64_t id = request->getParamAs<int64_t>("id", 0);
            int64_t org_id = request->getParamAs<int64_t>("org_id", 0);
            int64_t uid = getUserId(request);
            int32_t systemRole = UserMgr::GetInstance()->get(uid)->getRole();
            bool isPlatformAdmin = (systemRole == UserManager::Role::ADMIN);

            if (id) {
                auto info = TaskMgr::GetInstance()->get(id);
                if (!info || info->getIsDeleted()) {
                    result->setResult(404, "task not exist");
                    break;
                }
                int64_t infoOrgId = info->getOrgId();
                if (!isPlatformAdmin) {
                    auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(infoOrgId, uid);
                    if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
                        result->setResult(403, "Access Denied");
                        break;
                    }
                    bool isRelated = (uid == info->getCreatorId() || uid == info->getAssigneeId());
                    if (!permission::canViewTask(rel->getRole(), isRelated)) {
                        result->setResult(403, "Access Denied");
                        break;
                    }
                }
                rd::buildTaskJson(result->jsondata, info);
                break;
            }

            uint64_t page = request->getParamAs<uint64_t>("page", 1);
            uint64_t pageSize = request->getParamAs<uint64_t>("page_size", 10);
            uint64_t offset, size;
            rd::parsePagination(page, pageSize, offset, size);

            std::string search = request->getParam("search");
            std::string statusStr = request->getParam("status");
            std::string priorityStr = request->getParam("priority");
            int64_t filterAssigneeId = request->getParamAs<int64_t>("assignee_id", 0);

            std::vector<data::TaskInfo::ptr> all;
            if (isPlatformAdmin && org_id) {
                TaskMgr::GetInstance()->listByOrg(all, org_id, 0, UINT64_MAX, -1, true);
            } else if (!isPlatformAdmin) {
                std::vector<data::OrganizationUserRelInfo::ptr> userOrgs;
                OrganizationUserRelMgr::GetInstance()->getOrgByUserId(userOrgs, uid, OrganizationUserRelManager::Status::APPROVED, true);
                for (auto& rel : userOrgs) {
                    if (org_id && rel->getOrgId() != org_id) {
                        continue;
                    }
                    std::vector<data::TaskInfo::ptr> orgTasks;
                    TaskMgr::GetInstance()->listByOrg(orgTasks, rel->getOrgId(), 0, UINT64_MAX, -1, true);
                    for (auto& task : orgTasks) {
                        bool isRelated = (uid == task->getCreatorId() || uid == task->getAssigneeId());
                        if (permission::canViewTask(rel->getRole(), isRelated)) {
                            all.push_back(task);
                        }
                    }
                }
            } else {
                TaskMgr::GetInstance()->listByPages(all, 0, UINT64_MAX, -1, true);
            }

            std::vector<data::TaskInfo::ptr> filtered;
            for (auto& info : all) {
                if (!statusStr.empty() && rd::stringToTaskStatus(statusStr) != info->getStatus()) {
                    continue;
                }
                if (!priorityStr.empty() && rd::stringToPriority(priorityStr) != info->getPriority()) {
                    continue;
                }
                if (filterAssigneeId && info->getAssigneeId() != filterAssigneeId) {
                    continue;
                }
                if (!search.empty()) {
                    std::string title = info->getTitle();
                    if (title.find(search) == std::string::npos) {
                        continue;
                    }
                }
                filtered.push_back(info);
            }

            uint64_t total = filtered.size();
            Json::Value arr(Json::arrayValue);
            for (uint64_t i = offset; i < filtered.size() && arr.size() < size; ++i) {
                Json::Value item;
                rd::buildTaskJson(item, filtered[i]);
                arr.append(item);
            }
            result->set("total", total);
            result->set("list", arr);
        } while (0);
        response->setBody(result->toJsonString());
        return 0;
    }

    if (method == chen::http::HttpMethod::POST) {
        do {
            std::string reqBody = request->getBody();
            Json::Value body;
            if (!reqBody.empty()) {
                Json::Reader reader;
                reader.parse(reqBody, body);
            }

            int64_t id = request->getParamAs<int64_t>("id", 0);
            if (!id && !body.isNull()) {
                id = rd::getJsonInt64(body, "id");
            }
            int64_t org_id = request->getParamAs<int64_t>("org_id", 0);
            if (!org_id && !body.isNull()) {
                org_id = rd::getJsonInt64(body, "org_id");
            }

            int64_t uid = getUserId(request);
            if (!uid) {
                result->setResult(500, "not login");
                break;
            }

            auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
            if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
                result->setResult(403, "not a member of this organization");
                break;
            }
            int32_t orgRole = rel->getRole();

            bool is_new = false;
            data::TaskInfo::ptr info;
            if (id) {
                info = TaskMgr::GetInstance()->get(id);
                if (!info || info->getOrgId() != org_id) {
                    result->setResult(404, "task not exist");
                    break;
                }
                if (!permission::canEditTask(orgRole, uid, info->getAssigneeId())) {
                    result->setResult(403, "Access Denied");
                    break;
                }
            } else {
                if (!permission::canCreateTask(orgRole)) {
                    result->setResult(403, "Access Denied");
                    break;
                }
                info.reset(new data::TaskInfo);
                info->setOrgId(org_id);
                info->setCreatorId(uid);
                info->setCreateTime(time(0));
                is_new = true;
            }

            RD_PARAM_STR(title, "title");
            if (!title.empty()) {
                info->setTitle(title);
            }
            RD_PARAM_STR(desc, "description");
            if (!desc.empty()) {
                info->setDescription(desc);
            }
            RD_PARAM_STR(priorityStr, "priority");
            if (!priorityStr.empty()) {
                int32_t p = rd::stringToPriority(priorityStr);
                if (p >= 0) {
                    info->setPriority(p);
                }
            }
            RD_PARAM_STR(statusStr, "status");
            if (!statusStr.empty()) {
                int32_t s = rd::stringToTaskStatus(statusStr);
                if (s >= 0) {
                    info->setStatus(s);
                }
            }

            RD_PARAM_INT(assignee_id, "assignee_id");
            if (assignee_id) {
                info->setAssigneeId(assignee_id);
            }
            RD_PARAM_INT(requirement_id, "requirement_id");
            if (requirement_id) {
                info->setRequirementId(requirement_id);
            }
            RD_PARAM_INT(bug_id, "bug_id");
            if (bug_id) {
                info->setBugId(bug_id);
            }
            RD_PARAM_INT(deadline, "deadline");
            if (deadline) {
                info->setDeadline(deadline);
            }
            RD_PARAM_INT(estimated_hours, "estimated_hours");
            if (estimated_hours) {
                info->setEstimatedHours(estimated_hours);
            }

            info->setIsDeleted(0);
            info->setUpdateTime(time(0));

            auto db = getDB();
            if (!db) {
                result->setResult(500, "get db error");
                break;
            }
            if (data::TaskInfoDao::InsertOrUpdate(info, db)) {
                result->setResult(500, "insert or update task fail");
                break;
            }
            if (is_new) {
                TaskMgr::GetInstance()->add(info);
            }
            rd::buildTaskJson(result->jsondata, info);
        } while (0);
        response->setBody(result->toJsonString());
        return 0;
    }

    result->setResult(405, "Method Not Allowed");
    response->setBody(result->toJsonString());
    return 0;
}

}
}
