#ifndef __BLOG_SERVLETS_USER_INFO_SERVLET_H__
#define __BLOG_SERVLETS_USER_INFO_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class UserInfoServlet : public BlogServlet {
public:
    typedef std::shared_ptr<UserInfoServlet> ptr;
    UserInfoServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLETS_USER_INFO_SERVLET_H__