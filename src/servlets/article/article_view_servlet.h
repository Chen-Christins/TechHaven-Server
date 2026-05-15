#ifndef __BLOG_SERVLETS_ARTICLE_ARTICLE_VIEW_SERVLET_H__
#define __BLOG_SERVLETS_ARTICLE_ARTICLE_VIEW_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class ArticleViewServlet : public BlogLoginedServlet {
public:
    ArticleViewServlet();
    int32_t handle(chen::http::HttpRequest::ptr request,
                   chen::http::HttpResponse::ptr response,
                   chen::http::HttpSession::ptr session,
                   Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLETS_ARTICLE_ARTICLE_VIEW_SERVLET_H__
