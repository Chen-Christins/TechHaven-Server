#include "rd_requirement_servlet.h"
#include "rd_helper.h"
#include "rd_macros.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/requirement_manager.h"
#include "../../permission.h"

namespace blog {
namespace servlet {

RdRequirementServlet::RdRequirementServlet()
    : BlogLoginedServlet("RdRequirementServlet") {
}

int32_t RdRequirementServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
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
                auto info = RequirementMgr::GetInstance()->get(id);
                if (!info || info->getIsDeleted()) {
                    result->setResult(404, "requirement not exist");
                    break;
                }
                int64_t infoOrgId = info->getOrgId();
                if (!isPlatformAdmin) {
                    auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(infoOrgId, uid);
                    if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
                        result->setResult(403, "Access Denied");
                        break;
                    }
                    if (!permission::canViewRequirement(rel->getRole(), uid, info->getCreatorId())) {
                        result->setResult(403, "Access Denied");
                        break;
                    }
                }
                rd::buildRequirementJson(result->jsondata, info);
                break;
            }

            uint64_t page = request->getParamAs<uint64_t>("page", 1);
            uint64_t pageSize = request->getParamAs<uint64_t>("page_size", 10);
            uint64_t offset, size;
            rd::parsePagination(page, pageSize, offset, size);

            std::string search = request->getParam("search");
            std::string statusStr = request->getParam("status");
            std::string priorityStr = request->getParam("priority");

            std::vector<data::RequirementInfo::ptr> all;
            if (isPlatformAdmin && org_id) {
                RequirementMgr::GetInstance()->listByOrg(all, org_id, 0, UINT64_MAX, -1, true);
            } else if (!isPlatformAdmin) {
                std::vector<data::OrganizationUserRelInfo::ptr> userOrgs;
                OrganizationUserRelMgr::GetInstance()->getOrgByUserId(userOrgs, uid, OrganizationUserRelManager::Status::APPROVED, true);
                for (auto& rel : userOrgs) {
                    if (org_id && rel->getOrgId() != org_id) {
                        continue;
                    }
                    std::vector<data::RequirementInfo::ptr> orgReqs;
                    RequirementMgr::GetInstance()->listByOrg(orgReqs, rel->getOrgId(), 0, UINT64_MAX, -1, true);
                    for (auto& req : orgReqs) {
                        if (permission::canViewRequirement(rel->getRole(), uid, req->getCreatorId())) {
                            all.push_back(req);
                        }
                    }
                }
            } else {
                RequirementMgr::GetInstance()->listByPages(all, 0, UINT64_MAX, -1, true);
            }

            std::vector<data::RequirementInfo::ptr> filtered;
            for (auto& info : all) {
                if (!statusStr.empty() && rd::stringToRequirementStatus(statusStr) != info->getStatus()) {
                    continue;
                }
                if (!priorityStr.empty() && rd::stringToPriority(priorityStr) != info->getPriority()) {
                    continue;
                }
                if (!search.empty()) {
                    std::string title = info->getTitle();
                    std::string desc = info->getDescription();
                    std::string creator = rd::getUserName(info->getCreatorId());
                    if (title.find(search) == std::string::npos
                            && desc.find(search) == std::string::npos
                            && creator.find(search) == std::string::npos) {
                        continue;
                    }
                }
                filtered.push_back(info);
            }

            uint64_t total = filtered.size();
            Json::Value arr(Json::arrayValue);
            for (uint64_t i = offset; i < filtered.size() && arr.size() < size; ++i) {
                Json::Value item;
                rd::buildRequirementJson(item, filtered[i]);
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
            int64_t id = request->getParamAs<int64_t>("id", 0);
            std::string reqBody = request->getBody();
            Json::Value body;
            if (!reqBody.empty()) {
                Json::Reader reader;
                if (reader.parse(reqBody, body)) {
                    if (!id) {
                        id = rd::getJsonInt64(body, "id");
                    }
                }
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
            data::RequirementInfo::ptr info;

            if (id) {
                info = RequirementMgr::GetInstance()->get(id);
                if (!info || info->getOrgId() != org_id) {
                    result->setResult(404, "requirement not exist");
                    break;
                }
                if (!permission::canEditRequirement(orgRole)) {
                    result->setResult(403, "Access Denied");
                    break;
                }
            } else {
                if (!permission::canCreateRequirement(orgRole)) {
                    result->setResult(403, "Access Denied");
                    break;
                }
                info.reset(new data::RequirementInfo);
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
                int32_t s = rd::stringToRequirementStatus(statusStr);
                if (s >= 0) {
                    info->setStatus(s);
                }
            }

            RD_PARAM_INT(assignee_id, "assignee_id");
            if (assignee_id) {
                info->setAssigneeId(assignee_id);
            }
            RD_PARAM_INT(deadline, "deadline");
            if (deadline) {
                info->setDeadline(deadline);
            }

            RD_PARAM_STR(iteration, "iteration");
            if (!iteration.empty()) {
                info->setIteration(iteration);
            }
            RD_PARAM_STR(category, "category");
            if (!category.empty()) {
                info->setCategory(category);
            }
            RD_PARAM_STR(source, "source");
            if (!source.empty()) {
                info->setSource(source);
            }

            info->setIsDeleted(0);
            info->setUpdateTime(time(0));

            auto db = getDB();
            if (!db) {
                result->setResult(500, "get db error");
                break;
            }

            if (data::RequirementInfoDao::InsertOrUpdate(info, db)) {
                result->setResult(500, "insert or update requirement fail");
                break;
            }

            if (is_new) {
                RequirementMgr::GetInstance()->add(info);
            }

            rd::buildRequirementJson(result->jsondata, info);
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
