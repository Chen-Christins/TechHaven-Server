#include "rd_bug_delete_servlet.h"

#include "../../manager/user_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/bug_manager.h"
#include "../../permission.h"
#include "../../util.h"

namespace blog {
namespace servlet {

RdBugDeleteServlet::RdBugDeleteServlet()
    : BlogLoginedServlet("RdBugDeleteServlet") {
}

int32_t RdBugDeleteServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        DEFINE_AND_CHECK_TYPE(result, int64_t, org_id, "org_id");
        std::string idsStr = request->getParam("ids");

        std::set<int64_t> delIds;
        if (id) {
            delIds.insert(id);
        }
        if (!idsStr.empty()) {
            for (auto& s : chen::split(idsStr, ",")) {
                delIds.insert(chen::TypeUtil::Atoi(s));
            }
        }

        int64_t uid = getUserId(request);
        int32_t systemRole = UserMgr::GetInstance()->get(uid)->getRole();
        bool isPlatformAdmin = (systemRole == UserManager::Role::ADMIN);

        int32_t orgRole = 0;
        if (!isPlatformAdmin) {
            auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
            if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
                result->setResult(403, "Access Denied");
                break;
            }
            orgRole = rel->getRole();
        }

        std::vector<data::BugInfo::ptr> all;
        BugMgr::GetInstance()->listByOrg(all, org_id, 0, UINT64_MAX, -1, true);
        std::vector<data::BugInfo::ptr> delItems;
        for (auto& i : all) {
            if (!delIds.count(i->getId())) {
                continue;
            }
            if (!isPlatformAdmin && uid != i->getCreatorId()
                    && !permission::canDeleteBug(orgRole)) {
                continue;
            }
            delItems.push_back(i);
        }

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db error");
            break;
        }
        auto trans = db->openTransaction();
        if (!trans) {
            result->setResult(500, "open transaction fail");
            break;
        }
        time_t now = time(0);
        for (auto& i : delItems) {
            i->setIsDeleted(1);
            i->setUpdateTime(now);
            data::BugInfoDao::Update(i, db);
        }
        if (!trans->commit()) {
            for (auto& i : delItems) {
                i->setIsDeleted(0);
            }
            result->setResult(500, "commit fail");
            break;
        }
        if (!delItems.empty()) {
            auto& jids = result->jsondata["ids"];
            for (auto& i : delItems) {
                jids.append(i->getId());
            }
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
