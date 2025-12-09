#ifndef __BLOG_SERVLETS_ORGANIZATION_ORGANIZATION_JOIN_CHECK_SERVLET_H__
#define __BLOG_SERVLETS_ORGANIZATION_ORGANIZATION_JOIN_CHECK_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class OrganizationJoinCheckServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<OrganizationJoinCheckServlet> ptr;
    OrganizationJoinCheckServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLETS_ORGANIZATION_ORGANIZATION_JOIN_CHECK_SERVLET_H__