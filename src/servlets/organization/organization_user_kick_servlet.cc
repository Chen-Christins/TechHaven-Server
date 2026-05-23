#include "organization_user_kick_servlet.h"
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

OrganizationUserKickServlet::OrganizationUserKickServlet()
    : BlogLoginedServlet("OrganizationUserKickServlet") {
}

int32_t OrganizationUserKickServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id"); // organization_user_rel id
        DEFINE_AND_CHECK_TYPE(result, int64_t, org_id, "org_id"); // organization id
        DEFINE_AND_CHECK_TYPE(result, int64_t, user_id, "user_id"); // user_id

        // 操作者权限检查
        int64_t uid = getUserId(request);
        int32_t system_role = blog::UserMgr::GetInstance()->get(uid)->getRole();

        auto rel = blog::OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        int32_t org_role = rel->getRole();

        if (!permission::canManageMembers(system_role, org_role)) {
            result->setResult(403, "Access Denied");
            break;
        }
        
        // 踢出用户
        rel = blog::OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, user_id);
        if (!rel) {
            result->setResult(404, "invalid id");
            break;
        }
        
        rel->setStatus(OrganizationUserRelManager::Status::EXITED);
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

        // 通知组织管理员及被踢出的用户
        {
            auto org = OrganizationMgr::GetInstance()->get(org_id);
            auto kickedUser = UserMgr::GetInstance()->get(user_id);
            auto operUser = UserMgr::GetInstance()->get(uid);
            std::string org_name = org ? org->getName() : std::to_string(org_id);
            std::string kicked_name = kickedUser ? kickedUser->getName() : std::to_string(user_id);
            std::string oper_name = operUser ? operUser->getName() : std::to_string(uid);

            std::string title = "成员被移出组织";
            std::string content = "「" + oper_name + "」将「" + kicked_name + "」移出了组织「" + org_name + "」";

            // 通知所有管理员及拥有者
            std::vector<data::OrganizationUserRelInfo::ptr> members;
            OrganizationUserRelMgr::GetInstance()->getByPages(members, org_id, 0, 10000, -1, true);
            for (auto& m : members) {
                if (m->getRole() == OrganizationManager::Role::ORG_ADMIN) {
                    auto notifInfo = NotificationMgr::GetInstance()->addNotification(
                        m->getUserId(), title, content, "org_member_kicked", uid);
                    if (notifInfo) {
                        Json::Value wsMsg;
                        wsMsg["id"] = notifInfo->getId();
                        wsMsg["title"] = title;
                        wsMsg["content"] = content;
                        wsMsg["type"] = "org_member_kicked";
                        wsMsg["is_read"] = false;
                        wsMsg["create_time"] = notifInfo->getCreateTime();
                        NotificationMgr::GetInstance()->sendToUser(
                            m->getUserId(), chen::JsonUtil::ToString(wsMsg));
                    }
                }
            }

            // 通知被踢出的用户
            {
                std::string kicked_title = "您已被移出组织";
                std::string kicked_content = "您已被移出组织「" + org_name + "」";
                auto notifInfo = NotificationMgr::GetInstance()->addNotification(
                    user_id, kicked_title, kicked_content, "org_member_kicked", uid);
                if (notifInfo) {
                    Json::Value wsMsg;
                    wsMsg["id"] = notifInfo->getId();
                    wsMsg["title"] = kicked_title;
                    wsMsg["content"] = kicked_content;
                    wsMsg["type"] = "org_member_kicked";
                    wsMsg["is_read"] = false;
                    wsMsg["create_time"] = notifInfo->getCreateTime();
                    NotificationMgr::GetInstance()->sendToUser(
                        user_id, chen::JsonUtil::ToString(wsMsg));
                }
            }
        }

        result->set("success", true);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
