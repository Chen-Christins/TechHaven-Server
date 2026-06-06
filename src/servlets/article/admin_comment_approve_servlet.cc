#include "admin_comment_approve_servlet.h"
#include "../../manager/comment_manager.h"
#include "../../manager/user_manager.h"
#include "../../manager/notification_manager.h"
#include "../../util.h"
#include <json/json.h>
#include <sstream>

namespace blog {
namespace servlet {

AdminCommentApproveServlet::AdminCommentApproveServlet()
    : BlogLoginedServlet("AdminCommentApproveServlet") {
}

int32_t AdminCommentApproveServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
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
            ids, CommentManager::APPROVED);

        Json::Value idList(Json::arrayValue);
        for (auto& id : ids) {
            idList.append(std::to_string(id));
        }
        result->set("ids", idList);
        result->set("affected", affected);
        result->setResult(200, "ok");

        // Notify comment authors
        for (auto& cid : ids) {
            auto comment = CommentMgr::GetInstance()->get(cid);
            if (!comment) continue;
            int64_t author_id = comment->getUserId();
            if (author_id == uid) continue; // Don't notify self
            chen::IOManager::GetThis()->schedule([author_id, cid, comment]() {
                std::string title = "评论审核通过";
                std::string content = "你的评论「" + comment->getContent() + "」已通过审核";
                auto notif = NotificationMgr::GetInstance()->addNotification(
                    author_id, title, content, "comment_approved", 0, comment->getArticleId(), cid);
                if (notif) {
                    Json::Value wsMsg;
                    wsMsg["id"] = notif->getId();
                    wsMsg["title"] = title;
                    wsMsg["content"] = content;
                    wsMsg["type"] = "comment_approved";
                    wsMsg["article_id"] = comment->getArticleId();
                    wsMsg["comment_id"] = cid;
                    wsMsg["is_read"] = false;
                    wsMsg["create_time"] = notif->getCreateTime();
                    NotificationMgr::GetInstance()->sendToUser(author_id, chen::JsonUtil::ToString(wsMsg));
                }
            });
        }
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}
