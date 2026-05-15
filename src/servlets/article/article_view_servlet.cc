#include "article_view_servlet.h"
#include "../../manager/article_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

ArticleViewServlet::ArticleViewServlet()
    :BlogLoginedServlet("ArticleViewServlet") {
}

int32_t ArticleViewServlet::handle(chen::http::HttpRequest::ptr request,
    chen::http::HttpResponse::ptr response,
    chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(410, "not login");
            break;
        }

        DEFINE_AND_CHECK_TYPE(result, int64_t, article_id, "article_id");

        auto article = ArticleMgr::GetInstance()->get(article_id);
        if (!article || article->getIsDeleted()) {
            result->setResult(404, "article not found");
            break;
        }

        std::string cookie_id = std::to_string(uid);
        ArticleMgr::GetInstance()->incViews(article_id, cookie_id, uid);

        result->set("views", article->getViews());
        result->setResult(200, "ok");
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}
