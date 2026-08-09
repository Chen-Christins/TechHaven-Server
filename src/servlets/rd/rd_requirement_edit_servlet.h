#pragma once

#include "../../struct.h"
#include <json/json.h>
#include "blog/data/requirement_info.h"

namespace blog {
namespace servlet {

class RdRequirementEditServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<RdRequirementEditServlet> ptr;
    RdRequirementEditServlet();
    int32_t handle(chen::http::HttpRequest::ptr request
                ,chen::http::HttpResponse::ptr response
                ,chen::http::HttpSession::ptr session
                ,Result::ptr result) override;
private:
    void notifyAssignee(int64_t assignee_id, data::RequirementInfo::ptr requirement);
};

}
}
