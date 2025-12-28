#ifndef __BLOG_SERVLETS_CHUNK_UPLOAD_SERVLET_H__
#define __BLOG_SERVLETS_CHUNK_UPLOAD_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class ChunkUploadServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<ChunkUploadServlet> ptr;
    ChunkUploadServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;

    int32_t handleInit(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result);
    
    int32_t handleComplete(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result);
    
    int32_t handleCancel(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result);
    
    int32_t handleUpload(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result);
};

}
}

#endif // __BLOG_SERVLETS_CHUNK_UPLOAD_SERVLET_H__
