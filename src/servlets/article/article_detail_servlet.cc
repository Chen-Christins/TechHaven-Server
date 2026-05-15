#include "article_detail_servlet.h"
#include <chen/log/log.h>
#include "../../util.h"
#include "../../manager/article_manager.h"
#include "../../manager/user_manager.h"
#include "../../manager/category_manager.h"
#include "../../manager/label_manager.h"
#include "../../manager/article_category_rel_manager.h"
#include "../../manager/article_label_rel_manager.h"
#include "../../manager/user_follow_rel_manager.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

ArticleDetailServlet::ArticleDetailServlet()
    :BlogServlet("ArticleDetailServlet") {
}

int32_t ArticleDetailServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        auto type = request->getParamAs<uint32_t>("type");

		int64_t cur_uid = getUserId(request);
        if (!cur_uid) {
            result->setResult(500, "not login");
            break;
        }
		data::ArticleInfo::ptr info = ArticleMgr::GetInstance()->get(id);
        if (!info) {
            result->setResult(404, "invalid id");
            break;
        }
        int32_t state = info->getState();
        bool is_deleted = info->getIsDeleted();
        int32_t role = UserMgr::GetInstance()->get(cur_uid)->getRole();
        bool is_author = (cur_uid == info->getUserId());
        if (is_deleted) {
            result->setResult(403, "Access Denied");
            break;
        }
        if (state != ArticleManager::Status::PUBLISHED
                && role != UserManager::Role::ADMIN
                && !is_author) {
            result->setResult(403, "Access Denied");
            break;
        }
        int64_t uid = info->getUserId();
		if (type == 1 && cur_uid != uid) {
			result->setResult(403, "Access Denied");
			break;
		}
        auto authorInfo = UserMgr::GetInstance()->get(uid);
        result->set("id", info->getId());
        result->set("author", authorInfo->getName());
        result->set("author_avatar", authorInfo->getAvatar());

        // author stats
        std::vector<data::ArticleInfo::ptr> authorArticles;
        ArticleMgr::GetInstance()->listByUserId(authorArticles, uid, true);
        int64_t authorPraiseTotal = 0;
        int64_t authorArticleCount = 0;
        for (auto& a : authorArticles) {
            if (a->getState() == ArticleManager::Status::PUBLISHED) {
                authorArticleCount++;
                authorPraiseTotal += a->getPraise();
            }
        }
        result->set("author_article_count", authorArticleCount);
        result->set("author_praise_count", authorPraiseTotal);
        result->set("author_follower_count",
            UserFollowRelMgr::GetInstance()->countFollowers(uid));
        result->set("title", info->getTitle());
        result->set("content", info->getContent());
        result->set("user_id", uid);
        result->set("type", info->getType());
        result->set("publish_time", info->getPublishTime());
        result->set("update_time", info->getUpdateTime());
        result->set("state", info->getState());
        result->set("is_deleted", info->getIsDeleted());
        result->set("views", info->getViews());
        result->set("praise", info->getPraise());
        result->set("favorites", info->getFavorites());

        std::vector<data::ArticleCategoryRelInfo::ptr> cinfos;
        ArticleCategoryRelMgr::GetInstance()->listByArticleId(cinfos, id, true);

        for (auto& i : cinfos) {
            auto c = CategoryMgr::GetInstance()->get(i->getCategoryId());
            if (c && c->getIsDeleted() == 0) {
                Json::Value item;
                item["id"] = c->getId();
                item["name"] = c->getName();
                item["color"] = c->getColor();
                result->append("categorys", item);
            }
        }

        std::vector<data::ArticleLabelRelInfo::ptr> linfos;
        ArticleLabelRelMgr::GetInstance()->listByArticleId(linfos, id, true);
        for (auto& i : linfos) {
            auto l = LabelMgr::GetInstance()->get(i->getLabelId());
            if (l && l->getIsDeleted() == 0) {
                Json::Value item;
                item["id"] = l->getId();
                item["name"] = l->getName();
                item["color"] = l->getColor();
                result->append("labels", item);
            }
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
