#include "requirement_detail_servlet.h"
#include <chen/log/log.h>
#include "../../util.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/requirement_manager.h"
#include "../../permission.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

RequirementDetailServlet::RequirementDetailServlet()
    : BlogLoginedServlet("RequirementDetailServlet") {
}

int32_t RequirementDetailServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        DEFINE_AND_CHECK_TYPE(result, int64_t, org_id, "org_id");

        int64_t uid = getUserId(request);

        auto info = RequirementMgr::GetInstance()->get(id);
        if (!info || info->getIsDeleted() || info->getOrgId() != org_id) {
            result->setResult(404, "requirement not exist");
            break;
        }

        // 获取用户在组织中的角色
        auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
            result->setResult(403, "not a member of this organization");
            break;
        }
        int32_t orgRole = rel->getRole();

        if (!permission::canViewRequirement(orgRole, uid, info->getCreatorId())) {
            result->setResult(403, "Access Denied");
            break;
        }

        result->set("id", info->getId());
        result->set("org_id", info->getOrgId());
        result->set("title", info->getTitle());
        result->set("description", info->getDescription());
        result->set("priority", info->getPriority());
        result->set("status", info->getStatus());
        result->set("creator_id", info->getCreatorId());
        result->set("assignee_id", info->getAssigneeId());
        result->set("deadline", info->getDeadline());
        result->set("create_time", info->getCreateTime());
        result->set("update_time", info->getUpdateTime());
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
