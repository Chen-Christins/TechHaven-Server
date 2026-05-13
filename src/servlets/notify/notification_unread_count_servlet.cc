#include "notification_unread_count_servlet.h"
#include "../../manager/notification_manager.h"

namespace blog {
namespace servlet {

NotificationUnreadCountServlet::NotificationUnreadCountServlet()
    :BlogLoginedServlet("NotificationUnreadCountServlet") {
}

int32_t NotificationUnreadCountServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
		,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(410, "not login");
            break;
        }

        int64_t count = NotificationMgr::GetInstance()->unreadCount(uid);

        result->setResult(200, "ok");
        result->set("count", count);
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}
}
}
