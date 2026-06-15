#include "article_publish_servlet.h"
#include <chen/log/log.h>
#include <chen/iomanager/iomanager.h>
#include <json/json.h>
#include "../../manager/article_manager.h"
#include "../../manager/user_manager.h"
#include "../../manager/notification_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

ArticlePublishServlet::ArticlePublishServlet()
    : BlogLoginedServlet("ArticlePublishServlet") {
}

int32_t ArticlePublishServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        DEFINE_AND_CHECK_TYPE(result, int64_t, publish_time, "publish_time");

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setErrno(errcode::NOT_LOGIN);
            break;
        }
        data::ArticleInfo::ptr info = ArticleMgr::GetInstance()->get(id);
        if (!info) {
            result->setErrno(errcode::ARTICLE_INVALID_ID);
            break;
        }

        if (info->getIsDeleted()) {
            result->setErrno(errcode::ARTICLE_INVALID_ID);
            break;
        }
        if (info->getState() == ArticleManager::Status::PUBLISHED
                || info->getState() == ArticleManager::Status::CHECKING) {
            result->setErrno(errcode::ARTICLE_INVALID_STATE);
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
            result->setErrno(errcode::DB_CONNECTION_FAILED);
            break;
        }
        if (data::ArticleInfoDao::Update(info, db)) {
            result->setErrno(errcode::ARTICLE_UPDATE_FAILED);
            break;
        }
        ArticleMgr::GetInstance()->add(info);

        // 清除对应月份的日历缓存
        ArticleMgr::GetInstance()->clearCalendarCache(info->getUserId(), info->getPublishTime());

        // 异步通知管理员和审核员有新文章待审核
        {
            std::string article_title = info->getTitle();
            chen::IOManager::GetThis()->schedule([author_id=uid, article_id=id, article_title]() {
                auto author = UserMgr::GetInstance()->get(author_id);
                std::string author_name = author ? author->getName() : std::to_string(author_id);
                std::string title = "新的文章待审核";
                std::string content = "「" + author_name + "」提交了文章「" + article_title + "」等待审核";

                std::vector<int64_t> userIds;
                UserMgr::GetInstance()->getAllIds(userIds, true);
                for (auto targetId : userIds) {
                    auto u = UserMgr::GetInstance()->get(targetId);
                    if (u && (u->getRole() == UserManager::Role::ADMIN
                            || u->getRole() == UserManager::Role::CHECKER)) {
                        auto notif_info = NotificationMgr::GetInstance()->addNotification(
                            targetId, title, content, "article_review_request", author_id, article_id);
                        if (notif_info) {
                            Json::Value wsMsg;
                            wsMsg["id"] = notif_info->getId();
                            wsMsg["title"] = title;
                            wsMsg["content"] = content;
                            wsMsg["type"] = "article_review_request";
                            wsMsg["article_id"] = article_id;
                            wsMsg["is_read"] = false;
                            wsMsg["create_time"] = notif_info->getCreateTime();
                            NotificationMgr::GetInstance()->sendToUser(targetId, chen::JsonUtil::ToString(wsMsg));
                        }
                    }
                }
            });
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
