#include "organization_user_switch_role_servlet.h"
#include <chen/log/log.h>
#include <json/json.h>
#include "../../util.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/organization_manager.h"
#include "../../manager/notification_manager.h"
#include "../../permission.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

OrganizationUserSwitchRoleServlet::OrganizationUserSwitchRoleServlet()
    : BlogLoginedServlet("OrganizationUserSwitchRoleServlet") {
}

int32_t OrganizationUserSwitchRoleServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id"); // organization_user_rel id
        DEFINE_AND_CHECK_TYPE(result, int64_t, org_id, "org_id"); // organization id
        DEFINE_AND_CHECK_TYPE(result, int32_t, role, "role"); // new role
        DEFINE_AND_CHECK_TYPE(result, int64_t, user_id, "user_id"); // user_id

        // 操作者权限检查
        int64_t uid = getUserId(request);
        int32_t system_role = blog::UserMgr::GetInstance()->get(uid)->getRole();

        auto rel = blog::OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        int32_t org_role = rel->getRole();

        if (!permission::CanSwitchRole(system_role, org_role, role)) {
            result->setResult(403, "Access Denied");
            break;
        }

        // 更新用户角色
        rel = blog::OrganizationUserRelMgr::GetInstance()->get(id);
        if (!rel || rel->getOrgId() != org_id || rel->getUserId() != user_id) {
            result->setResult(404, "invalid id");;
            break;
        }

        if (role != OrganizationManager::Role::MEMBER
                && role != OrganizationManager::Role::REPORTER
                && role != OrganizationManager::Role::DEVELOPER
                && role != OrganizationManager::Role::DEV_LEAD
                && role != OrganizationManager::Role::ORG_ADMIN) {
            result->setResult(400, "invalid role");
            break;
        }

        rel->setRole(role);
        rel->setUpdateTime(time(0));

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db error");
            break;
        }

        if (data::OrganizationUserRelInfoDao::InsertOrUpdate(rel, db)) {
            result->setResult(500, "insert or update organization user rel fail");
            ERROR(logger) << "db error, errno=" << db->getErrno()
                          << ", errstr=" << db->getErrStr();
            break;
        }

        // 发送角色变更通知给被操作的用户
        {
            auto org = OrganizationMgr::GetInstance()->get(org_id);
            std::string org_name = org ? org->getName() : std::to_string(org_id);
            const char* role_name = "普通成员";
            if (role == OrganizationManager::Role::REPORTER) {
                role_name = "报告者";
            } else if (role == OrganizationManager::Role::DEVELOPER) {
                role_name = "开发者";
            } else if (role == OrganizationManager::Role::DEV_LEAD) {
                role_name = "研发主管";
            } else if (role == OrganizationManager::Role::ORG_ADMIN) {
                role_name = "组织管理员";
            }

            std::string title = "组织角色变更";
            std::string content = "您在组织「" + org_name + "」中的角色已被更新为" + role_name;

            auto notif_info = NotificationMgr::GetInstance()->addNotification(
                user_id, title, content, "org_role_change", uid);

            if (notif_info) {
                Json::Value wsMsg;
                wsMsg["id"] = notif_info->getId();
                wsMsg["title"] = title;
                wsMsg["content"] = content;
                wsMsg["type"] = "org_role_change";
                wsMsg["is_read"] = false;
                wsMsg["create_time"] = notif_info->getCreateTime();
                NotificationMgr::GetInstance()->sendToUser(
                    user_id, chen::JsonUtil::ToString(wsMsg));
            }
        }

        auto user = blog::UserMgr::GetInstance()->get(rel->getUserId());
        result->set("id", rel->getId());
        result->set("user_id", rel->getUserId());
        result->set("name", user->getName());
        result->set("avatar", user->getAvatar());
        result->set("email", user->getEmail());
        result->set("role", rel->getRole());
        result->set("status", rel->getStatus());
        result->set("join_time", rel->getCreateTime());
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
