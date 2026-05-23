#include "rd_my_tickets_servlet.h"
#include "rd_helper.h"
#include "../../manager/requirement_manager.h"
#include "../../manager/bug_manager.h"
#include "../../manager/task_manager.h"

namespace blog {
namespace servlet {

RdMyTicketsServlet::RdMyTicketsServlet()
    : BlogLoginedServlet("RdMyTicketsServlet") {
}

int32_t RdMyTicketsServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        std::string type = request->getParam("type");
        if (type.empty()) {
            result->setResult(400, "param type is required");
            break;
        }

        int64_t org_id = request->getParamAs<int64_t>("org_id", 0);
        uint64_t page = request->getParamAs<uint64_t>("page", 1);
        uint64_t pageSize = request->getParamAs<uint64_t>("page_size", 10);
        uint64_t offset, size;
        rd::parsePagination(page, pageSize, offset, size);

        std::string search = request->getParam("search");
        std::string statusStr = request->getParam("status");

        Json::Value arr(Json::arrayValue);
        uint64_t total = 0;

        if (type == "requirement") {
            std::vector<data::RequirementInfo::ptr> my;
            std::vector<data::RequirementInfo::ptr> all;
            RequirementMgr::GetInstance()->listByPages(all, 0, UINT64_MAX, -1, true);
            for (auto& info : all) {
                if (org_id && info->getOrgId() != org_id) {
                    continue;
                }
                if (info->getCreatorId() != uid && info->getAssigneeId() != uid) {
                    continue;
                }
                if (!statusStr.empty() && rd::stringToRequirementStatus(statusStr) != info->getStatus()) {
                    continue;
                }
                if (!search.empty()) {
                    std::string title = info->getTitle();
                    std::string desc = info->getDescription();
                    if (title.find(search) == std::string::npos
                            && desc.find(search) == std::string::npos) {
                        continue;
                    }
                }
                my.push_back(info);
            }
            total = my.size();
            for (uint64_t i = offset; i < my.size() && arr.size() < size; ++i) {
                Json::Value item;
                auto& info = my[i];
                item["id"] = info->getId();
                item["title"] = info->getTitle();
                item["description"] = info->getDescription();
                item["status"] = rd::requirementStatusToString(info->getStatus());
                item["priority"] = rd::priorityToString(info->getPriority());
                item["creator"] = rd::getUserName(info->getCreatorId());
                item["creator_id"] = info->getCreatorId();
                item["assignee"] = rd::getUserName(info->getAssigneeId());
                item["assignee_avatar"] = rd::getUserAvatar(info->getAssigneeId());
                item["assignee_id"] = info->getAssigneeId();
                item["org_id"] = info->getOrgId();
                item["org_name"] = rd::getOrgName(info->getOrgId());
                item["deadline"] = info->getDeadline();
                item["created_at"] = info->getCreateTime();
                arr.append(item);
            }
        } else if (type == "bug") {
            std::vector<data::BugInfo::ptr> my;
            std::vector<data::BugInfo::ptr> all;
            BugMgr::GetInstance()->listByPages(all, 0, UINT64_MAX, -1, true);
            for (auto& info : all) {
                if (org_id && info->getOrgId() != org_id) {
                    continue;
                }
                if (info->getCreatorId() != uid && info->getAssigneeId() != uid) {
                    continue;
                }
                if (!statusStr.empty() && rd::stringToBugStatus(statusStr) != info->getStatus()) {
                    continue;
                }
                if (!search.empty()) {
                    std::string title = info->getTitle();
                    std::string desc = info->getDescription();
                    if (title.find(search) == std::string::npos
                            && desc.find(search) == std::string::npos) {
                        continue;
                    }
                }
                my.push_back(info);
            }
            total = my.size();
            for (uint64_t i = offset; i < my.size() && arr.size() < size; ++i) {
                Json::Value item;
                auto& info = my[i];
                item["id"] = info->getId();
                item["title"] = info->getTitle();
                item["description"] = info->getDescription();
                item["severity"] = rd::severityToString(info->getSeverity());
                item["priority"] = rd::priorityToString(info->getPriority());
                item["status"] = rd::bugStatusToString(info->getStatus());
                item["creator"] = rd::getUserName(info->getCreatorId());
                item["creator_id"] = info->getCreatorId();
                item["assignee"] = rd::getUserName(info->getAssigneeId());
                item["assignee_avatar"] = rd::getUserAvatar(info->getAssigneeId());
                item["assignee_id"] = info->getAssigneeId();
                item["org_id"] = info->getOrgId();
                item["org_name"] = rd::getOrgName(info->getOrgId());
                item["module"] = info->getModule();
                item["created_at"] = info->getCreateTime();
                arr.append(item);
            }
        } else if (type == "task") {
            std::vector<data::TaskInfo::ptr> my;
            std::vector<data::TaskInfo::ptr> all;
            TaskMgr::GetInstance()->listByPages(all, 0, UINT64_MAX, -1, true);
            for (auto& info : all) {
                if (org_id && info->getOrgId() != org_id) {
                    continue;
                }
                if (info->getCreatorId() != uid && info->getAssigneeId() != uid) {
                    continue;
                }
                if (!statusStr.empty() && rd::stringToTaskStatus(statusStr) != info->getStatus()) {
                    continue;
                }
                if (!search.empty()) {
                    std::string title = info->getTitle();
                    if (title.find(search) == std::string::npos) {
                        continue;
                    }
                }
                my.push_back(info);
            }
            total = my.size();
            for (uint64_t i = offset; i < my.size() && arr.size() < size; ++i) {
                Json::Value item;
                auto& info = my[i];
                item["id"] = info->getId();
                item["title"] = info->getTitle();
                item["description"] = info->getDescription();
                item["status"] = rd::taskStatusToString(info->getStatus());
                item["priority"] = rd::priorityToString(info->getPriority());
                item["creator"] = rd::getUserName(info->getCreatorId());
                item["creator_id"] = info->getCreatorId();
                item["assignee"] = rd::getUserName(info->getAssigneeId());
                item["assignee_avatar"] = rd::getUserAvatar(info->getAssigneeId());
                item["assignee_id"] = info->getAssigneeId();
                item["org_id"] = info->getOrgId();
                item["org_name"] = rd::getOrgName(info->getOrgId());
                item["deadline"] = info->getDeadline();
                item["created_at"] = info->getCreateTime();
                arr.append(item);
            }
        } else {
            result->setResult(400, "invalid type");
            break;
        }

        result->set("list", arr);
        result->set("total", total);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
