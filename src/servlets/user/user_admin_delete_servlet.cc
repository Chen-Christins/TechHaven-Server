#include "user_admin_delete_servlet.h"
#include "chen/log/log.h"
#include "../../manager/user_manager.h"
#include "../../util.h"
#include <set>

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

UserAdminDeleteServlet::UserAdminDeleteServlet()
    :BlogLoginedServlet("UserAdminDeleteServlet") {
}

int32_t UserAdminDeleteServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
	do {
		DEFINE_AND_CHECK_STRING(result, ids, "ids");

		std::set<int64_t> user_ids;
        auto tmp = sylar::split(ids, ',');
        for (auto& i : tmp) {
            user_ids.insert(sylar::TypeUtil::Atoi(i));
        }

		int64_t uid = getUserId(request);
		auto role = UserMgr::GetInstance()->get(uid)->getRole();

		if (role != "admin") {
			result->setResult(403, "Access Denied");
			break;
		}

		std::vector<data::UserInfo::ptr> infos;
		for (const int64_t& id : user_ids) {
			auto info = UserMgr::GetInstance()->get(id);
			if (info->getIsDeleted()) {
				continue;
			}
			infos.emplace_back(info);
		}

		auto db = getDB();
        auto trans = db->openTransaction();
        if (!trans) {
            result->setResult(500, "open transaction fail");
            break;
        }
		time_t now = time(0);
        for (auto& i : infos) {
            i->setIsDeleted(1);
            i->setUpdateTime(now);
            data::UserInfoDao::Update(i, db);
        }
        if (!trans->commit()) {
            ERROR(logger) << "commit fail";
            result->setResult(500, "commit fail");

            for (auto& i : infos) {
                i->setIsDeleted(0);
            }
            break;
        }
        if (!infos.empty()) {
            auto& jids = result->jsondata["ids"];
            for (auto& i : infos) {
                jids.append(i->getId());
            }
        }
	}while (0);
	response->setBody(result->toJsonString());
    return 0;
}

}
}
