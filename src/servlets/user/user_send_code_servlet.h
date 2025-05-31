#ifndef __BLOG_SERVLETS_USER_SEND_CODE_SERVLET_H__
#define __BLOG_SERVLETS_USER_SEND_CODE_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class UserSendCodeServlet : public BlogServlet {
public:
    typedef std::shared_ptr<UserSendCodeServlet> ptr;
    UserSendCodeServlet();
    virtual int32_t handle(sylar::http::HttpRequest::ptr request
                    ,sylar::http::HttpResponse::ptr response
                    ,sylar::http::HttpSession::ptr session
                    ,Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLETS_USER_SEND_CODE_SERVLET_H__