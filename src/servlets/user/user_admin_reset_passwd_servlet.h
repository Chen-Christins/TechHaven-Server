#pragma once

#include "../../struct.h"

namespace blog {
namespace servlet {

class UserAdminResetPasswdServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<UserAdminResetPasswdServlet> ptr;
    UserAdminResetPasswdServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;
};

}
}
