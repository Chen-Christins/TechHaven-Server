#include "organization_user_kick_servlet.h"
#include <chen/log/log.h>
#include "../../util.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/organization_manager.h"


namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

OrganizationUserKickServlet::OrganizationUserKickServlet()
    : BlogLoginedServlet("OrganizationUserKickServlet") {
}

int32_t OrganizationUserKickServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id"); // organization_user_rel id
        DEFINE_AND_CHECK_TYPE(result, int64_t, org_id, "org_id"); // organization id
        DEFINE_AND_CHECK_TYPE(result, int64_t, user_id, "user_id"); // user_id

        // 操作者权限检查
        int64_t uid = getUserId(request);
        int32_t system_role = blog::UserMgr::GetInstance()->get(uid)->getRole();

        auto rel = blog::OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        int32_t org_role = rel->getRole();

        if (!checkPermission(system_role, org_role)) {
            result->setResult(403, "Access Denied");
            break;
        }
        
        // 踢出用户
        rel = blog::OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, user_id);
        if (!rel) {
            result->setResult(404, "invalid id");
            break;
        }
        
        rel->setStatus(OrganizationUserRelManager::Status::EXITED);
        rel->setUpdateTime(time(0));

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db error");
            break;
        }

        if (data::OrganizationUserRelInfoDao::InsertOrUpdate(rel, db)) {
            result->setResult(500, "insert or update organization user rel fail");
            ERROR(logger) << "db error, errno=" << db->getErrno()
                          << ", errstr=" << db->getErrStr();
            break;
        }

        result->set("success", true);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

bool OrganizationUserKickServlet::checkPermission(int32_t system_role, int32_t org_role) {
    // 系统管理员或组织所有者、管理员有权限踢出用户
    if (system_role == UserManager::Role::ADMIN) {
        return true;
    }
    if (org_role == OrganizationManager::Role::OWNER) {
        return true;
    }
    if (org_role == OrganizationManager::Role::ADMIN) {
        return true;
    }
    return false;
}

}
}
