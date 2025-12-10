#ifndef __BLOG_SERVLETS_ORGANIZATION_ORGANIZATION_USER_KICK_SERVLET_H__
#define __BLOG_SERVLETS_ORGANIZATION_ORGANIZATION_USER_KICK_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class OrganizationUserKickServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<OrganizationUserKickServlet> ptr;
    OrganizationUserKickServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;
    bool checkPermission(int32_t system_role, int32_t org_role);
};

}
}

#endif // __BLOG_SERVLETS_ORGANIZATION_ORGANIZATION_USER_KICK_SERVLET_H__