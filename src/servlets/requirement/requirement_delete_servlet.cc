#include "requirement_delete_servlet.h"
#include "blog/data/requirement_info.h"
#include <chen/log/log.h>
#include "../../util.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/requirement_manager.h"
#include "../../permission.h"
#include <set>

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

RequirementDeleteServlet::RequirementDeleteServlet()
    : BlogLoginedServlet("RequirementDeleteServlet") {
}

int32_t RequirementDeleteServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, org_id, "org_id");
        DEFINE_AND_CHECK_STRING(result, ids, "ids");

        std::set<int64_t> req_ids;
        auto tmp = chen::split(ids, ",");
        for (auto& i : tmp) {
            req_ids.insert(chen::TypeUtil::Atoi(i));
        }

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }

        auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
            result->setResult(403, "not a member of this organization");
            break;
        }
        int32_t orgRole = rel->getRole();

        if (!permission::canDeleteRequirement(orgRole)) {
            result->setResult(403, "Access Denied");
            break;
        }

        std::vector<data::RequirementInfo::ptr> all;
        RequirementMgr::GetInstance()->listByOrg(all, org_id, 0, UINT64_MAX, -1, true);

        std::vector<data::RequirementInfo::ptr> del_items;
        for (auto& i : all) {
            if (req_ids.count(i->getId())) {
                del_items.push_back(i);
            }
        }

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db error");
            break;
        }

        auto trans = db->openTransaction();
        if (!trans) {
            result->setResult(500, "open transaction fail");
            break;
        }

        time_t now = time(0);
        for (auto& i : del_items) {
            i->setIsDeleted(1);
            i->setUpdateTime(now);
            data::RequirementInfoDao::Update(i, db);
        }
        if (!trans->commit()) {
            ERROR(logger) << "commit fail";
            result->setResult(500, "commit fail");
            for (auto& i : del_items) {
                i->setIsDeleted(0);
            }
            break;
        }

        if (!del_items.empty()) {
            auto& jids = result->jsondata["ids"];
            for (auto& i : del_items) {
                jids.append(i->getId());
            }
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
