#include "article_query_servlet.h"
#include <chen/log/log.h>
#include "../../manager/article_manager.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

ArticleQueryServlet::ArticleQueryServlet()
    :BlogServlet("ArticleQueryServlet") {
}

int32_t ArticleQueryServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t user_id = request->getParamAs<int64_t>("user_id");
        int64_t page_from = request->getParamAs<int64_t>("page_from");
        int64_t page_size = request->getParamAs<int64_t>("page_size", 6);
		int64_t state = request->getParamAs<int64_t>("state", 0);
        
        std::vector<data::ArticleInfo::ptr> infos;
        auto total = ArticleMgr::GetInstance()->listByUserIdPages(infos, user_id, page_from, page_size, true, state);
        result->jsondata["total"] = total;
        result->jsondata["page_from"] = page_from;
        result->jsondata["page_size"] = page_size;
        for (auto& i : infos) {
            result->jsondata["ids"].append(i->getId());
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}