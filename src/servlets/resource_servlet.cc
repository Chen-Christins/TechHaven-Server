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
    response->setHeader("content-type", "text/html;charset=utf-8");
    return 0;
}

}
}