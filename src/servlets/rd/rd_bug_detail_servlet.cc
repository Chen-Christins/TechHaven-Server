#include "rd_bug_detail_servlet.h"
#include "rd_helper.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/bug_manager.h"
#include "../../permission.h"

namespace blog {
namespace servlet {

RdBugDetailServlet::RdBugDetailServlet()
    : BlogLoginedServlet("RdBugDetailServlet") {
}

int32_t RdBugDetailServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t id = request->getParamAs<int64_t>("id", 0);
        if (!id) {
            result->setResult(400, "param id is required");
            break;
        }

        int64_t uid = getUserId(request);
        int32_t systemRole = UserMgr::GetInstance()->get(uid)->getRole();
        bool isPlatformAdmin = (systemRole == UserManager::Role::ADMIN);

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
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
