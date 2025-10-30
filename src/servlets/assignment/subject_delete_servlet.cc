#include "subject_delete_servlet.h"
#include "../../manager/subject_manager.h"
#include <chen/log/log.h>
#include "../../manager/user_manager.h"
#include "../../util.h"
#include <set>

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

SubjectDeleteServlet::SubjectDeleteServlet()
    : BlogLoginedServlet("SubjectDeleteServlet") {
}

int32_t SubjectDeleteServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
	do {
		DEFINE_AND_CHECK_STRING(result, ids, "ids");
		std::set<int64_t> subject_ids;
		auto tmp = chen::split(ids, ",");
		for (auto& i : tmp) {
			subject_ids.insert(chen::TypeUtil::Atoi(i));
		}

		int64_t uid = getUserId(request);
		if (!uid) {
			result->setResult(500, "not login");
			break;
		}
		if (UserMgr::GetInstance()->get(uid)->getRole() != "admin") {
			result->setResult(403, "Access Denied");
			break;
		}
		std::vector<data::SubjectInfo::ptr> infos;
		if (!SubjectMgr::GetInstance()->listAll(infos, true)) {
			break;
		}

		std::vector<data::SubjectInfo::ptr> del_subjects;
		for (auto& i : infos) {
			if (subject_ids.count(i->getId())) {
				del_subjects.push_back(i);
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
		for (auto& i : del_subjects) {
			i->setIsDeleted(1);
			i->setUpdateTime(now);
			data::SubjectInfoDao::Update(i, db);
		}
		if (!trans->commit()) {
			ERROR(logger) << "commit fail";
			result->setResult(500, "commit fail");

			for (auto& i : del_subjects) {
				i->setIsDeleted(0);
			}
			break;
		}
		if (!del_subjects.empty()) {
			auto& jids = result->jsondata["ids"];
			for (auto& i : del_subjects) {
				jids.append(i->getId());
			}
		}
	} while (0);
	response->setBody(result->toJsonString());
    return 0;
}

}
}
