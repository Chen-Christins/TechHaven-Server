#ifndef __BLOG_SERVLET_USER_STATS_SERVLET_H__
#define __BLOG_SERVLET_USER_STATS_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class UserStatsServlet : public BlogLoginedServlet {
public:
    UserStatsServlet();
    int32_t handle(chen::http::HttpRequest::ptr request,
                   chen::http::HttpResponse::ptr response,
                   chen::http::HttpSession::ptr session,
                   Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLET_USER_STATS_SERVLET_H__
