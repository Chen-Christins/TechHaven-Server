#include "article_switch_state_servlet.h"
#include "../../manager/user_manager.h"
#include "../../manager/article_manager.h"
#include "../../manager/notification_manager.h"
#include "../../util.h"
#include <chen/log/log.h>

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

ArticleSwitchStateServlet::ArticleSwitchStateServlet()
    : BlogLoginedServlet("ArticleSwitchStateServlet") {
}

int32_t ArticleSwitchStateServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, uint64_t, id, "id");
        DEFINE_AND_CHECK_TYPE(result, int, new_state, "new_state");

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }

        auto article = ArticleMgr::GetInstance()->get(id);
        if (!article) {
            result->setResult(404, "article not found");
            break;
        }

        int32_t role = UserMgr::GetInstance()->get(uid)->getRole();
        if (role != UserManager::Role::ADMIN) {
            if (article->getUserId() != uid) {
                result->setResult(403, "Access Denied");
                break;
            } else if (new_state != ArticleManager::Status::PRIVATE) {
                result->setResult(400, "invalid new_state");
                break;
            }
        }

        article->setState(new_state);
        article->setUpdateTime(time(0));
        article->setPublishTime(0);

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db connection fail");
            break;
        }
        if (data::ArticleInfoDao::Update(article, db)) {
            result->setResult(500, "update article fail");
            break;
        }
        ArticleMgr::GetInstance()->add(article);

        // Notify article author if admin changed state
        if (role == UserManager::Role::ADMIN && article->getUserId() != uid) {
            chen::IOManager::GetThis()->schedule([article]() {
                std::string title = "文章状态变更";
                std::string content = "你的文章《" + article->getTitle() + "》状态已被管理员变更为「" +
                    (article->getState() == ArticleManager::PUBLISHED ? "已发布" : "私密") + "」";
                auto notif = NotificationMgr::GetInstance()->addNotification(
                    article->getUserId(), title, content, "article_state_changed", 0, article->getId());
                if (notif) {
                    Json::Value wsMsg;
                    wsMsg["id"] = notif->getId();
                    wsMsg["title"] = title;
                    wsMsg["content"] = content;
                    wsMsg["type"] = "article_state_changed";
                    wsMsg["article_id"] = article->getId();
                    wsMsg["is_read"] = false;
                    wsMsg["create_time"] = notif->getCreateTime();
                    NotificationMgr::GetInstance()->sendToUser(article->getUserId(), chen::JsonUtil::ToString(wsMsg));
                }
            });
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
