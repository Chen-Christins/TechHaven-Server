#include "requirement_create_servlet.h"
#include <chen/log/log.h>
#include "../../util.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/requirement_manager.h"
#include "../../permission.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

RequirementCreateServlet::RequirementCreateServlet()
    : BlogLoginedServlet("RequirementCreateServlet") {
}

int32_t RequirementCreateServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, org_id, "org_id");
        DEFINE_AND_CHECK_STRING(result, title, "title");
        DEFINE_AND_CHECK_TYPE(result, int32_t, priority, "priority");
        DEFINE_AND_CHECK_TYPE(result, int32_t, status, "status");
        DEFINE_AND_CHECK_STRING(result, description, "description");
        int64_t req_id = request->getParamAs<int64_t>("id", 0);

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }

        // 获取用户在组织中的角色
        auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
            result->setResult(403, "not a member of this organization");
            break;
        }
        int32_t orgRole = rel->getRole();

        bool is_new = false;
        data::RequirementInfo::ptr info;

        if (req_id) {
            info = RequirementMgr::GetInstance()->get(req_id);
            if (!info || info->getOrgId() != org_id) {
                result->setResult(404, "requirement not exist");
                break;
            }
            if (!permission::canEditRequirement(orgRole)) {
                result->setResult(403, "Access Denied");
                break;
            }
        } else {
            if (!permission::canCreateRequirement(orgRole)) {
                result->setResult(403, "Access Denied");
                break;
            }
            info.reset(new data::RequirementInfo);
            info->setOrgId(org_id);
            info->setCreatorId(uid);
            info->setCreateTime(time(0));
            is_new = true;
        }

        info->setTitle(title);
        info->setDescription(description);
        info->setPriority(priority);
        info->setStatus(status);
        int64_t deadline = request->getParamAs<int64_t>("deadline", 0);
        if (deadline) info->setDeadline(deadline);
        int64_t assignee_id = request->getParamAs<int64_t>("assignee_id", 0);
        if (assignee_id) info->setAssigneeId(assignee_id);
        info->setIsDeleted(0);
        info->setUpdateTime(time(0));

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db error");
            break;
        }

        if (data::RequirementInfoDao::InsertOrUpdate(info, db)) {
            result->setResult(500, "insert or update requirement fail");
            ERROR(logger) << "db error, errno=" << db->getErrno()
                          << ", errstr=" << db->getErrStr();
            break;
        }

        if (is_new) {
            RequirementMgr::GetInstance()->add(info);
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
