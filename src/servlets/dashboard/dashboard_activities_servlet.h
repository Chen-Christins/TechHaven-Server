#ifndef __BLOG_SERVLETS_DASHBOARD_DASHBOARD_ACTIVITIES_SERVLET_H__
#define __BLOG_SERVLETS_DASHBOARD_DASHBOARD_ACTIVITIES_SERVLET_H__

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

#endif // __BLOG_SERVLETS_DASHBOARD_DASHBOARD_ACTIVITIES_SERVLET_H__
