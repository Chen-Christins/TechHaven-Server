#include "notification_send_servlet.h"
#include "../../manager/notification_manager.h"
#include "../../manager/user_manager.h"
#include "../../util.h"

#include <chen/log/log.h>
#include <json/json.h>

#include <sstream>
#include <vector>

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

NotificationSendServlet::NotificationSendServlet()
    :BlogLoginedServlet("NotificationSendServlet") {
}

int32_t NotificationSendServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
		,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        // 权限检查: 仅管理员可发送通知
        int64_t uid = getUserId(request);
        data::UserInfo::ptr uinfo = UserMgr::GetInstance()->get(uid);
        if (!uinfo || uinfo->getRole() != UserManager::ADMIN) {
            result->setResult(403, "permission denied");
            break;
        }

        DEFINE_AND_CHECK_STRING(result, title, "title");
        DEFINE_AND_CHECK_STRING(result, content, "content");
        DEFINE_AND_CHECK_STRING(result, type, "type");
        DEFINE_AND_CHECK_STRING(result, target, "target");

        if (title.size() > 100) {
            result->setResult(400, "title too long (max 100)");
            break;
        }
        if (content.size() > 2000) {
            result->setResult(400, "content too long (max 2000)");
            break;
        }

        // 构建通知 JSON
        Json::Value notif;
        notif["type"] = "notification";
        notif["notification_type"] = type;
        notif["title"] = title;
        notif["content"] = content;
        notif["create_time"] = (int64_t)time(0);
        std::string msg = chen::JsonUtil::ToString(notif);

        auto& notifMgr = *NotificationMgr::GetInstance();

        if (target == "all") {
            // 广播: 为所有在线用户各插一条通知记录
            std::vector<int64_t> user_ids;
            UserMgr::GetInstance()->getAllIds(user_ids, true);
            for (auto uid : user_ids) {
                notifMgr.addNotification(uid, title, content, type, uid);
            }
            notifMgr.broadcast(msg);
            INFO(logger) << "[NOTIFY] broadcast: type=" << type
                << " title=" << title << " count=" << user_ids.size();
        } else if (target == "users") {
            std::string user_ids_str = request->getParam("user_ids");
            if (user_ids_str.empty()) {
                result->setResult(400, "user_ids required when target=users");
                break;
            }

            std::vector<int64_t> ids;
            std::istringstream iss(user_ids_str);
            std::string token;
            while (std::getline(iss, token, ',')) {
                if (!token.empty()) {
                    ids.push_back(std::stoll(token));
                }
            }

            int32_t sent = 0;
            for (auto id : ids) {
                notifMgr.addNotification(id, title, content, type, uid);
                if (notifMgr.sendToUser(id, msg) == 0) {
                    sent++;
                }
            }
            INFO(logger) << "[NOTIFY] send to users: count=" << ids.size()
                << " sent=" << sent << " type=" << type << " title=" << title;
        } else {
            result->setResult(400, "invalid target, must be 'all' or 'users'");
            break;
        }

        result->setResult(200, "ok");
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}
}
}
