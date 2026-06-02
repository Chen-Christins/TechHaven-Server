#include "organization_apply_create_servlet.h"
#include <chen/log/log.h>
#include "../../util.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_apply_manager.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

OrganizationApplyCreateServlet::OrganizationApplyCreateServlet()
    : BlogLoginedServlet("OrganizationApplyCreateServlet") {
}

int32_t OrganizationApplyCreateServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, name, "name");
        DEFINE_AND_CHECK_STRING(result, type, "type");
        std::string desc = request->getParamAs<std::string>("desc");

        int64_t user_id = getUserId(request);
        if (!user_id) {
            result->setResult(410, "not login");
            break;
        }

        auto uinfo = UserMgr::GetInstance()->get(user_id);
        if (!uinfo) {
            result->setResult(420, "user not exist");
            break;
        }

        auto info = std::make_shared<data::OrganizationApplyInfo>();
        info->setUserId(user_id);
        info->setOrgName(name);
        info->setOrgType(type);
        info->setOrgDescription(desc);
        info->setStatus(OrganizationApplyManager::Status::PENDING);
        info->setCreatedAt(time(0));
        info->setIsDeleted(0);

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db error");
            break;
        }

        if (data::OrganizationApplyInfoDao::Insert(info, db)) {
            result->setResult(500, "insert organization apply fail");
            ERROR(logger) << "db error, errno=" << db->getErrno()
                << " errstr=" << db->getErrStr();
            break;
        }

        OrganizationApplyMgr::GetInstance()->add(info);

        result->set("apply_id", info->getId());
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
