#ifndef __BLOG_SERVLETS_USER_RESET_PASSWD_SERVLET_H__
#define __BLOG_SERVLETS_USER_RESET_PASSWD_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class UserResetPasswdServlet : public BlogServlet {
public:
    typedef std::shared_ptr<UserResetPasswdServlet> ptr;
    UserResetPasswdServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;
    bool verificationEmailCode(chen::IDB::ptr db, const std::string& email
                    ,const std::string& code);
};

}
}

#endif // __BLOG_SERVLETS_USER_RESET_PASSWD_SERVLET_H__