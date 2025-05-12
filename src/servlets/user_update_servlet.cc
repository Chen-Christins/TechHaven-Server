#include "user_update_servlet.h"
#include "chen/log/log.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

UserUpdateServlet::UserUpdateServlet()
    :BlogServlet("UserUpdateServlet") {
}

int32_t UserUpdateServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
    return 0;
};

}
}
