#include "subject_create_servlet.h"
#include <chen/log/log.h>
#include "../../manager/user_manager.h"
#include "../../manager/subject_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

SubjectCreateServlet::SubjectCreateServlet()
    : BlogLoginedServlet("SubjectCreateServlet") {
}

int32_t SubjectCreateServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
	do {
		DEFINE_AND_CHECK_STRING(result, name, "name");
		DEFINE_AND_CHECK_STRING(result, color, "color");

		int64_t uid = getUserId(request);
		if (!uid) {
			result->setResult(500, "not login");
			break;
		}
		if (UserMgr::GetInstance()->get(uid)->getRole() != "admin") {
			result->setResult(403, "Access Denied");
			break;
		}

		bool new_subject = false;
		auto info = SubjectMgr::GetInstance()->getByName(name);
		if (!info) {
			info.reset(new data::SubjectInfo);
			info->setName(name);
			new_subject = true;
			info->setCreateTime(time(0));
		} else if (info->getIsDeleted()) {
			info->setCreateTime(time(0));
		}
		info->setColor(color);
		info->setIsDeleted(0);
		info->setUpdateTime(time(0));

		auto db = getDB();
		if (!db) {
			result->setResult(500, "get db error");
			break;
		}

		if (data::SubjectInfoDao::InsertOrUpdate(info, db)) {
			result->setResult(500, "insert or update subject fail");
			ERROR(logger) << "db error, errno=" << db->getErrno()
				<< " errstr=" << db->getErrStr();
			break;
		}

		if (new_subject) {
			SubjectMgr::GetInstance()->add(info);
		}

		result->set("id", info->getId());
		result->set("name", info->getName());
		result->set("color", info->getColor());
	} while (0);
	response->setBody(result->toJsonString());
    return 0;
}

}
}
