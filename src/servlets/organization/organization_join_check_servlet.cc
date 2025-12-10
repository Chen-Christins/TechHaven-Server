#include "organization_join_check_servlet.h"
#include <chen/log/log.h>
#include "../../util.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../types.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

OrganizationJoinCheckServlet::OrganizationJoinCheckServlet()
    : BlogLoginedServlet("OrganizationJoinCheckServlet") {
}

int32_t OrganizationJoinCheckServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, user_id, "user_id");
        DEFINE_AND_CHECK_TYPE(result, int64_t, org_id, "org_id");
        DEFINE_AND_CHECK_TYPE(result, int32_t, state, "state");

        if (state != static_cast<int32_t>(types::Status::UserOrganization::REJECTED)
                && state != static_cast<int32_t>(types::Status::UserOrganization::APPROVED)) {
            result->setResult(400, "invalid state");
            break;
        }

        auto user = blog::UserMgr::GetInstance()->get(user_id);
        auto org = blog::OrganizationMgr::GetInstance()->get(org_id);
        if (!user || !org) {
            result->setResult(404, "invalid id");
            break;
        }

        // check operater permission
        int64_t uid = getUserId(request);
        int32_t system_role = blog::UserMgr::GetInstance()->get(uid)->getRole();
        
        auto rel = blog::OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        int32_t org_role = rel->getRole();
        
        if (system_role != static_cast<int32_t>(types::Role::System::ADMIN)
                && org_role != static_cast<int32_t>(types::Role::Organization::ADMIN)
                && org_role != static_cast<int32_t>(types::Role::Organization::OWNER)) {
            result->setResult(403, "Access Denied");
            break;
        }

        // update user organization relation
        rel = blog::OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, user_id);
        if (!rel) {
            result->setResult(404, "invalid id");
            break;
        }
        if (rel->getStatus() != static_cast<int32_t>(types::Status::UserOrganization::PENDING)) {
            result->setResult(403, "invalid state");
            break;
        }
        rel->setStatus(state);
        rel->setRole(static_cast<int32_t>(types::Role::Organization::MEMBER));

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

}
}
