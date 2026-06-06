#include "user_admin_reset_passwd_servlet.h"
#include <chen/log/log.h>
#include "../../manager/user_manager.h"
#include "../../manager/notification_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

UserAdminResetPasswdServlet::UserAdminResetPasswdServlet()
    : BlogLoginedServlet("UserAdminResetPasswdServlet") {
}

int32_t UserAdminResetPasswdServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        DEFINE_AND_CHECK_STRING(result, passwd_f, "passwd_f");
        DEFINE_AND_CHECK_STRING(result, passwd_s, "passwd_s");

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }
        int32_t role = UserMgr::GetInstance()->get(uid)->getRole();

        if (role != UserManager::Role::ADMIN) {
            result->setResult(403, "Access Denied");
            break;
        }

        if (passwd_f.empty() || passwd_s.empty()) {
            result->setResult(400, "param passwd empty");
            break;
        }

        if (passwd_f != passwd_s) {
            result->setResult(400, "the passwords is different");
            break;
        }

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db connection fail");
            break;
        }

        chen::ITransaction::ptr trans = db->openTransaction();
        data::UserInfo::ptr info = UserMgr::GetInstance()->get(id);
        info->setPasswd(chen::md5(passwd_s));

        if (data::UserInfoDao::Update(info, db)) {
            result->setResult(500, "insert user fail");
            break;
        }
        trans->commit();

        // Notify affected user
        chen::IOManager::GetThis()->schedule([id]() {
            std::string title = "密码已被重置";
            std::string content = "你的账户密码已被管理员重置，请尽快修改密码";
            auto notif = NotificationMgr::GetInstance()->addNotification(
                id, title, content, "password_reset", 0);
            if (notif) {
                Json::Value wsMsg;
                wsMsg["id"] = notif->getId();
                wsMsg["title"] = title;
                wsMsg["content"] = content;
                wsMsg["type"] = "password_reset";
                wsMsg["is_read"] = false;
                wsMsg["create_time"] = notif->getCreateTime();
                NotificationMgr::GetInstance()->sendToUser(id, chen::JsonUtil::ToString(wsMsg));
            }
        });
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
