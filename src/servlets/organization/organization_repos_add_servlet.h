#ifndef __BLOG_SERVLETS_ORGANIZATION_REPOS_ADD_SERVLET_H__
#define __BLOG_SERVLETS_ORGANIZATION_REPOS_ADD_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class OrganizationReposAddServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<OrganizationReposAddServlet> ptr;
    OrganizationReposAddServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLETS_ORGANIZATION_REPOS_ADD_SERVLET_H__
