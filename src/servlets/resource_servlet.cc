#include "resource_servlet.h"
#include "chen/log/log.h"

namespace sylar {
namespace http {

static sylar::Logger::ptr logger = LOG_ROOT();

ResourceServlet::ResourceServlet(const std::string& path)
    :Servlet("ResourceServlet")
    ,m_path(path) {
}

int32_t ResourceServlet::handle(HttpRequest::ptr request, HttpResponse::ptr response
                    ,HttpSession::ptr session) {
    auto path = m_path + "/" + request->getPath();
    INFO(logger) << path;
    if (path.find("..") != std::string::npos) {
        response->setBody("invalid path");
        response->setStatus(HttpStatus::NOT_FOUND);
        return 0;
    }
    std::ifstream ifs(path);
    if (!ifs) {
        response->setBody("invalid file");
        response->setStatus(HttpStatus::NOT_FOUND);
        return 0;
    }

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