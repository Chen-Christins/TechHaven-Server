#include "article_publish_servlet.h"

#include <chen/log/log.h>
#include <chen/iomanager/iomanager.h>
#include <json/json.h>

#include "../../index.h"
#include "../../manager/article_manager.h"
#include "../../manager/user_manager.h"
#include "../../util.h"
#include "../../event/event_define.h"

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
        IndexMgr::GetInstance()->updateArticle(info);

        // 清除对应月份的日历缓存
        ArticleMgr::GetInstance()->clearCalendarCache(info->getUserId(), info->getPublishTime());

        // Notify admins and checkers about new article
        {
            auto author = UserMgr::GetInstance()->get(uid);
            EventArticleReviewData data = {};
            data.type = "request";
            data.article_id = id;
            data.article_title = info->getTitle();
            data.author_id = uid;
            data.author_name = author ? author->getName() : std::to_string(uid);
            
            chen::EventBusMgr::GetInstance()->emitAsync(EVENT_ID_ARTICLE_REVIEW, std::move(data));
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
