#pragma once

#include "../../struct.h"

namespace blog {
namespace servlet {

class AdminCommentDeleteServlet : public BlogLoginedServlet {
public:
    AdminCommentDeleteServlet();
    int32_t handle(chen::http::HttpRequest::ptr request,
                   chen::http::HttpResponse::ptr response,
                   chen::http::HttpSession::ptr session,
                   Result::ptr result) override;
};

}
}
