#include "rd_special_servlet.h"
#include "rd_helper.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/organization_manager.h"
#include "../../manager/requirement_manager.h"
#include "../../manager/bug_manager.h"
#include "../../manager/task_manager.h"

namespace blog {
namespace servlet {

// ============================================================================
// RdStatsServlet — GET /api/v1/rd/stats
// ============================================================================

RdStatsServlet::RdStatsServlet()
    : BlogLoginedServlet("RdStatsServlet") {
}

int32_t RdStatsServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t org_id = request->getParamAs<int64_t>("org_id", 0);
        int64_t uid = getUserId(request);
        int32_t systemRole = UserMgr::GetInstance()->get(uid)->getRole();
        bool isPlatformAdmin = (systemRole == UserManager::Role::ADMIN);
        time_t now = time(0);

        // 收集所有数据
        std::vector<data::RequirementInfo::ptr> reqs;
        std::vector<data::BugInfo::ptr> bugs;
        std::vector<data::TaskInfo::ptr> tasks;

        if (isPlatformAdmin) {
            if (org_id) {
                RequirementMgr::GetInstance()->listByOrg(reqs, org_id, 0, UINT64_MAX, -1, true);
                BugMgr::GetInstance()->listByOrg(bugs, org_id, 0, UINT64_MAX, -1, true);
                TaskMgr::GetInstance()->listByOrg(tasks, org_id, 0, UINT64_MAX, -1, true);
            } else {
                RequirementMgr::GetInstance()->listByPages(reqs, 0, UINT64_MAX, -1, true);
                BugMgr::GetInstance()->listByPages(bugs, 0, UINT64_MAX, -1, true);
                TaskMgr::GetInstance()->listByPages(tasks, 0, UINT64_MAX, -1, true);
            }
        } else {
            std::vector<data::OrganizationUserRelInfo::ptr> userOrgs;
            OrganizationUserRelMgr::GetInstance()->getOrgByUserId(userOrgs, uid, OrganizationUserRelManager::Status::APPROVED, true);
            for (auto& rel : userOrgs) {
                if (org_id && rel->getOrgId() != org_id) continue;
                std::vector<data::RequirementInfo::ptr> oReqs;
                RequirementMgr::GetInstance()->listByOrg(oReqs, rel->getOrgId(), 0, UINT64_MAX, -1, true);
                reqs.insert(reqs.end(), oReqs.begin(), oReqs.end());
                std::vector<data::BugInfo::ptr> oBugs;
                BugMgr::GetInstance()->listByOrg(oBugs, rel->getOrgId(), 0, UINT64_MAX, -1, true);
                bugs.insert(bugs.end(), oBugs.begin(), oBugs.end());
                std::vector<data::TaskInfo::ptr> oTasks;
                TaskMgr::GetInstance()->listByOrg(oTasks, rel->getOrgId(), 0, UINT64_MAX, -1, true);
                tasks.insert(tasks.end(), oTasks.begin(), oTasks.end());
            }
        }

        // 需求统计
        int total_reqs = 0, open_reqs = 0;
        for (auto& info : reqs) {
            total_reqs++;
            int st = info->getStatus();
            if (st != RequirementManager::STATUS_DONE && st != RequirementManager::STATUS_CLOSED) {
                open_reqs++;
            }
        }

        // 缺陷统计
        int total_bugs = 0, unresolved_bugs = 0;
        for (auto& info : bugs) {
            total_bugs++;
            int st = info->getStatus();
            if (st != BugManager::STATUS_FIXED && st != BugManager::STATUS_CLOSED) {
                unresolved_bugs++;
            }
        }

        // 任务统计
        int total_tasks = 0, overdue_tasks = 0;
        for (auto& info : tasks) {
            total_tasks++;
            int st = info->getStatus();
            if (st != TaskManager::STATUS_DONE && st != TaskManager::STATUS_CLOSED
                    && info->getDeadline() > 0 && info->getDeadline() < now) {
                overdue_tasks++;
            }
        }

        result->jsondata["total_requirements"] = total_reqs;
        result->jsondata["open_requirements"] = open_reqs;
        result->jsondata["total_bugs"] = total_bugs;
        result->jsondata["unresolved_bugs"] = unresolved_bugs;
        result->jsondata["total_tasks"] = total_tasks;
        result->jsondata["overdue_tasks"] = overdue_tasks;

    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

// ============================================================================
// RdMyTicketsServlet — GET /api/v1/rd/my-tickets
// ============================================================================

RdMyTicketsServlet::RdMyTicketsServlet()
    : BlogLoginedServlet("RdMyTicketsServlet") {
}

int32_t RdMyTicketsServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        std::string type = request->getParam("type");  // requirement/bug, empty = all
        int64_t org_id = request->getParamAs<int64_t>("org_id", 0);

