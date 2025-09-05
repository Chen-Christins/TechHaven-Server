#ifndef __BLOG_SERVLETS_CATEGORY_CREATE_SERVLET_H__
#define __BLOG_SERVLETS_CATEGORY_CREATE_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class CategoryCreateServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<CategoryCreateServlet> ptr;
    CategoryCreateServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLETS_CATEGORY_CREATE_SERVLET_H__