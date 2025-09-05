#include "label_delete_servlet.h"
#include <chen/log/log.h>
#include "blog/data/label_info.h"
#include "../../manager/label_manager.h"
#include "../../util.h"
#include <set>

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

LabelDeleteServlet::LabelDeleteServlet()
    :BlogLoginedServlet("LabelDeleteServlet") {
}

int32_t LabelDeleteServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
	do {
		DEFINE_AND_CHECK_STRING(result, ids, "ids");
		std::set<int64_t> label_ids;
		auto tmp = chen::split(ids, ",");
		for (auto& i : tmp) {
			label_ids.insert(chen::TypeUtil::Atoi(i));
		}

		int64_t uid = getUserId(request);
		if (!uid) {
			result->setResult(500, "not login");
			break;
		}
		std::vector<data::LabelInfo::ptr> infos;
		if (!LabelMgr::GetInstance()->listByUserId(infos, uid, true)) {
			break;
		}

		std::vector<data::LabelInfo::ptr> del_labels;
		for (auto& i : infos) {
			if (label_ids.count(i->getId())) {
				del_labels.push_back(i);
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
		for (auto& i : del_labels) {
			i->setIsDeleted(1);
			i->setUpdateTime(now);
			data::LabelInfoDao::Update(i, db);
		}
		if (!trans->commit()) {
			ERROR(logger) << "commit fail";
			result->setResult(500, "commit fail");

			for (auto& i : del_labels) {
				i->setIsDeleted(0);
			}
			break;
		}
		if (!del_labels.empty()) {
			auto& jids = result->jsondata["ids"];
			for (auto& i : del_labels) {
				jids.append(i->getId());
			}
		}
	} while (0);
	response->setBody(result->toJsonString());
    return 0;
}

}
}