        // 需求
        if (type.empty() || type == "requirement") {
            Json::Value arr(Json::arrayValue);
            std::vector<data::RequirementInfo::ptr> all;
            RequirementMgr::GetInstance()->listByPages(all, 0, UINT64_MAX, -1, true);
            for (auto& info : all) {
                if (org_id && info->getOrgId() != org_id) continue;
                if (info->getCreatorId() != uid && info->getAssigneeId() != uid) continue;
                Json::Value item;
                item["id"] = info->getId();
                item["title"] = info->getTitle();
                item["description"] = info->getDescription();
                item["status"] = rd::requirementStatusToString(info->getStatus());
                item["priority"] = rd::priorityToString(info->getPriority());
                item["creator"] = rd::getUserName(info->getCreatorId());
                item["creator_id"] = info->getCreatorId();
                item["assignee"] = rd::getUserName(info->getAssigneeId());
                item["assignee_id"] = info->getAssigneeId();
                item["org_id"] = info->getOrgId();
                item["org_name"] = rd::getOrgName(info->getOrgId());
                item["deadline"] = info->getDeadline();
                item["created_at"] = info->getCreateTime();
                arr.append(item);
            }
            result->jsondata["requirements"] = arr;
        }

        // 缺陷
        if (type.empty() || type == "bug") {
            Json::Value arr(Json::arrayValue);
            std::vector<data::BugInfo::ptr> all;
            BugMgr::GetInstance()->listByPages(all, 0, UINT64_MAX, -1, true);
            for (auto& info : all) {
                if (org_id && info->getOrgId() != org_id) continue;
                if (info->getCreatorId() != uid && info->getAssigneeId() != uid) continue;
                Json::Value item;
                item["id"] = info->getId();
                item["title"] = info->getTitle();
                item["description"] = info->getDescription();
                item["severity"] = rd::severityToString(info->getSeverity());
                item["priority"] = rd::priorityToString(info->getPriority());
                item["status"] = rd::bugStatusToString(info->getStatus());
                item["creator"] = rd::getUserName(info->getCreatorId());
                item["creator_id"] = info->getCreatorId();
                item["assignee"] = rd::getUserName(info->getAssigneeId());
                item["assignee_id"] = info->getAssigneeId();
                item["org_id"] = info->getOrgId();
                item["org_name"] = rd::getOrgName(info->getOrgId());
                item["module"] = info->getModule();
                item["created_at"] = info->getCreateTime();
                arr.append(item);
            }
            result->jsondata["bugs"] = arr;
        }

    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

// ============================================================================
// RdOrganizationsServlet — GET /api/v1/rd/organizations
// ============================================================================

RdOrganizationsServlet::RdOrganizationsServlet()
    : BlogLoginedServlet("RdOrganizationsServlet") {
}

int32_t RdOrganizationsServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);

        std::vector<data::OrganizationUserRelInfo::ptr> userOrgs;
        OrganizationUserRelMgr::GetInstance()->getOrgByUserId(userOrgs, uid, -1, true);

        Json::Value arr(Json::arrayValue);
        for (auto& rel : userOrgs) {
            auto org = OrganizationMgr::GetInstance()->get(rel->getOrgId());
            if (!org) continue;

            Json::Value item;
            item["org_id"] = org->getId();
            item["org_name"] = org->getName();
            item["role"] = rel->getRole();
            arr.append(item);
        }

        result->set("list", arr);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

// ============================================================================
// RdEnumsServlet — GET /api/v1/rd/enums
// ============================================================================

RdEnumsServlet::RdEnumsServlet()
    : BlogLoginedServlet("RdEnumsServlet") {
}

int32_t RdEnumsServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        // 状态枚举
        Json::Value reqStatus(Json::arrayValue);
        reqStatus.append(Json::Value(Json::objectValue));
        reqStatus[0]["label"] = "新建"; reqStatus[0]["value"] = "new";
        reqStatus.append(Json::Value(Json::objectValue));
        reqStatus[1]["label"] = "开发中"; reqStatus[1]["value"] = "developing";
        reqStatus.append(Json::Value(Json::objectValue));
        reqStatus[2]["label"] = "测试中"; reqStatus[2]["value"] = "testing";
        reqStatus.append(Json::Value(Json::objectValue));
        reqStatus[3]["label"] = "已完成"; reqStatus[3]["value"] = "done";
        reqStatus.append(Json::Value(Json::objectValue));
        reqStatus[4]["label"] = "已关闭"; reqStatus[4]["value"] = "closed";
        result->jsondata["requirement_status"] = reqStatus;

