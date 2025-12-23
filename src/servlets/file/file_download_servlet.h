#ifndef __BLOG_SERVLETS_FILE_FILE_DOWNLOAD_SERVLET_H__
#define __BLOG_SERVLETS_FILE_FILE_DOWNLOAD_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class FileDownloadServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<FileDownloadServlet> ptr;
    FileDownloadServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;
    bool checkPermission(int32_t system_role);
};

}
}

#endif // __BLOG_SERVLETS_FILE_FILE_DOWNLOAD_SERVLET_H__