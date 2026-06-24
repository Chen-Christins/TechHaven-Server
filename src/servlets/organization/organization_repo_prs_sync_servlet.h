/**
 * @file organization_repo_prs_sync_servlet.h
 * @brief 组织仓库 PR 全量同步接口
 * @author Christins
 * @date 2026-06-24
 * @copyright Apache 2.0
 */
#ifndef __BLOG_SERVLETS_ORGANIZATION_REPO_PRS_SYNC_SERVLET_H__
#define __BLOG_SERVLETS_ORGANIZATION_REPO_PRS_SYNC_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class OrganizationRepoPrsSyncServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<OrganizationRepoPrsSyncServlet> ptr;
    OrganizationRepoPrsSyncServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLETS_ORGANIZATION_REPO_PRS_SYNC_SERVLET_H__
