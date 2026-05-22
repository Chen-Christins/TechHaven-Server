#ifndef __BLOG_SERVLETS_RD_RD_SPECIAL_SERVLET_H__
#define __BLOG_SERVLETS_RD_RD_SPECIAL_SERVLET_H__

#include "../../struct.h"
#include <json/json.h>

namespace blog {
namespace servlet {

class RdStatsServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<RdStatsServlet> ptr;
    RdStatsServlet();
    int32_t handle(chen::http::HttpRequest::ptr request
                ,chen::http::HttpResponse::ptr response
                ,chen::http::HttpSession::ptr session
                ,Result::ptr result) override;
};

class RdMyTicketsServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<RdMyTicketsServlet> ptr;
    RdMyTicketsServlet();
    int32_t handle(chen::http::HttpRequest::ptr request
                ,chen::http::HttpResponse::ptr response
                ,chen::http::HttpSession::ptr session
                ,Result::ptr result) override;
};

class RdOrganizationsServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<RdOrganizationsServlet> ptr;
    RdOrganizationsServlet();
    int32_t handle(chen::http::HttpRequest::ptr request
                ,chen::http::HttpResponse::ptr response
                ,chen::http::HttpSession::ptr session
                ,Result::ptr result) override;
};

class RdEnumsServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<RdEnumsServlet> ptr;
    RdEnumsServlet();
    int32_t handle(chen::http::HttpRequest::ptr request
                ,chen::http::HttpResponse::ptr response
                ,chen::http::HttpSession::ptr session
                ,Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLETS_RD_RD_SPECIAL_SERVLET_H__
