#include "article_publish_servlet.h"
#include <chen/log/log.h>
#include <json/json.h>
#include "../../manager/article_manager.h"
#include "../../manager/user_manager.h"
#include "../../manager/notification_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

ArticlePublishServlet::ArticlePublishServlet()
    :BlogLoginedServlet("ArticlePublishServlet") {
}

int32_t ArticlePublishServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        DEFINE_AND_CHECK_TYPE(result, int64_t, publish_time, "publish_time");

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }
        data::ArticleInfo::ptr info = ArticleMgr::GetInstance()->get(id);
        if (!info) {
            result->setResult(401, "invalid id");
            break;
        }

        if (info->getIsDeleted()) {
            result->setResult(401, "invalid article");
            break;
        }
        if (info->getState() == ArticleManager::Status::PUBLISHED
                || info->getState() == ArticleManager::Status::CHECKING) {
            result->setResult(401, "invalid state");
            break;
        }
        info->setState(ArticleManager::Status::CHECKING);
        time_t now = time(0);
        if (publish_time > now) {
            info->setPublishTime(publish_time);
        } else {
            info->setPublishTime(now);
        }
        info->setUpdateTime(now);
        
        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db connection fail");
            break;
        }
        if (data::ArticleInfoDao::Update(info, db)) {
            result->setResult(500, "update article fail");
            break;
        }
        ArticleMgr::GetInstance()->add(info);

        // 通知管理员和审核员有新文章待审核
        {
            auto author = UserMgr::GetInstance()->get(uid);
            std::string author_name = author ? author->getName() : std::to_string(uid);
            std::string title = "新的文章待审核";
            std::string content = "「" + author_name + "」提交了文章「" + info->getTitle() + "」等待审核";

            std::vector<int64_t> userIds;
            UserMgr::GetInstance()->getAllIds(userIds, true);
            for (auto targetId : userIds) {
                auto u = UserMgr::GetInstance()->get(targetId);
                if (u && (u->getRole() == UserManager::Role::ADMIN
                        || u->getRole() == UserManager::Role::CHECKER)) {
                    auto notifInfo = NotificationMgr::GetInstance()->addNotification(
                        targetId, title, content, "article_review_request", uid, id);
                    if (notifInfo) {
                        Json::Value wsMsg;
                        wsMsg["id"] = notifInfo->getId();
                        wsMsg["title"] = title;
                        wsMsg["content"] = content;
                        wsMsg["type"] = "article_review_request";
                        wsMsg["article_id"] = id;
                        wsMsg["is_read"] = false;
                        wsMsg["create_time"] = notifInfo->getCreateTime();
                        NotificationMgr::GetInstance()->sendToUser(
                            targetId, chen::JsonUtil::ToString(wsMsg));
                    }
                }
            }
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
