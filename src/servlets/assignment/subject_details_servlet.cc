#include "subject_details_servlet.h"
#include "../../manager/subject_manager.h"
#include "../../manager/assignment_manager.h"
#include <chen/log/log.h>
#include <chen/util/json_util.h>
#include <unordered_set>

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

SubjectDetailsServlet::SubjectDetailsServlet()
    : BlogLoginedServlet("SubjectDetailsServlet") {
}

int32_t SubjectDetailsServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
	do {
		std::vector<data::SubjectInfo::ptr> data;
		SubjectMgr::GetInstance()->listAll(data, true);

		for (const auto& i : data) {
			int64_t id = i->getId();
			auto subject = SubjectMgr::GetInstance()->get(id);
			
			Json::Value v;
			v["id"] = id;
			v["name"] = subject->getName();
			v["color"] = subject->getColor();

			std::vector<data::AssignmentInfo::ptr> infos;
			AssignmentMgr::GetInstance()->listBySubjectId(infos, id, true);
			for (const auto& info : infos) {
				Json::Value t;
				t["id"] = info->getId();
				t["name"] = info->getName();
				t["color"] = info->getColor();
				t["date"] = info->getCreateTime();
				v["works"].append(t);
			}

			result->jsondata["details"].append(v);
		}
	} while (0);
	response->setBody(result->toJsonString());
    return 0;
}

}
}
