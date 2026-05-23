#include "rd_task_edit_servlet.h"
#include "rd_helper.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/task_manager.h"
#include "../../permission.h"
#include "../../util.h"

namespace blog {
namespace servlet {

RdTaskEditServlet::RdTaskEditServlet()
    : BlogLoginedServlet("RdTaskEditServlet") {
}

int32_t RdTaskEditServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t id = request->getParamAs<int64_t>("id", 0);
        DEFINE_AND_CHECK_TYPE(result, int64_t, org_id, "org_id");

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }

        auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
            result->setResult(403, "not a member of this organization");
            break;
        }
        int32_t orgRole = rel->getRole();

        bool is_new = false;
        data::TaskInfo::ptr info;
        if (id) {
            info = TaskMgr::GetInstance()->get(id);
            if (!info || info->getOrgId() != org_id) {
                result->setResult(404, "task not exist");
                break;
            }
            if (!permission::canEditTask(orgRole, uid, info->getAssigneeId())) {
                result->setResult(403, "Access Denied");
                break;
            }
        } else {
            if (!permission::canCreateTask(orgRole)) {
                result->setResult(403, "Access Denied");
                break;
            }
            info.reset(new data::TaskInfo);
            info->setOrgId(org_id);
            info->setCreatorId(uid);
            info->setCreateTime(time(0));
            is_new = true;
        }

        DEFINE_AND_CHECK_STRING(result, title, "title");
        if (!title.empty()) {
            info->setTitle(title);
        }
        
        std::string desc = request->getParam("description");
        if (!desc.empty()) {
            info->setDescription(desc);
        }
        
        std::string priorityStr = request->getParam("priority");
        if (!priorityStr.empty()) {
            int32_t p = rd::stringToPriority(priorityStr);
            if (p >= 0) {
                info->setPriority(p);
            }
        }

        std::string statusStr = request->getParam("status");
        if (!statusStr.empty()) {
            int32_t s = rd::stringToTaskStatus(statusStr);
            if (s >= 0) {
                info->setStatus(s);
            }
        }

        int64_t assignee_id = request->getParamAs<int64_t>("assignee_id", 0);
        if (assignee_id) {
            info->setAssigneeId(assignee_id);
        }

        int64_t requirement_id = request->getParamAs<int64_t>("requirement_id", 0);
        if (requirement_id) {
            info->setRequirementId(requirement_id);
        }

        int64_t bug_id = request->getParamAs<int64_t>("bug_id", 0);
        if (bug_id) {
            info->setBugId(bug_id);
        }

        int64_t deadline = request->getParamAs<int64_t>("deadline", 0);
        if (deadline) {
            info->setDeadline(deadline);
        }

        int64_t estimated_hours = request->getParamAs<int64_t>("estimated_hours", 0);
        if (estimated_hours) {
            info->setEstimatedHours(estimated_hours);
        }

        info->setIsDeleted(0);
        info->setUpdateTime(time(0));

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db error");
            break;
        }
        if (data::TaskInfoDao::InsertOrUpdate(info, db)) {
            result->setResult(500, "insert or update task fail");
            break;
        }
        if (is_new) {
            TaskMgr::GetInstance()->add(info);
        }
        rd::buildTaskJson(result->jsondata, info);
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}
