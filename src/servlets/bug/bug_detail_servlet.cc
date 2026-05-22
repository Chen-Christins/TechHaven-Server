#include "bug_detail_servlet.h"
#include <chen/log/log.h>
#include "../../util.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/bug_manager.h"
#include "../../permission.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

BugDetailServlet::BugDetailServlet()
    : BlogLoginedServlet("BugDetailServlet") {
}

int32_t BugDetailServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        DEFINE_AND_CHECK_TYPE(result, int64_t, org_id, "org_id");

        int64_t uid = getUserId(request);

        auto info = BugMgr::GetInstance()->get(id);
        if (!info || info->getIsDeleted() || info->getOrgId() != org_id) {
            result->setResult(404, "bug not exist");
            break;
        }

        auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
            result->setResult(403, "not a member of this organization");
            break;
        }
        int32_t orgRole = rel->getRole();

        if (!permission::canViewBug(orgRole, uid, info->getCreatorId())) {
            result->setResult(403, "Access Denied");
            break;
        }

        result->set("id", info->getId());
        result->set("org_id", info->getOrgId());
        result->set("title", info->getTitle());
        result->set("description", info->getDescription());
        result->set("severity", info->getSeverity());
        result->set("priority", info->getPriority());
        result->set("status", info->getStatus());
        result->set("creator_id", info->getCreatorId());
        result->set("assignee_id", info->getAssigneeId());
        result->set("requirement_id", info->getRequirementId());
        result->set("create_time", info->getCreateTime());
        result->set("update_time", info->getUpdateTime());
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
