#include "chunk_upload_servlet.h"
#include <chen/log/log.h>
#include <chen/config/config.h>

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();
static chen::ConfigVar<std::string>::ptr server_work_path = 
    chen::Config::Lookup<std::string>("server.work_path");

ChunkUploadServlet::ChunkUploadServlet()
    : BlogLoginedServlet("ChunkUploadServlet") {
}

int32_t ChunkUploadServlet::handle(chen::http::HttpRequest::ptr request
        , chen::http::HttpResponse::ptr response, chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

int32_t ChunkUploadServlet::handleInit(chen::http::HttpRequest::ptr request
        , chen::http::HttpResponse::ptr response, chen::http::HttpSession::ptr session, Result::ptr result) {
    return 0;
}
    
int32_t ChunkUploadServlet::handleComplete(chen::http::HttpRequest::ptr request
        , chen::http::HttpResponse::ptr response, chen::http::HttpSession::ptr session, Result::ptr result) {
    return 0;
}

int32_t ChunkUploadServlet::handleCancel(chen::http::HttpRequest::ptr request
        , chen::http::HttpResponse::ptr response, chen::http::HttpSession::ptr session, Result::ptr result) {
    return 0;
}

int32_t ChunkUploadServlet::handleUpload(chen::http::HttpRequest::ptr request
        , chen::http::HttpResponse::ptr response, chen::http::HttpSession::ptr session, Result::ptr result) {
    return 0;
}

} // namespace servlet
} // namespace blog
