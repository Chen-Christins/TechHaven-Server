#include "resource_servlet.h"
#include <chen/config/config.h>
#include <chen/log/log.h>
#include <chen/env.h>

namespace chen {
namespace http {

static chen::Logger::ptr logger = LOG_ROOT();

ResourceServlet::ResourceServlet(const std::string& path)
        : Servlet("ResourceServlet")
        , m_path(path) {
    m_content = "<html><head><title>404 Not Found"
        "</title></head><body><center><h1>404 Not Found</h1></center>"
        "<hr><center>" + chen::EnvMgr::GetInstance()->getEnv("server") + "</center></body></html>";
}

int32_t ResourceServlet::handle(HttpRequest::ptr request, HttpResponse::ptr response
                    ,HttpSession::ptr session) {
    std::string rpath = request->getPath();

    auto path = m_path + rpath;
    INFO(logger) << path;
    if (path.find("..") != std::string::npos) {
        response->setBody(m_content);
        response->setStatus(HttpStatus::NOT_FOUND);
        return 0;
    }
    std::ifstream ifs(path);

    std::string line;
    std::stringstream ss;
    while (std::getline(ifs, line)) {
        ss << line << std::endl;
    }
    response->setBody(ss.str());
    auto it = path.find_last_of(".");
    if (it != std::string::npos) {
        auto ext = path.substr(it + 1);
        if (ext == "html") {
            response->setHeader("Content-Type", "text/html");
        } else if (ext == "json") {
            response->setHeader("Content-Type", "application/json");
        } else if (ext == "xml") {
            response->setHeader("Content-Type", "application/xml");
        } else if (ext == "css") {
            response->setHeader("Content-Type", "text/css");
        } else if (ext == "js") {
            response->setHeader("Content-Type", "application/javascript");
        } else if (ext == "png") {
            response->setHeader("Content-Type", "image/png");
        } else if (ext == "jpg" || ext == "jpeg") {
            response->setHeader("Content-Type", "image/jpeg");
        } else if (ext == "gif") {
            response->setHeader("Content-Type", "image/gif");
        } else if (ext == "ico") {
            response->setHeader("Content-Type", "image/x-icon");
        } else if (ext == "txt") {
            response->setHeader("Content-Type", "text/plain");
        } else {
            response->setHeader("Content-Type", "application/octet-stream");
        }
    } else {
        response->setHeader("Content-Type", "text/plain");
    }
    return 0;
}

}
}