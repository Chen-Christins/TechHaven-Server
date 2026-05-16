#include "comment_create_servlet.h"
#include "../../manager/comment_manager.h"
#include "../../manager/article_manager.h"
#include "../../manager/user_manager.h"
#include "../../manager/notification_manager.h"
#include "../../util.h"
#include <chen/util/util.h>
#include <json/json.h>

namespace blog {
namespace servlet {

CommentCreateServlet::CommentCreateServlet()
    :BlogLoginedServlet("CommentCreateServlet") {
}

int32_t CommentCreateServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
		, chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(410, "not login");
            break;
        }

        DEFINE_AND_CHECK_TYPE(result, int64_t, article_id, "article_id");
        DEFINE_AND_CHECK_STRING(result, content, "content");

        int64_t parent_id = 0;
        request->checkGetParamAs("parent_id", parent_id);

        // check article exists
        auto article = ArticleMgr::GetInstance()->get(article_id);
        if (!article || article->getIsDeleted()) {
            result->setResult(404, "article not found");
            break;
        }

        // if replying, check parent comment exists
        if (parent_id > 0) {
            auto parent = CommentMgr::GetInstance()->get(parent_id);
            if (!parent || parent->getIsDeleted()) {
                result->setResult(404, "parent comment not found");
                break;
            }
        }

        std::string ip = session->getRemoteAddressString();
        std::string user_agent = request->getHeader("User-Agent");

        auto info = CommentMgr::GetInstance()->create(article_id, uid, content, parent_id, ip, user_agent);
        if (!info) {
            result->setResult(500, "create comment failed");
            break;
        }

        // build response
        Json::Value item;
        item["id"] = info->getId();
        item["content"] = info->getContent();
        item["user_id"] = info->getUserId();
        item["time"] = chen::Time2Str(info->getCreateTime());

        auto user = UserMgr::GetInstance()->get(uid);
        if (user) {
            item["user"] = user->getName();
            item["avatar"] = user->getAvatar();
        } else {
            item["user"] = "";
            item["avatar"] = "";
        }

        item["likes"] = 0;
        item["is_liked"] = false;
        item["replies"] = Json::Value(Json::arrayValue);
        item["reply_count"] = 0;

        result->jsondata = item;
        result->setResult(200, "ok");

        auto commenterInfo = UserMgr::GetInstance()->get(uid);
        std::string commenterName = commenterInfo ? commenterInfo->getName() : "someone";

        auto sendNotify = [&](int64_t targetUid, const std::string& title, const std::string& content) {
            if (targetUid == uid) return;
            auto notifInfo = NotificationMgr::GetInstance()->addNotification(
                targetUid, title, content, "comment", uid);
            if (notifInfo) {
                Json::Value wsMsg;
                wsMsg["id"] = notifInfo->getId();
                wsMsg["title"] = title;
                wsMsg["content"] = content;
                wsMsg["type"] = "comment";
                wsMsg["is_read"] = false;
                wsMsg["create_time"] = notifInfo->getCreateTime();
                NotificationMgr::GetInstance()->sendToUser(targetUid,
                    chen::JsonUtil::ToString(wsMsg));
            }
        };

        // notify article author
        int64_t authorId = article->getUserId();
        sendNotify(authorId, "文章评论",
            commenterName + " 评论了你的文章《" + article->getTitle() + "》");

        // notify parent comment author on reply
        if (parent_id > 0) {
            auto parent = CommentMgr::GetInstance()->get(parent_id);
            if (parent && parent->getUserId() != authorId) {
                sendNotify(parent->getUserId(), "评论回复",
                    commenterName + " 回复了你的评论");
            }
        }
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}
