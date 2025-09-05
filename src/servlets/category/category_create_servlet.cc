#include "category_create_servlet.h"
#include "../../manager/category_manager.h"
#include <chen/log/log.h>
#include "../../util.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

CategoryCreateServlet::CategoryCreateServlet()
    :BlogLoginedServlet("CategoryCreateServlet") {
}

int32_t CategoryCreateServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
	do {
		DEFINE_AND_CHECK_STRING(result, name, "name");
		int64_t parent_id = request->getParamAs<int64_t>("parent_id");

		int64_t uid = getUserId(request);
		if (!uid) {
			result->setResult(500, "not login");
			break;
		}

		data::CategoryInfo::ptr parent_info;
		if (parent_id) {
			parent_info = CategoryMgr::GetInstance()->get(parent_id);
			if (!parent_info) {
				result->setResult(401, "invalid parent_id");
				break;
			}
		}

		auto info = CategoryMgr::GetInstance()->getByUserIdName(uid, name);
		bool new_cat = false;
		if (!info) {
			info.reset(new data::CategoryInfo);
			info->setUserId(uid);
			info->setName(name);
			new_cat = true;
		}
		info->setParentId(parent_id);
		info->setIsDeleted(0);
		info->setCreateTime(time(0));
		info->setUpdateTime(time(0));

		auto db = getDB();
		if (!db) {
			result->setResult(500, "get db error");
			break;
		}

		if (data::CategoryInfoDao::InsertOrUpdate(info, db)) {
			result->setResult(500, "insert or update category fail");
			ERROR(logger) << "db error, errno=" << db->getErrno()
				<< " errstr=" << db->getErrStr();
			break;
		}

		if (new_cat) {
			CategoryMgr::GetInstance()->add(info);
		}

		result->set("id", info->getId());
		result->set("name", info->getName());
		result->set("parent_id", parent_id);
	} while (0);
	response->setBody(result->toJsonString());
    return 0;
}

}
}
