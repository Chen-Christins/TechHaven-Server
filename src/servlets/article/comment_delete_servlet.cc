#include "comment_delete_servlet.h"
#include "../../manager/comment_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

CommentDeleteServlet::CommentDeleteServlet()
    : BlogLoginedServlet("CommentDeleteServlet") {
}

int32_t CommentDeleteServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(410, "not login");
            break;
        }

        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");

        auto comment = CommentMgr::GetInstance()->get(id);
        if (!comment || comment->getIsDeleted()) {
            result->setResult(404, "comment not found");
            break;
        }

        // only the comment author can delete
        if (comment->getUserId() != uid) {
            result->setResult(403, "permission denied");
            break;
        }

        if (!CommentMgr::GetInstance()->del(id)) {
            result->setResult(500, "delete comment failed");
            break;
        }

        result->setResult(200, "ok");
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}
