#ifndef __BLOG_SERVLETS_RD_RD_TASK_EDIT_SERVLET_H__
#define __BLOG_SERVLETS_RD_RD_TASK_EDIT_SERVLET_H__

#include "../../struct.h"
#include <json/json.h>

#include "blog/data/task_info.h"

namespace blog {
namespace servlet {

class RdTaskEditServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<RdTaskEditServlet> ptr;
    RdTaskEditServlet();
    int32_t handle(chen::http::HttpRequest::ptr request
                ,chen::http::HttpResponse::ptr response
                ,chen::http::HttpSession::ptr session
                ,Result::ptr result) override;
private:
    void notifyAssignee(int64_t assignee_id, data::TaskInfo::ptr task);
};

}
}

#endif // __BLOG_SERVLETS_RD_RD_TASK_EDIT_SERVLET_H__
