#include "user_list_servlet.h"
#include <chen/log/log.h>
#include "../../manager/user_manager.h"
#include "../../types.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

UserListServlet::UserListServlet()
    :BlogLoginedServlet("UserListServlet") {
}

int32_t UserListServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
	do {
		int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }
		int32_t role = UserMgr::GetInstance()->get(uid)->getRole();

		if (role != (int32_t)types::Role::System::ADMIN) {
			result->setResult(403, "Access Denied");
			break;
		}
		
		std::vector<int64_t> ids;
		UserMgr::GetInstance()->getAllIds(ids, false);

		std::sort(ids.begin(), ids.end(), std::less<>());

		for (size_t i = 0; i < ids.size(); ++i) {
			result->jsondata["ids"].append(ids[i]);
		}
	} while (0);
	response->setBody(result->toJsonString());
    return 0;
}

}
}
