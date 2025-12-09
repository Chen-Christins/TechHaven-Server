#include "organization_delete_servlet.h"
#include <chen/log/log.h>
#include "../../util.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_manager.h"
#include "../../types.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

OrganizationDeleteServlet::OrganizationDeleteServlet()
    : BlogLoginedServlet("OrganizationDeleteServlet") {
}

int32_t OrganizationDeleteServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, ids, "ids");
		std::set<int64_t> assignment_ids;
		auto tmp = chen::split(ids, ",");
		for (auto& i : tmp) {
			assignment_ids.insert(chen::TypeUtil::Atoi(i));
		}

		int64_t uid = getUserId(request);
		if (!uid) {
			result->setResult(500, "not login");
			break;
		}
		if (UserMgr::GetInstance()->get(uid)->getRole() != (int32_t)types::Role::System::ADMIN) {
			result->setResult(403, "Access Denied");
			break;
		}
		std::vector<data::OrganizationInfo::ptr> infos;
		if (!OrganizationMgr::GetInstance()->listByPages(infos, 0, UINT64_MAX, -1, true)) {
			break;
		}

		std::vector<data::OrganizationInfo::ptr> del_organizations;
		for (auto& i : infos) {
			if (assignment_ids.count(i->getId())) {
				del_organizations.push_back(i);
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
		for (auto& i : del_organizations) {
			i->setIsDeleted(1);
			i->setUpdateTime(now);
			data::OrganizationInfoDao::Update(i, db);
		}
		if (!trans->commit()) {
			ERROR(logger) << "commit fail";
			result->setResult(500, "commit fail");

			for (auto& i : del_organizations) {
				i->setIsDeleted(0);
			}
			break;
		}
		if (!del_organizations.empty()) {
			auto& jids = result->jsondata["ids"];
			for (auto& i : del_organizations) {
				jids.append(i->getId());
			}
		}
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
