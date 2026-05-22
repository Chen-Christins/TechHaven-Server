#include "task_list_servlet.h"
#include <chen/log/log.h>
#include "../../util.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/task_manager.h"
#include "../../permission.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

TaskListServlet::TaskListServlet()
    : BlogLoginedServlet("TaskListServlet") {
}

int32_t TaskListServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, org_id, "org_id");
        int32_t status = request->getParamAs<int32_t>("status", -1);
        uint64_t offset = request->getParamAs<uint64_t>("offset", 0);
        uint64_t size = request->getParamAs<uint64_t>("size", 20);

        int64_t uid = getUserId(request);

        auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
            result->setResult(403, "not a member of this organization");
            break;
        }
        int32_t orgRole = rel->getRole();

        std::vector<data::TaskInfo::ptr> all;
        TaskMgr::GetInstance()->listByOrg(all, org_id, 0, UINT64_MAX, status, true);

        std::vector<data::TaskInfo::ptr> filtered;
        for (auto& info : all) {
            bool isRelated = (uid == info->getCreatorId()
                           || uid == info->getAssigneeId());
            if (permission::canViewTask(orgRole, isRelated)) {
                filtered.push_back(info);
            }
        }

        uint64_t total = filtered.size();
        Json::Value arr(Json::arrayValue);
        for (uint64_t i = offset; i < filtered.size() && arr.size() < size; ++i) {
            auto& info = filtered[i];
            Json::Value item;
            item["id"] = info->getId();
            item["org_id"] = info->getOrgId();
            item["title"] = info->getTitle();
            item["description"] = info->getDescription();
            item["priority"] = info->getPriority();
            item["status"] = info->getStatus();
            item["creator_id"] = info->getCreatorId();
            item["assignee_id"] = info->getAssigneeId();
            item["requirement_id"] = info->getRequirementId();
            item["bug_id"] = info->getBugId();
            item["deadline"] = info->getDeadline();
            item["create_time"] = info->getCreateTime();
            item["update_time"] = info->getUpdateTime();
            arr.append(item);
        }

        result->set("total", total);
        result->set("list", arr);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
