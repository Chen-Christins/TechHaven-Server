#include "organization_join_check_servlet.h"
#include <chen/log/log.h>
#include <json/json.h>
#include "../../util.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/notification_manager.h"
#include "../../permission.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

OrganizationJoinCheckServlet::OrganizationJoinCheckServlet()
    : BlogLoginedServlet("OrganizationJoinCheckServlet") {
}

int32_t OrganizationJoinCheckServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, user_id, "user_id");
        DEFINE_AND_CHECK_TYPE(result, int64_t, org_id, "org_id");
        DEFINE_AND_CHECK_TYPE(result, int32_t, state, "state");

        if (state != OrganizationUserRelManager::Status::REJECTED
                && state != OrganizationUserRelManager::Status::APPROVED) {
            result->setErrno(errcode::PARAM_INVALID, "invalid state");
            break;
        }

        auto user = blog::UserMgr::GetInstance()->get(user_id);
        auto org = blog::OrganizationMgr::GetInstance()->get(org_id);
        if (!user || !org) {
            result->setErrno(errcode::ARTICLE_INVALID_ID);
            break;
        }

        // check operater permission
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setErrno(errcode::NOT_LOGIN);
            break;
        }
        auto current_user = blog::UserMgr::GetInstance()->get(uid);
        if (!current_user) {
            result->setErrno(errcode::USER_NOT_FOUND);
            break;
        }
        int32_t system_role = current_user->getRole();

        auto rel = blog::OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        if (!rel) {
            result->setErrno(errcode::ORG_NOT_MEMBER);
            break;
        }
        int32_t org_role = rel->getRole();

        if (!permission::CanManageMembers(system_role, org_role)) {
            result->setErrno(errcode::ACCESS_DENIED);
            break;
        }

        // update user organization relation
        rel = blog::OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, user_id);
        if (!rel) {
            result->setErrno(errcode::ARTICLE_INVALID_ID);
            break;
        }
        if (rel->getStatus() != OrganizationUserRelManager::Status::PENDING) {
            result->setErrno(errcode::ORG_INVALID_STATE);
            break;
        }
        rel->setStatus(state);
        rel->setRole(OrganizationManager::Role::MEMBER);

        auto db = getDB();
        if (!db) {
            result->setErrno(errcode::DB_OPERATION_FAILED);
            break;
        }

        if (data::OrganizationUserRelInfoDao::InsertOrUpdate(rel, db)) {
            result->setErrno(errcode::ORG_USER_REL_FAILED);
            ERROR(logger) << "db error, errno=" << db->getErrno()
                          << ", errstr=" << db->getErrStr();
            break;
        }

        // 通知申请人审批结果
        {
            const char* notif_type = (state == OrganizationUserRelManager::Status::APPROVED)
                ? "org_join_approved" : "org_join_rejected";
            std::string title = (state == OrganizationUserRelManager::Status::APPROVED)
                ? "加入申请已通过" : "加入申请被拒绝";
            std::string content = (state == OrganizationUserRelManager::Status::APPROVED)
                ? "您申请加入组织「" + org->getName() + "」的请求已通过"
                : "您申请加入组织「" + org->getName() + "」的请求已被拒绝";

            auto notif_info = NotificationMgr::GetInstance()->addNotification(
                user_id, title, content, notif_type, uid);
            if (notif_info) {
                Json::Value wsMsg;
                wsMsg["id"] = notif_info->getId();
                wsMsg["title"] = title;
                wsMsg["content"] = content;
                wsMsg["type"] = notif_type;
                wsMsg["is_read"] = false;
                wsMsg["create_time"] = notif_info->getCreateTime();
                NotificationMgr::GetInstance()->sendToUser(
                    user_id, chen::JsonUtil::ToString(wsMsg));
            }
        }

        // 将该组织其他管理员的 org_join_request 通知标记已读，避免上线后看到已处理的通知
        {
            std::vector<data::OrganizationUserRelInfo::ptr> members;
            OrganizationUserRelMgr::GetInstance()->getByPages(members, org_id, 0, 10000, -1, true);
            for (auto& m : members) {
                if (m->getRole() == OrganizationManager::Role::ORG_ADMIN) {
                    NotificationMgr::GetInstance()->markReadByType(
                        m->getUserId(), "org_join_request");
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
