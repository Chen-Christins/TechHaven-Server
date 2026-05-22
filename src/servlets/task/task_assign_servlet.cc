#include "task_assign_servlet.h"
#include <chen/log/log.h>
#include "../../util.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/task_manager.h"
#include "../../permission.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

TaskAssignServlet::TaskAssignServlet()
    : BlogLoginedServlet("TaskAssignServlet") {
}

int32_t TaskAssignServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        DEFINE_AND_CHECK_TYPE(result, int64_t, org_id, "org_id");
        DEFINE_AND_CHECK_TYPE(result, int64_t, assignee_id, "assignee_id");

        int64_t uid = getUserId(request);

        auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
            result->setResult(403, "not a member of this organization");
            break;
        }
        int32_t orgRole = rel->getRole();

        if (!permission::canAssignTask(orgRole)) {
            result->setResult(403, "Access Denied");
            break;
        }

        auto targetRel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, assignee_id);
        if (!targetRel || targetRel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
            result->setResult(400, "assignee not in organization");
            break;
        }

        auto info = TaskMgr::GetInstance()->get(id);
        if (!info || info->getIsDeleted() || info->getOrgId() != org_id) {
            result->setResult(404, "task not exist");
            break;
        }

        info->setAssigneeId(assignee_id);
        info->setUpdateTime(time(0));

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db error");
            break;
        }

        if (data::TaskInfoDao::InsertOrUpdate(info, db)) {
            result->setResult(500, "update task fail");
            ERROR(logger) << "db error, errno=" << db->getErrno()
                          << ", errstr=" << db->getErrStr();
            break;
        }

        result->set("id", info->getId());
        result->set("assignee_id", info->getAssigneeId());
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
