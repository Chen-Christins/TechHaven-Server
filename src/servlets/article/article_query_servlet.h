#ifndef __BLOG_SERVLETS_ARTICLE_QUERY_SERVLET_H__
#define __BLOG_SERVLETS_ARTICLE_QUERY_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class ArticleQueryServlet : public BlogServlet {
public:
    typedef std::shared_ptr<ArticleQueryServlet> ptr;
    ArticleQueryServlet();
    virtual int32_t handle(sylar::http::HttpRequest::ptr request
                    ,sylar::http::HttpResponse::ptr response
                    ,sylar::http::HttpSession::ptr session
                    ,Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLETS_ARTICLE_QUERY_SERVLET_H__