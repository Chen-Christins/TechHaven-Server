#include "article_publish_servlet.h"
#include "chen/log/log.h"
#include "../manager/article_manager.h"
#include "../util.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

ArticlePublishServlet::ArticlePublishServlet()
    :BlogLoginedServlet("ArticlePublishServlet") {
}

int32_t ArticlePublishServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
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
        if (info->getState() != 0) {
            result->setResult(401, "invalid state");
            break;
        }
        info->setState(1);
        info->setPublishTime(publish_time);
        info->setUpdateTime(time(0));
        
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
};

}
}
