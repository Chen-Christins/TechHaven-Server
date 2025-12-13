#ifndef __BLOG_SERVLETS_RESOURCE_RESOURCE_LIST_SERVLET_H__
#define __BLOG_SERVLETS_RESOURCE_RESOURCE_LIST_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class ResourceListServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<ResourceListServlet> ptr;
    ResourceListServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;
    bool checkPermession(int32_t system_role);
};

}
}

#endif // __BLOG_SERVLETS_RESOURCE_RESOURCE_LIST_SERVLET_H__