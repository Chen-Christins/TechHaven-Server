#include "article_update_servlet.h"

#include <chen/log/log.h>

#include "../../index.h"
#include "../../manager/article_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

ArticleUpdateServlet::ArticleUpdateServlet()
    : BlogLoginedServlet("ArticleUpdateServlet") {
}

int32_t ArticleUpdateServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        DEFINE_AND_CHECK_STRING(result, title, "title");
        DEFINE_AND_CHECK_STRING(result, content, "content");

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setErrno(errcode::NOT_LOGIN);
            break;
        }

        data::ArticleInfo::ptr info = ArticleMgr::GetInstance()->get(id);
        if (uid != info->getUserId()) {
            result->setErrno(errcode::ACCESS_DENIED);
            break;
        }
        int32_t state = info->getState();
        info->setTitle(title);
        info->setContent(content);

        if (state == ArticleManager::Status::PUBLISHED) {
            info->setState(ArticleManager::Status::CHECKING);
        } else if (state == ArticleManager::Status::REJECTED) {
            info->setState(ArticleManager::Status::PRIVATE);
        }
        info->setUpdateTime(time(0));

        auto db = getDB();
        if (!db) {
            result->setErrno(errcode::DB_CONNECTION_FAILED);
            break;
        }
        if (data::ArticleInfoDao::Update(info, db)) {
            result->setErrno(errcode::ARTICLE_UPDATE_FAILED);
            ERROR(logger) << "db error errno=" << db->getErrno()
                << " errstr=" << db->getErrStr();
            break;
        }

        IndexMgr::GetInstance()->updateArticle(info);

        // 如果从已发布变为审核中，清除对应月份的日历缓存
        if (state == ArticleManager::Status::PUBLISHED && info->getPublishTime() > 0) {
            ArticleMgr::GetInstance()->clearCalendarCache(info->getUserId(), info->getPublishTime());
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
