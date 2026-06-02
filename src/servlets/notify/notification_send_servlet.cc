#include "notification_send_servlet.h"
#include "../../manager/notification_manager.h"
#include "../../manager/user_manager.h"
#include "../../util.h"

#include <chen/log/log.h>
#include <chen/iomanager/iomanager.h>
#include <json/json.h>

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

        int64_t article_id = 0;
        request->checkGetParamAs("article_id", article_id);
        int64_t comment_id = 0;
        request->checkGetParamAs("comment_id", comment_id);

        // 构建通知 JSON
        Json::Value notif;
        notif["type"] = "notification";
        notif["notification_type"] = type;
        notif["title"] = title;
        notif["content"] = content;
        notif["article_id"] = article_id;
        notif["comment_id"] = comment_id;
        notif["create_time"] = (int64_t)time(0);
        std::string msg = chen::JsonUtil::ToString(notif);

        auto& notifMgr = *NotificationMgr::GetInstance();

        if (target == "all") {
            // 异步广播：收集用户列表后立即返回，后台完成 DB 写入和 WS 推送
            std::vector<int64_t> user_ids;
            UserMgr::GetInstance()->getAllIds(user_ids, true);

            chen::IOManager::GetThis()->schedule(
                [user_ids, title, content, type, sender_id = uid, article_id, comment_id, msg]() {
                    for (auto target_uid : user_ids) {
                        NotificationMgr::GetInstance()->addNotification(
                            target_uid, title, content, type, sender_id, article_id, comment_id);
                    }
                    NotificationMgr::GetInstance()->broadcast(msg);
                });

            INFO(logger) << "[NOTIFY] broadcast scheduled: type=" << type
                << " title=" << title << " count=" << user_ids.size();
        } else if (target == "users") {
            std::string user_ids_str = request->getParam("user_ids");
            if (user_ids_str.empty()) {
                result->setResult(400, "user_ids required when target=users");
                break;
            }

            std::vector<int64_t> ids;
            for (auto& s : chen::split(user_ids_str, ',')) {
                auto trimmed = chen::StringUtil::Trim(s);
                if (!trimmed.empty()) {
                    ids.push_back(std::stoll(trimmed));
                }
            }

            int32_t sent = 0;
            for (auto id : ids) {
                notifMgr.addNotification(id, title, content, type, uid, article_id, comment_id);
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
