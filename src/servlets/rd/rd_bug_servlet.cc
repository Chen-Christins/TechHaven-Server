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

}
}
