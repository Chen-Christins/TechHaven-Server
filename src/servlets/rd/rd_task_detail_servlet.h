#ifndef __BLOG_SERVLETS_RD_RD_TASK_DETAIL_SERVLET_H__
#define __BLOG_SERVLETS_RD_RD_TASK_DETAIL_SERVLET_H__

#include "../../struct.h"
#include <json/json.h>

namespace blog {
namespace servlet {

class RdTaskDetailServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<RdTaskDetailServlet> ptr;
    RdTaskDetailServlet();
    int32_t handle(chen::http::HttpRequest::ptr request
                ,chen::http::HttpResponse::ptr response
                ,chen::http::HttpSession::ptr session
                ,Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLETS_RD_RD_TASK_DETAIL_SERVLET_H__
