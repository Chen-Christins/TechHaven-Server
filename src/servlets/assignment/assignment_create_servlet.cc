#include "assignment_create_servlet.h"
#include <chen/log/log.h>
#include "../../manager/user_manager.h"
#include "../../manager/assignment_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

AssignmentCreateServlet::AssignmentCreateServlet()
    : BlogLoginedServlet("AssignmentCreateServlet") {
}

int32_t AssignmentCreateServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
	do {
		DEFINE_AND_CHECK_STRING(result, name, "name");
		DEFINE_AND_CHECK_STRING(result, subject_name, "subject_name");
        DEFINE_AND_CHECK_TYPE(result, uint64_t, end_time, "end_time");
        DEFINE_AND_CHECK_TYPE(result, int32_t, file_size, "file_size");
        DEFINE_AND_CHECK_TYPE(result, int32_t, status, "status");
        DEFINE_AND_CHECK_STRING(result, description, "description");
        DEFINE_AND_CHECK_STRING(result, file_type, "file_type");

		int64_t uid = getUserId(request);
		if (!uid) {
			result->setResult(500, "not login");
			break;
		}
		if (UserMgr::GetInstance()->get(uid)->getRole() != "admin") {
			result->setResult(403, "Access Denied");
			break;
		}

		bool new_assignment = false;
		auto info = AssignmentMgr::GetInstance()->getByName(subject_name, name);
		if (!info) {
			info.reset(new data::AssignmentInfo);
			info->setName(name);
			info->setSubjectName(subject_name);
			info->setCreateTime(time(0));
			new_assignment = true;
		} else if (info->getIsDeleted()) {
			info->setCreateTime(time(0));
		}
        info->setDescription(description);
        info->setFileType(file_type);
        info->setDeadline(end_time);
        info->setMaxSize(file_size);
        info->setStatus(status);
		info->setIsDeleted(0);
		info->setUpdateTime(time(0));

		auto db = getDB();
		if (!db) {
			result->setResult(500, "get db error");
			break;
		}

		if (data::AssignmentInfoDao::InsertOrUpdate(info, db)) {
			result->setResult(500, "insert or update assignment fail");
			ERROR(logger) << "db error, errno=" << db->getErrno()
				<< " errstr=" << db->getErrStr();
			break;
		}

		if (new_assignment) {
			AssignmentMgr::GetInstance()->add(info);
		}

		result->set("id", info->getId());
		result->set("name", info->getName());
		result->set("subject_name", info->getSubjectName());
        result->set("status", info->getStatus());
        result->set("end_time", info->getDeadline());
        result->set("create_time", info->getCreateTime());
        result->set("file_size", info->getMaxSize());
        result->set("file_type", info->getFileType());
        result->set("description", info->getDescription());
	} while (0);
	response->setBody(result->toJsonString());
    return 0;
}

}
}
