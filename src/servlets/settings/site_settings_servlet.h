#ifndef __BLOG_SERVLETS_SETTINGS_SITE_SETTINGS_SERVLET_H__
#define __BLOG_SERVLETS_SETTINGS_SITE_SETTINGS_SERVLET_H__

#include "../../struct.h"

namespace blog::servlet {

class SiteSettingsServlet : public BlogServlet {
public:
    typedef std::shared_ptr<SiteSettingsServlet> ptr;
    SiteSettingsServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;
};

} // namespace blog::servlet

#endif // __BLOG_SERVLETS_SETTINGS_SITE_SETTINGS_SERVLET_H__
