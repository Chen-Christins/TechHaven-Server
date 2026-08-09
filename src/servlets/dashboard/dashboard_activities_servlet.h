#pragma once

#include "../../struct.h"

namespace blog {
namespace servlet {

class DashboardActivitiesServlet : public BlogLoginedServlet {
public:
    DashboardActivitiesServlet();
    int32_t handle(chen::http::HttpRequest::ptr request,
                   chen::http::HttpResponse::ptr response,
                   chen::http::HttpSession::ptr session,
                   Result::ptr result) override;
};

}
}
