#ifndef __BLOG_SERVLETS_RD_RD_REQUIREMENT_SERVLET_H__
#define __BLOG_SERVLETS_RD_RD_REQUIREMENT_SERVLET_H__

#include "../../struct.h"
#include <json/json.h>
#include "blog/data/requirement_info.h"
#include "blog/data/bug_info.h"
#include "blog/data/task_info.h"

namespace blog {
namespace servlet {

class RdRequirementServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<RdRequirementServlet> ptr;
    RdRequirementServlet();
    int32_t handle(chen::http::HttpRequest::ptr request
                ,chen::http::HttpResponse::ptr response
                ,chen::http::HttpSession::ptr session
                ,Result::ptr result) override;
private:
    int32_t handleList(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response,
                       chen::http::HttpSession::ptr session, Result::ptr result);
    int32_t handleCreate(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response,
                         chen::http::HttpSession::ptr session, Result::ptr result);
    int32_t handleDelete(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response,
                         chen::http::HttpSession::ptr session, Result::ptr result);
    void buildRequirementJson(Json::Value& item, data::RequirementInfo::ptr info);
};

class RdBugServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<RdBugServlet> ptr;
    RdBugServlet();
    int32_t handle(chen::http::HttpRequest::ptr request
                ,chen::http::HttpResponse::ptr response
                ,chen::http::HttpSession::ptr session
                ,Result::ptr result) override;
private:
    int32_t handleList(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response,
                       chen::http::HttpSession::ptr session, Result::ptr result);
    int32_t handleCreate(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response,
                         chen::http::HttpSession::ptr session, Result::ptr result);
    int32_t handleDelete(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response,
                         chen::http::HttpSession::ptr session, Result::ptr result);
    void buildBugJson(Json::Value& item, data::BugInfo::ptr info);
};

class RdTaskServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<RdTaskServlet> ptr;
    RdTaskServlet();
    int32_t handle(chen::http::HttpRequest::ptr request
                ,chen::http::HttpResponse::ptr response
                ,chen::http::HttpSession::ptr session
                ,Result::ptr result) override;
private:
    int32_t handleList(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response,
                       chen::http::HttpSession::ptr session, Result::ptr result);
    int32_t handleCreate(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response,
                         chen::http::HttpSession::ptr session, Result::ptr result);
    int32_t handleDelete(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response,
                         chen::http::HttpSession::ptr session, Result::ptr result);
    void buildTaskJson(Json::Value& item, data::TaskInfo::ptr info);
};

}
}

#endif // __BLOG_SERVLETS_RD_RD_REQUIREMENT_SERVLET_H__
