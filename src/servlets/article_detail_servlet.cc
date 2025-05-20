#include "article_detail_servlet.h"
#include "chen/log/log.h"
#include "../util.h"
#include "../manager/article_manager.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

ArticleDetailServlet::ArticleDetailServlet()
    :BlogServlet("ArticleDetailServlet") {
}

int32_t ArticleDetailServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        
        data::ArticleInfo::ptr info = ArticleMgr::GetInstance()->get(id);
        if (!info) {
            result->setResult(404, "invalid id");
            break;
        }
        result->set("id", info->getId());
        result->set("title", info->getTitle());
        result->set("content", info->getContent());
        result->set("user_id", info->getUserId());
        result->set("type", info->getType());
        result->set("publish_time", info->getPublishTime());
        
        
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
};

}
}
