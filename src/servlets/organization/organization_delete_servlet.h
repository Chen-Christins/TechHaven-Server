#ifndef __BLOG_SERVLETS_ORGANIZATION_ORGANIZATION_DELETE_SERVLET_H__
#define __BLOG_SERVLETS_ORGANIZATION_ORGANIZATION_DELETE_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class OrganizationDeleteServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<OrganizationDeleteServlet> ptr;
    OrganizationDeleteServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLETS_ORGANIZATION_ORGANIZATION_DELETE_SERVLET_H__