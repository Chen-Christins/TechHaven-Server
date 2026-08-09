#pragma once

#include "../../struct.h"
#include <json/json.h>

namespace blog {
namespace servlet {

class RdCheckAccessServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<RdCheckAccessServlet> ptr;
    RdCheckAccessServlet();
    int32_t handle(chen::http::HttpRequest::ptr request
                ,chen::http::HttpResponse::ptr response
                ,chen::http::HttpSession::ptr session
                ,Result::ptr result) override;
};

}
}
