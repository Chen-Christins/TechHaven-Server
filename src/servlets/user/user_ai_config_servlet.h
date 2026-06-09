/**
 * @file user_ai_config_servlet.h
 * @brief 用户AI配置 — 保存与获取
 * @author Christins
 * @date 2026-06-09
 * @copyright Apache 2.0
 */
#ifndef __BLOG_SERVLETS_USER_AI_CONFIG_SERVLET_H__
#define __BLOG_SERVLETS_USER_AI_CONFIG_SERVLET_H__

#include "../../struct.h"

namespace blog {
namespace servlet {

class UserAIConfigServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<UserAIConfigServlet> ptr;
    UserAIConfigServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;
};

}
}

#endif // __BLOG_SERVLETS_USER_AI_CONFIG_SERVLET_H__
