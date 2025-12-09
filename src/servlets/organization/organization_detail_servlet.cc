#include "organization_detail_servlet.h"
#include <chen/log/log.h>
#include "../../util.h"
#include "../../manager/organization_manager.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

OrganizationDetailServlet::OrganizationDetailServlet()
    : BlogLoginedServlet("OrganizationDetailServlet") {
}

int32_t OrganizationDetailServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");

        auto org = OrganizationMgr::GetInstance()->get(id);
        if (!org) {
            result->setResult(1, "organization not exists");
            break;
        }

        result->set("id", org->getId());
        result->set("name", org->getName());
        result->set("description", org->getDescription());
        result->set("type", org->getType());
        result->set("status", org->getStatus());
    } while(false);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
