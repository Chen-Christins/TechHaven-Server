#include "article_praise_servlet.h"
#include "../../manager/article_praise_rel_manager.h"
#include "../../manager/article_manager.h"
#include "../../manager/user_manager.h"
#include "../../manager/notification_manager.h"
#include "../../util.h"
#include <json/json.h>

namespace blog {
namespace servlet {

ArticlePraiseServlet::ArticlePraiseServlet()
    :BlogLoginedServlet("ArticlePraiseServlet") {
}

int32_t ArticlePraiseServlet::handle(chen::http::HttpRequest::ptr request,
    chen::http::HttpResponse::ptr response,
    chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(410, "not login");
            break;
        }

        DEFINE_AND_CHECK_TYPE(result, int64_t, article_id, "article_id");

        // check article exists
        auto article = ArticleMgr::GetInstance()->get(article_id);
        if (!article || article->getIsDeleted()) {
            result->setResult(404, "article not found");
            break;
        }

        bool alreadyPraising = ArticlePraiseRelMgr::GetInstance()->isPraising(uid, article_id);

        if (alreadyPraising) {
            // unlike
            if (!ArticlePraiseRelMgr::GetInstance()->unpraise(uid, article_id)) {
                result->setResult(500, "unpraise failed");
                break;
            }
            ArticleMgr::GetInstance()->decPraiseCount(article_id);
            result->set("is_praising", false);
        } else {
            // like
            auto info = ArticlePraiseRelMgr::GetInstance()->praise(uid, article_id);
            if (!info) {
                result->setResult(500, "praise failed");
                break;
            }
            ArticleMgr::GetInstance()->incPraiseCount(article_id);
            result->set("is_praising", true);

            // notify the article author (only if not self-liking)
            int64_t authorId = article->getUserId();
            if (authorId != uid) {
                auto likerInfo = UserMgr::GetInstance()->get(uid);
                std::string likerName = likerInfo ? likerInfo->getName() : "someone";
                std::string notifyTitle = "文章点赞";
                std::string notifyContent = likerName + " 赞了你的文章《" + article->getTitle() + "》";

                auto notifInfo = NotificationMgr::GetInstance()->addNotification(
                    authorId, notifyTitle, notifyContent, "praise", uid);
                if (notifInfo) {
                    Json::Value wsMsg;
                    wsMsg["id"] = notifInfo->getId();
                    wsMsg["title"] = notifyTitle;
                    wsMsg["content"] = notifyContent;
                    wsMsg["type"] = "praise";
                    wsMsg["is_read"] = false;
                    wsMsg["create_time"] = notifInfo->getCreateTime();
                    NotificationMgr::GetInstance()->sendToUser(authorId,
                        chen::JsonUtil::ToString(wsMsg));
                }
            }
        }

        result->set("praise_count", (int64_t)article->getPraise());
        result->setResult(200, "ok");
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}
