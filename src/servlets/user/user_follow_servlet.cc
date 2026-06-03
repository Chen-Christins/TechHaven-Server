#include "user_follow_servlet.h"
#include "../../manager/user_follow_rel_manager.h"
#include "../../manager/user_manager.h"
#include "../../manager/notification_manager.h"
#include "../../util.h"
#include <json/json.h>

namespace blog {
namespace servlet {

UserFollowServlet::UserFollowServlet()
    : BlogLoginedServlet("UserFollowServlet") {
}

int32_t UserFollowServlet::handle(chen::http::HttpRequest::ptr request,
    chen::http::HttpResponse::ptr response,
    chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(410, "not login");
            break;
        }

        DEFINE_AND_CHECK_TYPE(result, int64_t, following_id, "following_id");

        if (following_id == uid) {
            result->setResult(400, "cannot follow yourself");
            break;
        }

        // check the target user exists
        auto targetUser = UserMgr::GetInstance()->get(following_id);
        if (!targetUser || targetUser->getIsDeleted()) {
            result->setResult(404, "user not found");
            break;
        }

        bool already_following = UserFollowRelMgr::GetInstance()->isFollowing(uid, following_id);

        auto info = UserFollowRelMgr::GetInstance()->follow(uid, following_id);
        if (!info) {
            result->setResult(500, "follow failed");
            break;
        }

        // notify the followed user (only for new follows, not re-follows)
        if (!already_following) {
            auto follower_info = UserMgr::GetInstance()->get(uid);
            std::string follower_name = follower_info ? follower_info->getName() : "someone";
            std::string notify_title = "新关注";
            std::string notify_content = follower_name + " 关注了你";

            auto notif_info = NotificationMgr::GetInstance()->addNotification(
                following_id, notify_title, notify_content, "follow", uid);
            if (notif_info) {
                Json::Value wsMsg;
                wsMsg["id"] = notif_info->getId();
                wsMsg["title"] = notify_title;
                wsMsg["content"] = notify_content;
                wsMsg["type"] = "follow";
                wsMsg["is_read"] = false;
                wsMsg["create_time"] = notif_info->getCreateTime();
                NotificationMgr::GetInstance()->sendToUser(following_id,
                    chen::JsonUtil::ToString(wsMsg));
            }
        }

        result->setResult(200, "ok");
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}
