#ifndef __BLOG_SERVLETS_BUG_BUG_LIST_SERVLET_H__
#define __BLOG_SERVLETS_BUG_BUG_LIST_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class BugListServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<BugListServlet> ptr;
    BugListServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLETS_BUG_BUG_LIST_SERVLET_H__
