#ifndef __BLOG_SERVLETS_SETTINGS_SYSTEM_SETTINGS_SERVLET_H__
#define __BLOG_SERVLETS_SETTINGS_SYSTEM_SETTINGS_SERVLET_H__

#include "../../struct.h"

namespace blog::servlet {

class SystemSettingsServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<SystemSettingsServlet> ptr;
    SystemSettingsServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;
};

} // namespace blog::servlet

#endif // __BLOG_SERVLETS_SETTINGS_SYSTEM_SETTINGS_SERVLET_H__
