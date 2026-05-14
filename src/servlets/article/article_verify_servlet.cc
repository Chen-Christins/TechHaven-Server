#include "article_verify_servlet.h"
#include <chen/log/log.h>
#include <json/json.h>
#include "../../util.h"
#include "../../manager/article_manager.h"
#include "../../manager/user_manager.h"
#include "../../manager/notification_manager.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

ArticleVerifyServlet::ArticleVerifyServlet()
    :BlogLoginedServlet("ArticleVerifyServlet") {
}

int32_t ArticleVerifyServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        DEFINE_AND_CHECK_TYPE(result, int64_t, state, "state");
        
        // 传入参数state，决定文章的去留
        if (state != ArticleManager::Status::PUBLISHED
                && state != ArticleManager::Status::REJECTED) {
            result->setResult(401, "invalid state");
            break;
        }

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }

        data::ArticleInfo::ptr info = ArticleMgr::GetInstance()->get(id);
        if (!info) {
            result->setResult(401, "invalid id");
            break;
        }

        if (info->getIsDeleted()) {
            result->setResult(401, "invalid article");
            break;
        }

        if (info->getState() != ArticleManager::Status::CHECKING) {
            result->setResult(401, "invalid article state");
            break;
        }

        if (state == ArticleManager::Status::REJECTED) {
            info->setState(state);
        } else if (state == ArticleManager::Status::PUBLISHED) {
            if (info->getPublishTime() <= time(0)) {
                info->setState(ArticleManager::Status::PUBLISHED);
            } else {
                info->setState(ArticleManager::Status::PRIVATE);
            }
        }
        info->setUpdateTime(time(0));

        auto db = getDB();
        if (data::ArticleInfoDao::Update(info, db)) {
            result->setResult(500, "update article fail");
            info->setState(ArticleManager::Status::CHECKING);

            ERROR(logger) << "db error errno=" << db->getErrno()
                << " errstr=" << db->getErrStr();
            break;
        }

        // 通知作者审核结果
        {
            int64_t author_id = info->getUserId();
            bool approved = (state == ArticleManager::Status::PUBLISHED
                || info->getState() == ArticleManager::Status::PUBLISHED);
            const char* notif_type = approved
                ? "article_review_approved" : "article_review_rejected";
            std::string title = approved ? "文章审核通过" : "文章审核未通过";
            std::string content = "您的文章「" + info->getTitle() + "」"
                + (approved ? "已通过审核" : "未通过审核");

            auto notifInfo = NotificationMgr::GetInstance()->addNotification(
                author_id, title, content, notif_type, uid);
            if (notifInfo) {
                Json::Value wsMsg;
                wsMsg["id"] = notifInfo->getId();
                wsMsg["title"] = title;
                wsMsg["content"] = content;
                wsMsg["type"] = notif_type;
                wsMsg["is_read"] = false;
                wsMsg["create_time"] = notifInfo->getCreateTime();
                NotificationMgr::GetInstance()->sendToUser(
                    author_id, chen::JsonUtil::ToString(wsMsg));
            }
        }

        // 将其他管理员/审核员的 article_review_request 通知标记已读
        {
            std::vector<int64_t> userIds;
            UserMgr::GetInstance()->getAllIds(userIds, true);
            for (auto targetId : userIds) {
                auto u = UserMgr::GetInstance()->get(targetId);
                if (u && (u->getRole() == UserManager::Role::ADMIN
                        || u->getRole() == UserManager::Role::CHECKER)) {
                    NotificationMgr::GetInstance()->markReadByType(
                        targetId, "article_review_request");
                }
            }
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
