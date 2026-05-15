#include "user_follow_servlet.h"
#include "../../manager/user_follow_rel_manager.h"
#include "../../manager/user_manager.h"
#include "../../manager/notification_manager.h"
#include "../../util.h"
#include <json/json.h>

namespace blog {
namespace servlet {

UserFollowServlet::UserFollowServlet()
    :BlogLoginedServlet("UserFollowServlet") {
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

        bool alreadyFollowing = UserFollowRelMgr::GetInstance()->isFollowing(uid, following_id);

        auto info = UserFollowRelMgr::GetInstance()->follow(uid, following_id);
        if (!info) {
            result->setResult(500, "follow failed");
            break;
        }

        // notify the followed user (only for new follows, not re-follows)
        if (!alreadyFollowing) {
            auto followerInfo = UserMgr::GetInstance()->get(uid);
            std::string followerName = followerInfo ? followerInfo->getName() : "someone";
            std::string notifyTitle = "新关注";
            std::string notifyContent = followerName + " 关注了你";

            auto notifInfo = NotificationMgr::GetInstance()->addNotification(
                following_id, notifyTitle, notifyContent, "follow", uid);
            if (notifInfo) {
                Json::Value wsMsg;
                wsMsg["id"] = notifInfo->getId();
                wsMsg["title"] = notifyTitle;
                wsMsg["content"] = notifyContent;
                wsMsg["type"] = "follow";
                wsMsg["is_read"] = false;
                wsMsg["create_time"] = notifInfo->getCreateTime();
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
