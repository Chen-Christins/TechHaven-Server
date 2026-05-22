#ifndef __BLOG_SERVLETS_RD_RD_HELPER_H__
#define __BLOG_SERVLETS_RD_RD_HELPER_H__

#include <string>
#include <memory>

#include <json/json.h>
#include <chen/http/http.h>


#include "blog/data/requirement_info.h"
#include "blog/data/bug_info.h"
#include "blog/data/task_info.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_manager.h"
#include "../../manager/requirement_manager.h"
#include "../../manager/bug_manager.h"
#include "../../manager/task_manager.h"

namespace blog {
namespace servlet {
namespace rd {

// ============================================================================
// 安全 JSON 取值
// ============================================================================
inline int64_t getJsonInt64(const Json::Value& body, const std::string& key, int64_t def = 0) {
    if (body.isNull() || !body.isMember(key)) return def;
    const auto& v = body[key];
    if (v.isInt() || v.isUInt()) return v.asInt64();
    if (v.isString()) {
        std::string s = v.asString();
        if (s.empty()) return def;
        try { return std::stoll(s); } catch (...) { return def; }
    }
    return def;
}

inline std::string getJsonString(const Json::Value& body, const std::string& key, const std::string& def = "") {
    if (body.isNull() || !body.isMember(key)) return def;
    const auto& v = body[key];
    if (v.isString()) return v.asString();
    if (v.isInt() || v.isUInt()) return std::to_string(v.asInt64());
    return def;
}

// ============================================================================
// 参数获取: body 优先，query params 回退（供宏和直接调用使用）
// body 取值若为 0/空，继续回退到 query params
// ============================================================================
inline std::string getParamString(const Json::Value& body, std::shared_ptr<chen::http::HttpRequest> req, const std::string& key) {
    std::string val;
    if (!body.isNull() && body.isMember(key)) {
        const auto& v = body[key];
        if (v.isString()) val = v.asString();
        else val = std::to_string(getJsonInt64(body, key));
    }
    if (val.empty()) {
        val = req->getParam(key);
    }
    return val;
}

inline int64_t getParamInt64(const Json::Value& body, std::shared_ptr<chen::http::HttpRequest> req, const std::string& key) {
    int64_t val = 0;
    if (!body.isNull() && body.isMember(key)) {
        val = getJsonInt64(body, key);
    }
    if (!val) {
        val = req->getParamAs<int64_t>(key, 0);
    }
    return val;
}

// ============================================================================
// 分页: page/page_size → offset/size
// ============================================================================
inline void parsePagination(uint64_t page, uint64_t page_size, uint64_t& offset, uint64_t& size) {
    if (page < 1) page = 1;
    offset = (page - 1) * page_size;
    size = page_size;
}

// ============================================================================
// 需求状态枚举映射
// ============================================================================
inline const char* requirementStatusToString(int32_t status) {
    switch (status) {
    case RequirementManager::STATUS_DRAFT:      return "new";
    case RequirementManager::STATUS_PENDING:    return "developing";
    case RequirementManager::STATUS_INPROGRESS: return "testing";
    case RequirementManager::STATUS_DONE:       return "done";
    case RequirementManager::STATUS_CLOSED:     return "closed";
    default: return "new";
    }
}

inline int32_t stringToRequirementStatus(const std::string& s) {
    if (s == "new")        return RequirementManager::STATUS_DRAFT;
    if (s == "developing") return RequirementManager::STATUS_PENDING;
    if (s == "testing")    return RequirementManager::STATUS_INPROGRESS;
    if (s == "done")       return RequirementManager::STATUS_DONE;
    if (s == "closed")     return RequirementManager::STATUS_CLOSED;
    return -1;
}

// ============================================================================
// 缺陷状态枚举映射
// ============================================================================
inline const char* bugStatusToString(int32_t status) {
    switch (status) {
    case BugManager::STATUS_PENDING:    return "new";
    case BugManager::STATUS_INPROGRESS: return "processing";
    case BugManager::STATUS_FIXED:      return "verified";
    case BugManager::STATUS_CLOSED:     return "closed";
    case BugManager::STATUS_REOPENED:   return "reopened";
    default: return "new";
    }
}

inline int32_t stringToBugStatus(const std::string& s) {
    if (s == "new")        return BugManager::STATUS_PENDING;
    if (s == "processing") return BugManager::STATUS_INPROGRESS;
    if (s == "verified")   return BugManager::STATUS_FIXED;
    if (s == "closed")     return BugManager::STATUS_CLOSED;
    if (s == "reopened")   return BugManager::STATUS_REOPENED;
    return -1;
}

// ============================================================================
// 任务状态枚举映射
// ============================================================================
inline const char* taskStatusToString(int32_t status) {
    switch (status) {
    case TaskManager::STATUS_TODO:       return "todo";
    case TaskManager::STATUS_INPROGRESS: return "doing";
    case TaskManager::STATUS_DONE:       return "done";
    case TaskManager::STATUS_CLOSED:     return "closed";
    default: return "todo";
    }
}

inline int32_t stringToTaskStatus(const std::string& s) {
    if (s == "todo")   return TaskManager::STATUS_TODO;
    if (s == "doing")  return TaskManager::STATUS_INPROGRESS;
    if (s == "done")   return TaskManager::STATUS_DONE;
    if (s == "closed") return TaskManager::STATUS_CLOSED;
    return -1;
}

// ============================================================================
// 优先级枚举映射
// ============================================================================
inline const char* priorityToString(int32_t priority) {
    switch (priority) {
    case 1: return "low";
    case 2: return "medium";
    case 3: return "high";
    case 4: return "urgent";
    default: return "medium";
    }
}

inline int32_t stringToPriority(const std::string& s) {
    if (s == "low")    return 1;
    if (s == "medium") return 2;
    if (s == "high")   return 3;
    if (s == "urgent") return 4;
    return -1;
}

// ============================================================================
// 严重程度枚举映射
// ============================================================================
inline const char* severityToString(int32_t severity) {
    switch (severity) {
    case 1: return "minor";
    case 2: return "normal";
    case 3: return "serious";
    case 4: return "fatal";
    default: return "normal";
    }
}

inline int32_t stringToSeverity(const std::string& s) {
    if (s == "minor")   return 1;
    if (s == "normal")  return 2;
    if (s == "serious") return 3;
    if (s == "fatal")   return 4;
    return -1;
}

// ============================================================================
// 组织角色枚举映射（用于前端展示）
// ============================================================================
inline int32_t orgRoleToFrontend(int32_t role) {
    // 前端期望: 1=报告者, 2=开发者, 3=研发主管, 4=组织管理员
    switch (role) {
    case OrganizationManager::Role::MEMBER:    return 0;  // 普通成员→前端显示0
    case OrganizationManager::Role::REPORTER:  return 1;
    case OrganizationManager::Role::DEVELOPER: return 2;
    case OrganizationManager::Role::DEV_LEAD:  return 3;
    case OrganizationManager::Role::ORG_ADMIN: return 4;
    default: return 0;
    }
}

// ============================================================================
// 构建公共响应字段
// ============================================================================
inline void setUserFields(Json::Value& item, data::UserInfo::ptr user) {
    if (user) {
        item["creator"] = user->getName();
        item["assignee"] = user->getName();
    }
}

inline std::string getOrgName(int64_t orgId) {
    auto org = OrganizationMgr::GetInstance()->get(orgId);
    return org ? org->getName() : "";
}

inline std::string getUserName(int64_t userId) {
    auto user = UserMgr::GetInstance()->get(userId);
    return user ? user->getName() : "";
}

// ============================================================================
// 构建 JSON 响应对象
// ============================================================================
inline void buildRequirementJson(Json::Value& item, data::RequirementInfo::ptr info) {
    item["id"] = info->getId();
    item["title"] = info->getTitle();
    item["description"] = info->getDescription();
    item["priority"] = priorityToString(info->getPriority());
    item["status"] = requirementStatusToString(info->getStatus());
    item["creator"] = getUserName(info->getCreatorId());
    item["creator_id"] = info->getCreatorId();
    item["assignee"] = getUserName(info->getAssigneeId());
    item["assignee_id"] = info->getAssigneeId();
    item["org_id"] = info->getOrgId();
    item["org_name"] = getOrgName(info->getOrgId());
    item["iteration"] = info->getIteration();
    item["category"] = info->getCategory();
    item["source"] = info->getSource();
    item["deadline"] = info->getDeadline();
    item["created_at"] = info->getCreateTime();
    item["updated_at"] = info->getUpdateTime();
}

inline void buildBugJson(Json::Value& item, data::BugInfo::ptr info) {
    item["id"] = info->getId();
    item["title"] = info->getTitle();
    item["description"] = info->getDescription();
    item["severity"] = severityToString(info->getSeverity());
    item["priority"] = priorityToString(info->getPriority());
    item["status"] = bugStatusToString(info->getStatus());
    item["creator"] = getUserName(info->getCreatorId());
    item["creator_id"] = info->getCreatorId();
    item["assignee"] = getUserName(info->getAssigneeId());
    item["assignee_id"] = info->getAssigneeId();
    item["org_id"] = info->getOrgId();
    item["org_name"] = getOrgName(info->getOrgId());
    item["related_requirement_id"] = info->getRequirementId();
    item["module"] = info->getModule();
    item["steps_to_reproduce"] = info->getStepsToReproduce();
    item["environment"] = info->getEnvironment();
    item["created_at"] = info->getCreateTime();
    item["updated_at"] = info->getUpdateTime();
}

inline void buildTaskJson(Json::Value& item, data::TaskInfo::ptr info) {
    item["id"] = info->getId();
    item["title"] = info->getTitle();
    item["description"] = info->getDescription();
    item["status"] = taskStatusToString(info->getStatus());
    item["priority"] = priorityToString(info->getPriority());
    item["creator"] = getUserName(info->getCreatorId());
    item["creator_id"] = info->getCreatorId();
    item["assignee"] = getUserName(info->getAssigneeId());
    item["assignee_id"] = info->getAssigneeId();
    item["org_id"] = info->getOrgId();
    item["org_name"] = getOrgName(info->getOrgId());
    item["requirement_id"] = info->getRequirementId();
    item["bug_id"] = info->getBugId();
    item["deadline"] = info->getDeadline();
    item["estimated_hours"] = info->getEstimatedHours();
    item["created_at"] = info->getCreateTime();
    item["updated_at"] = info->getUpdateTime();
}

} // namespace rd
} // namespace servlet
} // namespace blog

#endif // __BLOG_SERVLETS_RD_RD_HELPER_H__
