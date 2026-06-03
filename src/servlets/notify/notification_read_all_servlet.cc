#include "notification_read_all_servlet.h"
#include "../../manager/notification_manager.h"

namespace blog {
namespace servlet {

NotificationReadAllServlet::NotificationReadAllServlet()
    : BlogLoginedServlet("NotificationReadAllServlet") {
}

int32_t NotificationReadAllServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(410, "not login");
            break;
        }

        int64_t count = NotificationMgr::GetInstance()->markAllRead(uid);

        result->setResult(200, "ok");
        result->set("count", count);
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}
