#include "rd_organization_members_servlet.h"
#include "rd_helper.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

RdOrganizationMembersServlet::RdOrganizationMembersServlet()
    : BlogLoginedServlet("RdOrganizationMembersServlet") {
}

int32_t RdOrganizationMembersServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, org_id, "org_id");

        int64_t uid = getUserId(request);
        auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
            result->setResult(403, "not a member of this organization");
            break;
        }

        std::vector<data::OrganizationUserRelInfo::ptr> members;
        OrganizationUserRelMgr::GetInstance()->getByPages(members, org_id, 0, UINT64_MAX, OrganizationUserRelManager::Status::APPROVED, true);

        Json::Value arr(Json::arrayValue);
        for (auto& m : members) {
            Json::Value item;
            item["user_id"] = m->getUserId();
            item["name"] = rd::getUserName(m->getUserId());
            item["role"] = rd::orgRoleToFrontend(m->getRole());
            item["avatar"] = rd::getUserAvatar(m->getUserId());
            arr.append(item);
        }

        result->set("list", arr);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
