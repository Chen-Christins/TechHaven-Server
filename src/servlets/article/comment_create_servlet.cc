#include "comment_create_servlet.h"
#include "../../manager/comment_manager.h"
#include "../../manager/article_manager.h"
#include "../../manager/user_manager.h"
#include "../../manager/notification_manager.h"
#include "../../util.h"
#include <chen/iomanager/iomanager.h>
#include <chen/util/util.h>
#include <json/json.h>

namespace blog {
namespace servlet {

CommentCreateServlet::CommentCreateServlet()
    : BlogLoginedServlet("CommentCreateServlet") {
}

int32_t CommentCreateServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setErrno(errcode::NOT_LOGIN);
            break;
        }

        DEFINE_AND_CHECK_TYPE(result, int64_t, article_id, "article_id");
        DEFINE_AND_CHECK_STRING(result, content, "content");

        int64_t parent_id = 0;
        request->checkGetParamAs("parent_id", parent_id);

        // check article exists
        auto article = ArticleMgr::GetInstance()->get(article_id);
        if (!article || article->getIsDeleted()) {
            result->setErrno(errcode::ARTICLE_NOT_FOUND);
            break;
        }

        // if replying, check parent comment exists
        if (parent_id > 0) {
            auto parent = CommentMgr::GetInstance()->get(parent_id);
            if (!parent || parent->getIsDeleted()) {
                result->setErrno(errcode::COMMENT_PARENT_NOT_FOUND);
                break;
            }
        }

        std::string ip = session->getRemoteAddressString();
        std::string user_agent = request->getHeader("User-Agent");

        auto info = CommentMgr::GetInstance()->create(article_id, uid, content, parent_id, ip, user_agent);
        if (!info) {
            result->setErrno(errcode::COMMENT_CREATE_FAILED);
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
        result->setErrno(errcode::SUCCESS);

        // 异步通知扇出：评论者信息 + 通知目标用户
        int64_t commenter_id = uid;
        int64_t comment_id = info->getId();
        int64_t author_id = article->getUserId();
        std::string article_title = article->getTitle();
        int64_t reply_to_parent_id = parent_id;

        chen::IOManager::GetThis()->schedule(
            [commenter_id, comment_id, author_id, article_title, article_id, reply_to_parent_id]() {
                auto commenter_info = UserMgr::GetInstance()->get(commenter_id);
                std::string commenter_name = commenter_info ? commenter_info->getName() : "someone";

                auto send_notify = [&](int64_t targetUid, const std::string& title, const std::string& content) {
                    if (targetUid == commenter_id) {
                        return;
                    }
                    auto notif_info = NotificationMgr::GetInstance()->addNotification(
                        targetUid, title, content, "comment", commenter_id, article_id, comment_id);
                    if (notif_info) {
                        Json::Value wsMsg;
                        wsMsg["id"] = notif_info->getId();
                        wsMsg["title"] = title;
                        wsMsg["content"] = content;
                        wsMsg["type"] = "comment";
                        wsMsg["article_id"] = article_id;
                        wsMsg["comment_id"] = comment_id;
                        wsMsg["is_read"] = false;
                        wsMsg["create_time"] = notif_info->getCreateTime();
                        NotificationMgr::GetInstance()->sendToUser(targetUid,
                            chen::JsonUtil::ToString(wsMsg));
                    }
                };

                // notify article author
                send_notify(author_id, "文章评论",
                    commenter_name + " 评论了你的文章《" + article_title + "》");

                // notify parent comment author on reply
                if (reply_to_parent_id > 0) {
                    auto parent = CommentMgr::GetInstance()->get(reply_to_parent_id);
                    if (parent && parent->getUserId() != author_id) {
                        send_notify(parent->getUserId(), "评论回复",
                            commenter_name + " 回复了你的评论");
                    }
                }
            });
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}
