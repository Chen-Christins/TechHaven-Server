#include "admin_comment_spam_servlet.h"
#include "../../manager/comment_manager.h"
#include "../../manager/user_manager.h"
#include "../../util.h"
#include <json/json.h>
#include <sstream>

namespace blog {
namespace servlet {

AdminCommentSpamServlet::AdminCommentSpamServlet()
    :BlogLoginedServlet("AdminCommentSpamServlet") {
}

int32_t AdminCommentSpamServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
    	, chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }
        int32_t role = UserMgr::GetInstance()->get(uid)->getRole();
        if (role != UserManager::Role::ADMIN) {
            result->setResult(403, "Access Denied");
            break;
        }

        DEFINE_AND_CHECK_STRING(result, ids_str, "ids");

        std::vector<int64_t> ids;
        std::stringstream ss(ids_str);
        std::string token;
        while (std::getline(ss, token, ',')) {
            try {
                ids.push_back(std::stoll(token));
            } catch (...) {}
        }

        int64_t affected = CommentMgr::GetInstance()->batchUpdateStatus(
            ids, CommentManager::SPAM);

        Json::Value idList(Json::arrayValue);
        for (auto& id : ids) {
            idList.append(std::to_string(id));
        }
        result->set("ids", idList);
        result->set("affected", affected);
        result->setResult(200, "ok");
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}
