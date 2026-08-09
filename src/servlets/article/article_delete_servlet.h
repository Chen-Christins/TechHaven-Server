#pragma once

#include "../../struct.h"

namespace blog {
namespace servlet {

class ArticleDeleteServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<ArticleDeleteServlet> ptr;
    ArticleDeleteServlet();
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                    ,chen::http::HttpResponse::ptr response
                    ,chen::http::HttpSession::ptr session
                    ,Result::ptr result) override;
};

}
}
