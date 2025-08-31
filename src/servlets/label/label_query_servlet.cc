#include "label_query_servlet.h"
#include "../../manager/label_manager.h"
#include <chen/log/log.h>


namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

LabelQueryServlet::LabelQueryServlet()
    :BlogLoginedServlet("LabelQueryServlet") {
}

int32_t LabelQueryServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
	do {
		int64_t user_id = request->getParamAs<int64_t>("user_id");
		std::string ids = request->getParam("ids");
		if (user_id == 0 && ids.empty()) {
			result->setResult(400, "get user_id and ids is null");
			break;
		}

		std::vector<data::LabelInfo::ptr> infos;
		if (user_id) {
			LabelMgr::GetInstance()->listByUserId(infos, user_id, true);
		} else {
			auto tmp = sylar::split(ids, ",");
			for (auto& i : tmp) {
				auto id = sylar::TypeUtil::Atoi(i);
				if (id) {
					auto info = LabelMgr::GetInstance()->get(id);
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
			result->jsondata.append(v);
		}
	} while (0);
	response->setBody(result->toJsonString());
    return 0;
}

}
}
