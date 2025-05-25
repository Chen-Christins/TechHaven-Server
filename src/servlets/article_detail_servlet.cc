#include "article_detail_servlet.h"
#include "chen/log/log.h"
#include "../util.h"
#include "../manager/article_manager.h"
#include "../manager/user_manager.h"
#include "../manager/category_manager.h"
#include "../manager/label_manager.h"
#include "../manager/article_category_rel_manager.h"
#include "../manager/article_label_rel_manager.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

ArticleDetailServlet::ArticleDetailServlet()
    :BlogServlet("ArticleDetailServlet") {
}

int32_t ArticleDetailServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        
        data::ArticleInfo::ptr info = ArticleMgr::GetInstance()->get(id);
        if (!info) {
            result->setResult(404, "invalid id");
            break;
        }
        int64_t uid = info->getUserId();
        std::string author = UserMgr::GetInstance()->get(uid)->getName();
        result->set("id", info->getId());
        result->set("author", author);
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
                result->append("categorys", c->getId());
            }
        }

        std::vector<data::ArticleLabelRelInfo::ptr> linfos;
        ArticleLabelRelMgr::GetInstance()->listByArticleId(linfos, id, true);
        for (auto& i : linfos) {
            auto l = LabelMgr::GetInstance()->get(i->getLabelId());
            if (l && l->getIsDeleted() == 0) {
                result->append("labels", l->getId());
            }
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
};

}
}
