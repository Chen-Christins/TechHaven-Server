#include "rd_bug_servlet.h"
#include "rd_helper.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/bug_manager.h"
#include "../../permission.h"

namespace blog {
namespace servlet {

RdBugServlet::RdBugServlet()
    : BlogLoginedServlet("RdBugServlet") {
}

int32_t RdBugServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
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
                auto info = BugMgr::GetInstance()->get(id);
                if (!info || info->getIsDeleted()) {
                    result->setResult(404, "bug not exist");
                    break;
                }
                int64_t infoOrgId = info->getOrgId();
                if (!isPlatformAdmin) {
                    auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(infoOrgId, uid);
                    if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
                        result->setResult(403, "Access Denied");
                        break;
                    }
                    if (!permission::canViewBug(rel->getRole(), uid, info->getCreatorId())) {
                        result->setResult(403, "Access Denied");
                        break;
                    }
                }
                rd::buildBugJson(result->jsondata, info);
                break;
            }

            uint64_t page = request->getParamAs<uint64_t>("page", 1);
            uint64_t pageSize = request->getParamAs<uint64_t>("page_size", 10);
            uint64_t offset, size;
            rd::parsePagination(page, pageSize, offset, size);

            std::string search = request->getParam("search");
            std::string statusStr = request->getParam("status");
            std::string priorityStr = request->getParam("priority");
            std::string severityStr = request->getParam("severity");

            std::vector<data::BugInfo::ptr> all;
            if (isPlatformAdmin && org_id) {
                BugMgr::GetInstance()->listByOrg(all, org_id, 0, UINT64_MAX, -1, true);
            } else if (!isPlatformAdmin) {
                std::vector<data::OrganizationUserRelInfo::ptr> userOrgs;
                OrganizationUserRelMgr::GetInstance()->getOrgByUserId(userOrgs, uid, OrganizationUserRelManager::Status::APPROVED, true);
                for (auto& rel : userOrgs) {
                    if (org_id && rel->getOrgId() != org_id) {
                        continue;
                    }
                    std::vector<data::BugInfo::ptr> orgBugs;
                    BugMgr::GetInstance()->listByOrg(orgBugs, rel->getOrgId(), 0, UINT64_MAX, -1, true);
                    for (auto& bug : orgBugs) {
                        if (permission::canViewBug(rel->getRole(), uid, bug->getCreatorId())) {
                            all.push_back(bug);
                        }
                    }
                }
            } else {
                BugMgr::GetInstance()->listByPages(all, 0, UINT64_MAX, -1, true);
            }

            std::vector<data::BugInfo::ptr> filtered;
            for (auto& info : all) {
                if (!statusStr.empty() && rd::stringToBugStatus(statusStr) != info->getStatus()) {
                    continue;
                }
                if (!priorityStr.empty() && rd::stringToPriority(priorityStr) != info->getPriority()) {
                    continue;
                }
                if (!severityStr.empty() && rd::stringToSeverity(severityStr) != info->getSeverity()) {
                    continue;
                }
                if (!search.empty()) {
                    std::string title = info->getTitle();
                    std::string desc = info->getDescription();
                    if (title.find(search) == std::string::npos && desc.find(search) == std::string::npos) {
                        continue;
                    }
                }
                filtered.push_back(info);
            }

            uint64_t total = filtered.size();
            Json::Value arr(Json::arrayValue);
            for (uint64_t i = offset; i < filtered.size() && arr.size() < size; ++i) {
                Json::Value item;
                rd::buildBugJson(item, filtered[i]);
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
            data::BugInfo::ptr info;
            if (id) {
                info = BugMgr::GetInstance()->get(id);
                if (!info || info->getOrgId() != org_id) {
                    result->setResult(404, "bug not exist");
                    break;
                }
                if (!permission::canEditBug(orgRole, uid, info->getCreatorId())) {
                    result->setResult(403, "Access Denied");
                    break;
                }
            } else {
                if (!permission::canCreateBug(orgRole)) {
                    result->setResult(403, "Access Denied");
                    break;
                }
                info.reset(new data::BugInfo);
                info->setOrgId(org_id);
                info->setCreatorId(uid);
                info->setCreateTime(time(0));
                is_new = true;
            }

            auto getParam = [&](const std::string& key) -> std::string {
                if (!body.isNull() && body.isMember(key)) {
                    if (body[key].isString()) {
                        return body[key].asString();
                    }
                    return std::to_string(rd::getJsonInt64(body, key));
                }
                return request->getParam(key);
            };
            auto getParamInt = [&](const std::string& key) -> int64_t {
                if (!body.isNull() && body.isMember(key)) {
                    return rd::getJsonInt64(body, key);
                }
                return request->getParamAs<int64_t>(key, 0);
            };

            std::string title = getParam("title");
            if (!title.empty()) {
                info->setTitle(title);
            }
            std::string desc = getParam("description");
            if (!desc.empty()) {
                info->setDescription(desc);
            }
            std::string severityStr = getParam("severity");
            if (!severityStr.empty()) {
                int32_t s = rd::stringToSeverity(severityStr);
                if (s >= 0) {
                    info->setSeverity(s);
                }
            }
            std::string priorityStr = getParam("priority");
            if (!priorityStr.empty()) {
                int32_t p = rd::stringToPriority(priorityStr);
                if (p >= 0) {
                    info->setPriority(p);
                }
            }
            std::string statusStr = getParam("status");
            if (!statusStr.empty()) {
                int32_t s = rd::stringToBugStatus(statusStr);
                if (s >= 0) {
                    info->setStatus(s);
                }
            }

            int64_t assignee_id = getParamInt("assignee_id");
            if (assignee_id) {
                info->setAssigneeId(assignee_id);
            }
            int64_t requirement_id = getParamInt("related_requirement_id");
            if (requirement_id) {
                info->setRequirementId(requirement_id);
            }

            std::string module = getParam("module");
            if (!module.empty()) {
                info->setModule(module);
            }
            std::string steps = getParam("steps_to_reproduce");
            if (!steps.empty()) {
                info->setStepsToReproduce(steps);
            }
            std::string env = getParam("environment");
            if (!env.empty()) {
                info->setEnvironment(env);
            }

            info->setIsDeleted(0);
            info->setUpdateTime(time(0));

            auto db = getDB();
            if (!db) {
                result->setResult(500, "get db error");
                break;
            }
            if (data::BugInfoDao::InsertOrUpdate(info, db)) {
                result->setResult(500, "insert or update bug fail");
                break;
            }
            if (is_new) {
                BugMgr::GetInstance()->add(info);
            }
            rd::buildBugJson(result->jsondata, info);
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
