#include "label_create_servlet.h"
#include "../../manager/label_manager.h"
#include <chen/log/log.h>
#include "../../util.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

LabelCreateServlet::LabelCreateServlet()
    :BlogLoginedServlet("LabelCreateServlet") {
}

int32_t LabelCreateServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
	do {
		DEFINE_AND_CHECK_STRING(result, name, "name");

		int64_t uid = getUserId(request);
		if (!uid) {
			result->setResult(500, "not login");
			break;
		}
		
		auto info = LabelMgr::GetInstance()->getByUserIdName(uid, name);
		bool new_label = false;
		if (!info) {
			info.reset(new data::LabelInfo);
			info->setUserId(uid);
			info->setName(name);
			new_label = true;
		}
		info->setIsDeleted(0);
		info->setCreateTime(time(0));
		info->setUpdateTime(time(0));

		auto db = getDB();
		if (!db) {
			result->setResult(500, "get db error");
			break;
		}

		if (data::LabelInfoDao::InsertOrUpdate(info, db)) {
			result->setResult(500, "insert or update category fail");
			ERROR(logger) << "db error, errno=" << db->getErrno()
				<< " errstr=" << db->getErrStr();
			break;
		}

		if (new_label) {
			LabelMgr::GetInstance()->add(info);
		}

		result->set("id", info->getId());
		result->set("name", info->getName());
	} while (0);
	response->setBody(result->toJsonString());
    return 0;
}

}
}
