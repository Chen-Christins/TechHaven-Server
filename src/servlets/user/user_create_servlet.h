#ifndef __BLOG_SERVLETS_USER_CREATE_SERVLET_H__
#define __BLOG_SERVLETS_USER_CREATE_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class UserCreateServlet : public BlogServlet {
public:
    typedef std::shared_ptr<UserCreateServlet> ptr;
    UserCreateServlet();
    virtual int32_t handle(sylar::http::HttpRequest::ptr request
                    ,sylar::http::HttpResponse::ptr response
                    ,sylar::http::HttpSession::ptr session
                    ,Result::ptr result) override;

    bool verificationEmailCode(sylar::IDB::ptr db, const std::string& email
                    ,const std::string& code);
};

}
}

#endif // __BLOG_SERVLETS_USER_CREATE_SERVLET_H__