#ifndef __BLOG_SERVLETS_DATABASE_BACKUP_DELETE_SERVLET_H__
#define __BLOG_SERVLETS_DATABASE_BACKUP_DELETE_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class BackupDeleteServlet : public BlogLoginedServlet {
public:
    BackupDeleteServlet();
    int32_t handle(chen::http::HttpRequest::ptr request,
                   chen::http::HttpResponse::ptr response,
                   chen::http::HttpSession::ptr session,
                   Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLETS_DATABASE_BACKUP_DELETE_SERVLET_H__
