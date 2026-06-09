/**
 * @file article_ai_summary_servlet.h
 * @brief 文章AI总结 — 异步接口，转发AI原始JSON响应
 * @author Christins
 * @date 2026-06-10
 * @copyright Apache 2.0
 */
#ifndef __BLOG_SERVLETS_ARTICLE_AI_SUMMARY_SERVLET_H__
#define __BLOG_SERVLETS_ARTICLE_AI_SUMMARY_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class ArticleAISummaryServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<ArticleAISummaryServlet> ptr;
    ArticleAISummaryServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLETS_ARTICLE_AI_SUMMARY_SERVLET_H__
