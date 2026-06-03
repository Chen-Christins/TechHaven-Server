#include "rd_check_access_servlet.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/user_manager.h"
#include "../../permission.h"

namespace blog {
namespace servlet {

RdCheckAccessServlet::RdCheckAccessServlet()
    : BlogLoginedServlet("RdCheckAccessServlet") {
}

int32_t RdCheckAccessServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t current_uid = getUserId(request);
        data::UserInfo::ptr currentUser = UserMgr::GetInstance()->get(current_uid);
        if (!currentUser || currentUser->getState() != 1) {
            result->setResult(410, "invalid user");
            break;
        }

        int64_t target_uid = current_uid;
        std::string uidParam = request->getParam("user_id");
        if (!uidParam.empty()) {
            target_uid = atol(uidParam.c_str());
            // 只有管理员可以查询其他用户的平台访问权限
            if (target_uid != current_uid && currentUser->getRole() != UserManager::Role::ADMIN) {
                result->setResult(403, "Access Denied");
                break;
            }
        }

        data::UserInfo::ptr targetUser = UserMgr::GetInstance()->get(target_uid);
        if (!targetUser || targetUser->getState() != 1) {
            result->setResult(200, "ok");
            result->set("can_access", "0");
            result->set("reason", "user not found or disabled");
            break;
        }

        int32_t system_role = targetUser->getRole();
        std::vector<data::OrganizationUserRelInfo::ptr> user_orgs;
        OrganizationUserRelMgr::GetInstance()->getOrgByUserId(
            user_orgs, target_uid, OrganizationUserRelManager::APPROVED, true);

        int32_t highest_org_role = 0;
        for (auto& rel : user_orgs) {
            if (rel->getRole() > highest_org_role) {
                highest_org_role = rel->getRole();
            }
        }

        bool canAccess = permission::CanAccessPlatform(system_role, highest_org_role);

        result->setResult(200, "ok");
        result->set("can_access", canAccess ? "1" : "0");
        if (!canAccess) {
            result->set("reason", "requires system admin or organization role >= reporter");
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
