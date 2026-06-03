#include "user_stats_servlet.h"
#include "../../include/managers.h"
#include <set>

namespace blog {
namespace servlet {

UserStatsServlet::UserStatsServlet()
    : BlogLoginedServlet("UserStatsServlet") {
}

int32_t UserStatsServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = request->getParamAs<int64_t>("user_id", 0);
        if (!uid) {
            auto sdata = getSessionData(request, response);
            uid = sdata->getData<int64_t>(CookieKey::USER_ID);
        }
        if (!uid) {
            result->setResult(410, "not login");
            break;
        }

        std::vector<data::ArticleInfo::ptr> articles;
        ArticleMgr::GetInstance()->listByUserId(articles, uid, true);

        int64_t total_articles = 0;
        int64_t published_articles = 0;
        int64_t private_articles = 0;
        int64_t total_views = 0;
        int64_t total_likes = 0;
        std::set<int64_t> unique_labels;

        for (auto& a : articles) {
            if (a->getIsDeleted()) continue;
            total_articles++;

            int32_t state = a->getState();
            if (state == ArticleManager::PUBLISHED) {
                published_articles++;
            } else if (state == ArticleManager::PRIVATE) {
                private_articles++;
            }

            total_views += a->getViews();
            total_likes += a->getPraise();

            std::vector<data::ArticleLabelRelInfo::ptr> labels;
            ArticleLabelRelMgr::GetInstance()->listByArticleId(labels, a->getId(), true);
            for (auto& l : labels) {
                unique_labels.insert(l->getLabelId());
            }
        }

        // total_organizations
        std::vector<data::OrganizationUserRelInfo::ptr> orgs;
        OrganizationUserRelMgr::GetInstance()->getOrgByUserId(
            orgs, uid, OrganizationUserRelManager::APPROVED, true);
        int64_t total_organizations = orgs.size();

        result->setResult(200, "ok");
        result->set("total_articles", total_articles);
        result->set("published_articles", published_articles);
        result->set("private_articles", private_articles);
        result->set("total_views", total_views);
        result->set("total_likes", total_likes);
        result->set("total_comments", 0);       // 暂无评论表
        result->set("total_tags", (int64_t)unique_labels.size());
        result->set("total_organizations", total_organizations);
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}
}
}
