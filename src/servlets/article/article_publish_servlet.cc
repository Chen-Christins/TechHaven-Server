#include "article_publish_servlet.h"
#include <chen/log/log.h>
#include "../../manager/article_manager.h"
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
        if (info->getState() != (int32_t)State::UNPUBLISH) {
            result->setResult(401, "invalid state");
            break;
        }
        info->setState((int32_t)State::VERIFYING);
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
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
