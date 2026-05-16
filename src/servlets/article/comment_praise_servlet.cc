#include "comment_praise_servlet.h"
#include "../../manager/comment_praise_rel_manager.h"
#include "../../manager/comment_manager.h"
#include "../../manager/user_manager.h"
#include "../../manager/notification_manager.h"
#include "../../util.h"
#include <json/json.h>

namespace blog {
namespace servlet {

CommentPraiseServlet::CommentPraiseServlet()
    :BlogLoginedServlet("CommentPraiseServlet") {
}

int32_t CommentPraiseServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
		, chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(410, "not login");
            break;
        }

        DEFINE_AND_CHECK_TYPE(result, int64_t, comment_id, "comment_id");

        // check comment exists
        auto comment = CommentMgr::GetInstance()->get(comment_id);
        if (!comment || comment->getIsDeleted()) {
            result->setResult(404, "comment not found");
            break;
        }

        bool alreadyPraising = CommentPraiseRelMgr::GetInstance()->isPraising(uid, comment_id);

        if (alreadyPraising) {
            // unlike
            if (!CommentPraiseRelMgr::GetInstance()->unpraise(uid, comment_id)) {
                result->setResult(500, "unpraise failed");
                break;
            }
            result->set("is_praising", false);
        } else {
            // like
            auto info = CommentPraiseRelMgr::GetInstance()->praise(uid, comment_id);
            if (!info) {
                result->setResult(500, "praise failed");
                break;
            }
            result->set("is_praising", true);

            // notify the comment author (only if not self-liking)
            int64_t authorId = comment->getUserId();
            if (authorId != uid) {
                auto likerInfo = UserMgr::GetInstance()->get(uid);
                std::string likerName = likerInfo ? likerInfo->getName() : "someone";
                std::string notifyTitle = "评论点赞";
                std::string notifyContent = likerName + " 赞了你的评论";

                auto notifInfo = NotificationMgr::GetInstance()->addNotification(
                    authorId, notifyTitle, notifyContent, "comment_praise", uid,
                    comment->getArticleId(), comment_id);
                if (notifInfo) {
                    Json::Value wsMsg;
                    wsMsg["id"] = notifInfo->getId();
                    wsMsg["title"] = notifyTitle;
                    wsMsg["content"] = notifyContent;
                    wsMsg["type"] = "comment_praise";
                    wsMsg["article_id"] = comment->getArticleId();
                    wsMsg["comment_id"] = comment_id;
                    wsMsg["is_read"] = false;
                    wsMsg["create_time"] = notifInfo->getCreateTime();
                    NotificationMgr::GetInstance()->sendToUser(authorId,
                        chen::JsonUtil::ToString(wsMsg));
                }
            }
        }

        int64_t praiseCount = CommentPraiseRelMgr::GetInstance()->countByComment(comment_id);
        result->set("praise_count", praiseCount);
        result->setResult(200, "ok");
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}
