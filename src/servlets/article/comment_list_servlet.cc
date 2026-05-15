#include "comment_list_servlet.h"
#include "../../manager/comment_manager.h"
#include "../../manager/comment_praise_rel_manager.h"
#include "../../manager/article_manager.h"
#include "../../manager/user_manager.h"
#include "../../util.h"
#include <chen/util/util.h>
#include <json/json.h>

namespace blog {
namespace servlet {

CommentListServlet::CommentListServlet()
    :BlogServlet("CommentListServlet") {
}

int32_t CommentListServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
		, chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, article_id, "article_id");

        int64_t offset = request->getParamAs<int64_t>("offset", 0);
        int64_t size = request->getParamAs<int64_t>("size", 20);

        // check article exists
        auto article = ArticleMgr::GetInstance()->get(article_id);
        if (!article || article->getIsDeleted()) {
            result->setResult(404, "article not found");
            break;
        }

        int64_t uid = getUserId(request);

        std::vector<data::CommentInfo::ptr> comments;
        CommentMgr::GetInstance()->listByArticle(comments, article_id, offset, size);
        int64_t total = CommentMgr::GetInstance()->countByArticle(article_id);

        Json::Value list(Json::arrayValue);
        for (auto& c : comments) {
            Json::Value item;
            item["id"] = c->getId();
            item["content"] = c->getContent();
            item["user_id"] = c->getUserId();
            item["time"] = chen::Time2Str(c->getCreateTime());

            // user info
            auto user = UserMgr::GetInstance()->get(c->getUserId());
            if (user) {
                item["user"] = user->getName();
                item["avatar"] = user->getAvatar();
            } else {
                item["user"] = "";
                item["avatar"] = "";
            }

            // praise info
            int64_t likes = CommentPraiseRelMgr::GetInstance()->countByComment(c->getId());
            item["likes"] = likes;
            if (uid > 0) {
                item["is_liked"] = CommentPraiseRelMgr::GetInstance()->isPraising(uid, c->getId());
            } else {
                item["is_liked"] = false;
            }

            // nested replies (first 3)
            std::vector<data::CommentInfo::ptr> nestedReplies;
            CommentMgr::GetInstance()->listReplies(nestedReplies, c->getId(), 0, 3);
            Json::Value repliesArr(Json::arrayValue);
            for (auto& r : nestedReplies) {
                Json::Value reply;
                reply["id"] = r->getId();
                reply["content"] = r->getContent();
                reply["user_id"] = r->getUserId();
                reply["time"] = chen::Time2Str(r->getCreateTime());

                auto ruser = UserMgr::GetInstance()->get(r->getUserId());
                if (ruser) {
                    reply["user"] = ruser->getName();
                    reply["avatar"] = ruser->getAvatar();
                } else {
                    reply["user"] = "";
                    reply["avatar"] = "";
                }

                int64_t rlikes = CommentPraiseRelMgr::GetInstance()->countByComment(r->getId());
                reply["likes"] = rlikes;
                if (uid > 0) {
                    reply["is_liked"] = CommentPraiseRelMgr::GetInstance()->isPraising(uid, r->getId());
                } else {
                    reply["is_liked"] = false;
                }
                reply["reply_count"] = CommentMgr::GetInstance()->countReplies(r->getId());
                repliesArr.append(reply);
            }
            item["replies"] = repliesArr;
            item["reply_count"] = CommentMgr::GetInstance()->countReplies(c->getId());

            list.append(item);
        }

        result->set("total", total);
        result->set("list", list);
        result->setResult(200, "ok");
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}
