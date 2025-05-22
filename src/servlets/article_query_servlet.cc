#include "article_query_servlet.h"
#include "chen/log/log.h"
#include "../manager/article_manager.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

ArticleQueryServlet::ArticleQueryServlet()
    :BlogServlet("ArticleQueryServlet") {
}

int32_t ArticleQueryServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t user_id = request->getParamAs<int64_t>("user_id");
        int64_t page_from = request->getParamAs<int64_t>("page_from");
        int64_t page_size = request->getParamAs<int64_t>("page_size", 6);
        
        INFO(logger) << "user_id=" << user_id;

        std::vector<data::ArticleInfo::ptr> infos;
        auto total = ArticleMgr::GetInstance()->listByUserIdPages(infos, user_id, page_from, page_size, true, 0);
        result->jsondata["total"] = total;
        result->jsondata["page_from"] = page_from;
        result->jsondata["page_size"] = page_size;
        for (auto& i : infos) {
            result->jsondata["ids"].append(i->getId());
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
};

}
}