#include "category_query_servlet.h"
#include "../../manager/category_manager.h"
#include <chen/log/log.h>


namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

CategoryQueryServlet::CategoryQueryServlet()
    :BlogLoginedServlet("CategoryQueryServlet") {
}

int32_t CategoryQueryServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
	do {
		int64_t user_id = request->getParamAs<int64_t>("user_id");
		std::string ids = request->getParam("ids");
		if (user_id == 0 && ids.empty()) {
			result->setResult(400, "get user_id and ids is null");
			break;
		}

		std::vector<data::CategoryInfo::ptr> infos;
		if (user_id) {
			CategoryMgr::GetInstance()->listByUserId(infos, user_id, true);
		} else {
			auto tmp = chen::split(ids, ",");
			for (auto& i : tmp) {
				auto id = chen::TypeUtil::Atoi(i);
				if (id) {
					auto info = CategoryMgr::GetInstance()->get(id);
					if (info) {
						infos.push_back(info);
					}
				}
			}
		}
		for (auto& i : infos) {
			Json::Value v;
			v["id"] = i->getId();
			v["name"] = i->getName();
			v["color"] = i->getColor();
			v["parent_id"] = i->getParentId();
			result->jsondata.append(v);
		}
	} while (0);
	response->setBody(result->toJsonString());
    return 0;
}

}
}
