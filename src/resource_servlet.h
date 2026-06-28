/**
 * @file resource_servlet.h
 * @brief 服务器资源处理类的servlet
 * @author Christins
 * @date 2025-05-05
 * @copyright Apache 2.0
 */
#ifndef __RESOURCE_SERVLET_H__
#define __RESOURCE_SERVLET_H__

#include <string>
#include <memory>

#include <chen/http/http.h>
#include <chen/http/http_session.h>
#include <chen/http/servlet.h>

namespace chen {
namespace http {

class ResourceServlet : public Servlet {
public:
    typedef std::shared_ptr<ResourceServlet> ptr;
    ResourceServlet(const std::string& path);
    virtual int32_t handle(HttpRequest::ptr request, HttpResponse::ptr response
                        ,HttpSession::ptr session) override;

private:
    std::string m_path;
    std::string m_content;
};

}
}

#endif // __RESOURCE_SERVLET_H__