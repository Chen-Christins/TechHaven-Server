#include "assignment_admin_lists_servlet.h"
#include "../../manager/user_manager.h"
#include "../../manager/assignment_manager.h"
#include "../../util.h"
#include <chen/log/log.h>

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

AssignmentAdminListsServlet::AssignmentAdminListsServlet()
    :BlogLoginedServlet("AssignmentAdminListsServlet") {
}

int32_t AssignmentAdminListsServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, uint64_t, page_size, "page_size");
        DEFINE_AND_CHECK_TYPE(result, uint64_t, page_num, "page_num");
        int32_t state = request->getParamAs<int32_t>("state", -1);

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }
        if (UserMgr::GetInstance()->get(uid)->getRole() != "admin") {
            result->setResult(403, "Access Denied");
            break;
        }

        uint64_t offset = (page_num - 1) * page_size;

        std::vector<data::AssignmentInfo::ptr> infos;
        uint64_t total = AssignmentMgr::GetInstance()->listByPages(infos, offset, page_size, state, true);

        result->set("total", total);
        auto& list = result->jsondata["list"];
        for (const auto& i : infos) {
            Json::Value item;
            item["id"] = i->getId();
            item["name"] = i->getName();
            item["subject_name"] = i->getSubjectName();
            item["end_time"] = i->getDeadline();
            item["status"] = i->getStatus();
            item["create_time"] = i->getCreateTime();
            item["description"] = i->getDescription();
            item["file_type"] = i->getFileType();
            item["file_size"] = i->getMaxSize();
            list.append(item);
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