        Json::Value bugStatus(Json::arrayValue);
        bugStatus.append(Json::Value(Json::objectValue));
        bugStatus[0]["label"] = "新建"; bugStatus[0]["value"] = "new";
        bugStatus.append(Json::Value(Json::objectValue));
        bugStatus[1]["label"] = "处理中"; bugStatus[1]["value"] = "processing";
        bugStatus.append(Json::Value(Json::objectValue));
        bugStatus[2]["label"] = "已验证"; bugStatus[2]["value"] = "verified";
        bugStatus.append(Json::Value(Json::objectValue));
        bugStatus[3]["label"] = "已关闭"; bugStatus[3]["value"] = "closed";
        bugStatus.append(Json::Value(Json::objectValue));
        bugStatus[4]["label"] = "重新打开"; bugStatus[4]["value"] = "reopened";
        result->jsondata["bug_status"] = bugStatus;

        Json::Value taskStatus(Json::arrayValue);
        taskStatus.append(Json::Value(Json::objectValue));
        taskStatus[0]["label"] = "待办"; taskStatus[0]["value"] = "todo";
        taskStatus.append(Json::Value(Json::objectValue));
        taskStatus[1]["label"] = "进行中"; taskStatus[1]["value"] = "doing";
        taskStatus.append(Json::Value(Json::objectValue));
        taskStatus[2]["label"] = "已完成"; taskStatus[2]["value"] = "done";
        taskStatus.append(Json::Value(Json::objectValue));
        taskStatus[3]["label"] = "已关闭"; taskStatus[3]["value"] = "closed";
        result->jsondata["task_status"] = taskStatus;

        // 优先级枚举
        Json::Value priority(Json::arrayValue);
        priority.append(Json::Value(Json::objectValue));
        priority[0]["label"] = "低"; priority[0]["value"] = "low";
        priority.append(Json::Value(Json::objectValue));
        priority[1]["label"] = "中"; priority[1]["value"] = "medium";
        priority.append(Json::Value(Json::objectValue));
        priority[2]["label"] = "高"; priority[2]["value"] = "high";
        priority.append(Json::Value(Json::objectValue));
        priority[3]["label"] = "紧急"; priority[3]["value"] = "urgent";
        result->jsondata["priority"] = priority;

        // 严重程度枚举
        Json::Value severity(Json::arrayValue);
        severity.append(Json::Value(Json::objectValue));
        severity[0]["label"] = "轻微"; severity[0]["value"] = "minor";
        severity.append(Json::Value(Json::objectValue));
        severity[1]["label"] = "一般"; severity[1]["value"] = "normal";
        severity.append(Json::Value(Json::objectValue));
        severity[2]["label"] = "严重"; severity[2]["value"] = "serious";
        severity.append(Json::Value(Json::objectValue));
        severity[3]["label"] = "致命"; severity[3]["value"] = "fatal";
        result->jsondata["severity"] = severity;

        // 需求分类枚举
        Json::Value category(Json::arrayValue);
        category.append(Json::Value(Json::objectValue));
        category[0]["label"] = "功能需求"; category[0]["value"] = "feature";
        category.append(Json::Value(Json::objectValue));
        category[1]["label"] = "技术需求"; category[1]["value"] = "tech";
        category.append(Json::Value(Json::objectValue));
        category[2]["label"] = "优化需求"; category[2]["value"] = "optimization";
        category.append(Json::Value(Json::objectValue));
        category[3]["label"] = "缺陷修复"; category[3]["value"] = "bugfix";
        result->jsondata["category"] = category;

        // 需求来源枚举
        Json::Value source(Json::arrayValue);
        source.append(Json::Value(Json::objectValue));
        source[0]["label"] = "产品"; source[0]["value"] = "product";
        source.append(Json::Value(Json::objectValue));
        source[1]["label"] = "客户"; source[1]["value"] = "customer";
        source.append(Json::Value(Json::objectValue));
        source[2]["label"] = "内部"; source[2]["value"] = "internal";
        source.append(Json::Value(Json::objectValue));
        source[3]["label"] = "其他"; source[3]["value"] = "other";
        result->jsondata["source"] = source;

        // 组织角色枚举
        Json::Value orgRole(Json::arrayValue);
        orgRole.append(Json::Value(Json::objectValue));
        orgRole[0]["label"] = "普通成员"; orgRole[0]["value"] = 1;
        orgRole.append(Json::Value(Json::objectValue));
        orgRole[1]["label"] = "报告者"; orgRole[1]["value"] = 2;
        orgRole.append(Json::Value(Json::objectValue));
        orgRole[2]["label"] = "开发者"; orgRole[2]["value"] = 3;
        orgRole.append(Json::Value(Json::objectValue));
        orgRole[3]["label"] = "研发主管"; orgRole[3]["value"] = 4;
        orgRole.append(Json::Value(Json::objectValue));
        orgRole[4]["label"] = "组织管理员"; orgRole[4]["value"] = 5;
        result->jsondata["roles"] = orgRole;

    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
