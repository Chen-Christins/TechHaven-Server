#include "article_praise_servlet.h"

#include "../../manager/article_praise_rel_manager.h"
#include "../../manager/article_manager.h"
#include "../../manager/user_manager.h"
#include "../../manager/notification_manager.h"
#include "../../util.h"

#include <chen/iomanager/iomanager.h>

#include <json/json.h>

namespace blog {
namespace servlet {

ArticlePraiseServlet::ArticlePraiseServlet()
    : BlogLoginedServlet("ArticlePraiseServlet") {
}

int32_t ArticlePraiseServlet::handle(chen::http::HttpRequest::ptr request,
    chen::http::HttpResponse::ptr response,
    chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setErrno(errcode::NOT_LOGIN);
            break;
        }

        DEFINE_AND_CHECK_TYPE(result, int64_t, article_id, "article_id");

        // check article exists
        auto article = ArticleMgr::GetInstance()->get(article_id);
        if (!article || article->getIsDeleted()) {
            result->setErrno(errcode::ARTICLE_NOT_FOUND);
            break;
        }

        bool already_praising = ArticlePraiseRelMgr::GetInstance()->isPraising(uid, article_id);

        if (already_praising) {
            // unlike
            if (!ArticlePraiseRelMgr::GetInstance()->unpraise(uid, article_id)) {
                result->setErrno(errcode::ARTICLE_UNPRAISE_FAILED);
                break;
            }
            ArticleMgr::GetInstance()->decPraiseCount(article_id);
            result->set("is_praising", false);
        } else {
            // like
            auto info = ArticlePraiseRelMgr::GetInstance()->praise(uid, article_id);
            if (!info) {
                result->setErrno(errcode::ARTICLE_PRAISE_FAILED);
                break;
            }
            ArticleMgr::GetInstance()->incPraiseCount(article_id);
            result->set("is_praising", true);

            // 异步通知文章作者（非自赞时）
            int64_t author_id = article->getUserId();
            if (author_id != uid) {
                std::string article_title = article->getTitle();
                chen::IOManager::GetThis()->schedule(
                    [author_id, liker_id=uid, article_id, article_title]() {
                        auto liker_info = UserMgr::GetInstance()->get(liker_id);
                        std::string liker_name = liker_info ? liker_info->getName() : "someone";
                        std::string notify_title = "文章点赞";
                        std::string notify_content = liker_name + " 赞了你的文章《" + article_title + "》";

                        auto notif_info = NotificationMgr::GetInstance()->addNotification(
                            author_id, notify_title, notify_content, "praise", liker_id, article_id);
                        if (notif_info) {
                            Json::Value wsMsg;
                            wsMsg["id"] = notif_info->getId();
                            wsMsg["title"] = notify_title;
                            wsMsg["content"] = notify_content;
                            wsMsg["type"] = "praise";
                            wsMsg["article_id"] = article_id;
                            wsMsg["is_read"] = false;
                            wsMsg["create_time"] = notif_info->getCreateTime();
                            NotificationMgr::GetInstance()->sendToUser(author_id,
                                chen::JsonUtil::ToString(wsMsg));
                        }
                    });
            }
        }

        result->set("praise_count", (int64_t)article->getPraise());
        result->setErrno(errcode::SUCCESS);
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}
