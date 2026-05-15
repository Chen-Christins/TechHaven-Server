#ifndef __BLOG_SERVLETS_ARTICLE_ARTICLE_IS_PRAISING_SERVLET_H__
#define __BLOG_SERVLETS_ARTICLE_ARTICLE_IS_PRAISING_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class ArticleIsPraisingServlet : public BlogLoginedServlet {
public:
    ArticleIsPraisingServlet();
    int32_t handle(chen::http::HttpRequest::ptr request,
                   chen::http::HttpResponse::ptr response,
                   chen::http::HttpSession::ptr session,
                   Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLETS_ARTICLE_ARTICLE_IS_PRAISING_SERVLET_H__
