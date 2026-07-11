#ifndef __BLOG_SERVLETS_DATABASE_BACKUP_DOWNLOAD_SERVLET_H__
#define __BLOG_SERVLETS_DATABASE_BACKUP_DOWNLOAD_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class BackupDownloadServlet : public BlogLoginedServlet {
public:
    BackupDownloadServlet();
    int32_t handle(chen::http::HttpRequest::ptr request,
                   chen::http::HttpResponse::ptr response,
                   chen::http::HttpSession::ptr session,
                   Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLETS_DATABASE_BACKUP_DOWNLOAD_SERVLET_H__
