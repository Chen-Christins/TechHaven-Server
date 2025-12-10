#include "organization_join_servlet.h"
#include <chen/log/log.h>
#include "../../util.h"
#include "../../manager/organization_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../types.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

OrganizationJoinServlet::OrganizationJoinServlet()
    : BlogLoginedServlet("OrganizationJoinServlet") {
}

int32_t OrganizationJoinServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");

        int64_t uid = getUserId(request);

        auto org = OrganizationMgr::GetInstance()->get(id);
        if (!org || org->getIsDeleted()) {
            result->setResult(404, "invalid id");
            break;
        }

        if (org->getStatus() == static_cast<int32_t>(types::Status::Organization::INACTIVE)) {
            result->setResult(403, "organization is disabled");
            break;
        }

        auto info = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(id, uid);
        if (!info) {
            info = std::make_shared<data::OrganizationUserRelInfo>();
            info->setOrgId(id);
            info->setUserId(uid);
            info->setCreateTime(time(0));
        } else if (info->getStatus() == static_cast<int32_t>(types::Status::UserOrganization::APPROVED)) {
            result->setResult(403, "you have joined this organization");
            break;
        } else if (info->getStatus() == static_cast<int32_t>(types::Status::UserOrganization::PENDING)) {
            result->setResult(403, "you have applied to join this organization");
            break;
        }
        info->setRole(static_cast<int32_t>(types::Role::Organization::MEMBER)); // 普通成员
        info->setStatus(static_cast<int32_t>(types::Status::UserOrganization::PENDING)); // 申请中
        info->setUpdateTime(time(0));

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db error");
            break;
        }

        if (data::OrganizationUserRelInfoDao::InsertOrUpdate(info, db)) {
            result->setResult(500, "insert or update organization fail");
            ERROR(logger) << "db error, errno=" << db->getErrno()
				<< " errstr=" << db->getErrStr();
            break;
        }

        OrganizationUserRelMgr::GetInstance()->add(info);

        result->set("id", org->getId());
        result->set("name", org->getName());
        result->set("description", org->getDescription());
        result->set("type", org->getType());
        result->set("status", org->getStatus());
        result->set("user_in_org", info->getStatus());
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
