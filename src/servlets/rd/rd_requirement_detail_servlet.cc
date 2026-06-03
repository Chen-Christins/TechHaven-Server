#include "rd_requirement_detail_servlet.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/requirement_manager.h"
#include "../../permission.h"
#include "rd_helper.h"

namespace blog {
namespace servlet {

RdRequirementDetailServlet::RdRequirementDetailServlet()
    : BlogLoginedServlet("RdRequirementDetailServlet") {
}

int32_t RdRequirementDetailServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t id = request->getParamAs<int64_t>("id", 0);
        if (!id) {
            result->setResult(400, "param id is required");
            break;
        }

        int64_t uid = getUserId(request);
        int32_t system_role = UserMgr::GetInstance()->get(uid)->getRole();
        bool is_platform_admin = (system_role == UserManager::Role::ADMIN);

        auto info = RequirementMgr::GetInstance()->get(id);
        if (!info || info->getIsDeleted()) {
            result->setResult(404, "requirement not exist");
            break;
        }

        int64_t info_org_id = info->getOrgId();
        if (!is_platform_admin) {
            auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(info_org_id, uid);
            if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
                result->setResult(403, "Access Denied");
                break;
            }
            if (!permission::CanViewRequirement(rel->getRole(), uid, info->getCreatorId())) {
                result->setResult(403, "Access Denied");
                break;
            }
        }

        rd::BuildRequirementJson(result->jsondata, info);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
