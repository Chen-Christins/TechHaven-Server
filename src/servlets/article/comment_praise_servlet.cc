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
    : BlogLoginedServlet("CommentPraiseServlet") {
}

int32_t CommentPraiseServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setErrno(errcode::NOT_LOGIN);
            break;
        }

        DEFINE_AND_CHECK_TYPE(result, int64_t, comment_id, "comment_id");

        // check comment exists
        auto comment = CommentMgr::GetInstance()->get(comment_id);
        if (!comment || comment->getIsDeleted()) {
            result->setErrno(errcode::COMMENT_NOT_FOUND);
            break;
        }

        bool already_praising = CommentPraiseRelMgr::GetInstance()->isPraising(uid, comment_id);

        if (already_praising) {
            // unlike
            if (!CommentPraiseRelMgr::GetInstance()->unpraise(uid, comment_id)) {
                result->setErrno(errcode::COMMENT_UNPRAISE_FAILED);
                break;
            }
            result->set("is_praising", false);
        } else {
            // like
            auto info = CommentPraiseRelMgr::GetInstance()->praise(uid, comment_id);
            if (!info) {
                result->setErrno(errcode::COMMENT_PRAISE_FAILED);
                break;
            }
            result->set("is_praising", true);

            // notify the comment author (only if not self-liking)
            int64_t author_id = comment->getUserId();
            if (author_id != uid) {
                auto liker_info = UserMgr::GetInstance()->get(uid);
                std::string liker_name = liker_info ? liker_info->getName() : "someone";
                std::string notify_title = "评论点赞";
                std::string notify_content = liker_name + " 赞了你的评论";

                auto notif_info = NotificationMgr::GetInstance()->addNotification(
                    author_id, notify_title, notify_content, "comment_praise", uid,
                    comment->getArticleId(), comment_id);
                if (notif_info) {
                    Json::Value wsMsg;
                    wsMsg["id"] = notif_info->getId();
                    wsMsg["title"] = notify_title;
                    wsMsg["content"] = notify_content;
                    wsMsg["type"] = "comment_praise";
                    wsMsg["article_id"] = comment->getArticleId();
                    wsMsg["comment_id"] = comment_id;
                    wsMsg["is_read"] = false;
                    wsMsg["create_time"] = notif_info->getCreateTime();
                    NotificationMgr::GetInstance()->sendToUser(author_id,
                        chen::JsonUtil::ToString(wsMsg));
                }
            }
        }

        int64_t praise_count = CommentPraiseRelMgr::GetInstance()->countByComment(comment_id);
        result->set("praise_count", praise_count);
        result->setErrno(errcode::SUCCESS);
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}
