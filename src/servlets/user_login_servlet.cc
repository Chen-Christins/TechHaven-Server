#include "user_login_servlet.h"
#include "../util.h"

namespace blog {
namespace servlet {

UserLoginServlet::UserLoginServlet() 
    :BlogServlet("UserLoginServlet") {
}

int32_t UserLoginServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, auth_id, "auth_id");
        DEFINE_AND_CHECK_STRING(result, passwd, "passwd");
        
    } while (false);
    
}

}
}