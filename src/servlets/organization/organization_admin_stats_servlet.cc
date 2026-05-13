#include "organization_admin_stats_servlet.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_manager.h"

namespace blog {
namespace servlet {

OrganizationAdminStatsServlet::OrganizationAdminStatsServlet()
    : BlogLoginedServlet("OrganizationAdminStatsServlet") {
}

int32_t OrganizationAdminStatsServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(401, "not login");
            break;
        }
        if (UserMgr::GetInstance()->get(uid)->getRole() != UserManager::Role::ADMIN) {
            result->setResult(403, "Access Denied");
            break;
        }

        auto stats = OrganizationMgr::GetInstance()->getStats();

        result->setResult(200, "ok");
        result->set("total_organizations", stats.total);
        result->set("active_organizations", stats.active);
        result->set("inactive_organizations", stats.inactive);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
