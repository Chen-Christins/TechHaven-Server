#ifndef __BLOG_PERMISSION_H__
#define __BLOG_PERMISSION_H__

#include "manager/user_manager.h"
#include "manager/organization_manager.h"

namespace blog {
namespace permission {

using OrgRole = OrganizationManager::Role;

// ============================================================================
// 成员管理权限
// ============================================================================

/// 是否可以管理组织成员（审批加入、踢人、设角色）
inline bool canManageMembers(int32_t systemRole, int32_t orgRole) {
    if (systemRole == UserManager::Role::ADMIN) return true;
    if (orgRole == OrgRole::ORG_ADMIN || orgRole == OrgRole::DEV_LEAD) return true;
    return false;
}

/// 是否可以将成员切换到目标角色
inline bool canSwitchRole(int32_t systemRole, int32_t orgRole, int32_t targetRole) {
    if (systemRole == UserManager::Role::ADMIN) return true;
    if (orgRole == OrgRole::ORG_ADMIN) return true;
    // 研发主管可以将成员设置为: 普通成员、报告者、开发者
    if (orgRole == OrgRole::DEV_LEAD
            && (targetRole == OrgRole::MEMBER
             || targetRole == OrgRole::REPORTER
             || targetRole == OrgRole::DEVELOPER)) {
        return true;
    }
    return false;
}

// ============================================================================
// 需求 (Requirement) 权限
// ============================================================================

/// 查看需求
inline bool canViewRequirement(int32_t orgRole, int64_t userId, int64_t creatorId) {
    if (orgRole >= OrgRole::DEVELOPER) return true;   // 开发者及以上看全部
    if (orgRole == OrgRole::REPORTER) return userId == creatorId; // 报告者看自己的
    return true; // 普通成员看全部（组织透明）
}

/// 创建需求: 报告者及以上
inline bool canCreateRequirement(int32_t orgRole) {
    return orgRole >= OrgRole::REPORTER;
}

/// 编辑需求: 研发主管及以上
inline bool canEditRequirement(int32_t orgRole) {
    return orgRole >= OrgRole::DEV_LEAD;
}

/// 删除需求: 研发主管及以上
inline bool canDeleteRequirement(int32_t orgRole) {
    return orgRole >= OrgRole::DEV_LEAD;
}

/// 分配需求负责人: 研发主管及以上
inline bool canAssignRequirement(int32_t orgRole) {
    return orgRole >= OrgRole::DEV_LEAD;
}

// ============================================================================
// 缺陷 (Bug) 权限
// ============================================================================

/// 查看缺陷
inline bool canViewBug(int32_t orgRole, int64_t userId, int64_t creatorId) {
    if (orgRole >= OrgRole::DEVELOPER) return true;    // 开发者及以上看全部
    if (orgRole == OrgRole::REPORTER) return userId == creatorId; // 报告者看自己的
    return true; // 普通成员看全部
}

/// 创建缺陷: 报告者及以上
inline bool canCreateBug(int32_t orgRole) {
    return orgRole >= OrgRole::REPORTER;
}

/// 编辑缺陷: 开发者只能编辑自己创建的，研发主管及以上编辑全部
inline bool canEditBug(int32_t orgRole, int64_t userId, int64_t creatorId) {
    if (orgRole >= OrgRole::DEV_LEAD) return true;
    if (orgRole == OrgRole::DEVELOPER) return userId == creatorId;
    return false;
}

/// 删除缺陷: 研发主管及以上
inline bool canDeleteBug(int32_t orgRole) {
    return orgRole >= OrgRole::DEV_LEAD;
}

/// 分配缺陷负责人: 研发主管及以上
inline bool canAssignBug(int32_t orgRole) {
    return orgRole >= OrgRole::DEV_LEAD;
}

// ============================================================================
// 任务 (Task) 权限
// ============================================================================

/// 查看任务
/// @param isRelated 用户是否为任务的创建者、负责人，或关联需求/缺陷的创建者/负责人
inline bool canViewTask(int32_t orgRole, bool isRelated) {
    if (orgRole >= OrgRole::DEV_LEAD) return true;    // 研发主管及以上看全部
    if (orgRole >= OrgRole::DEVELOPER) return isRelated; // 开发者看相关的
    return true; // 普通成员看全部（组织透明）
}

/// 创建任务: 研发主管及以上
inline bool canCreateTask(int32_t orgRole) {
    return orgRole >= OrgRole::DEV_LEAD;
}

/// 编辑任务: 开发者编辑自己负责的，研发主管及以上编辑全部
inline bool canEditTask(int32_t orgRole, int64_t userId, int64_t assigneeId) {
    if (orgRole >= OrgRole::DEV_LEAD) return true;
    if (orgRole == OrgRole::DEVELOPER) return userId == assigneeId;
    return false;
}

/// 删除任务: 研发主管及以上
inline bool canDeleteTask(int32_t orgRole) {
    return orgRole >= OrgRole::DEV_LEAD;
}

/// 分配任务负责人: 研发主管及以上
inline bool canAssignTask(int32_t orgRole) {
    return orgRole >= OrgRole::DEV_LEAD;
}

} // namespace permission
} // namespace blog

#endif // __BLOG_PERMISSION_H__
