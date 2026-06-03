#include "user_is_following_servlet.h"
#include "../../manager/user_follow_rel_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

UserIsFollowingServlet::UserIsFollowingServlet()
    : BlogLoginedServlet("UserIsFollowingServlet") {
}

int32_t UserIsFollowingServlet::handle(chen::http::HttpRequest::ptr request,
    chen::http::HttpResponse::ptr response,
    chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(410, "not login");
            break;
        }

        DEFINE_AND_CHECK_TYPE(result, int64_t, user_id, "user_id");

        bool following = UserFollowRelMgr::GetInstance()->isFollowing(uid, user_id);

        result->setResult(200, "ok");
        result->set("is_following", following);
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}
