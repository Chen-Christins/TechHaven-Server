#include "article_admin_lists_servlet.h"
#include "../../manager/user_manager.h"
#include "../../manager/article_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

ArticleAdminListsServlet::ArticleAdminListsServlet()
    : BlogLoginedServlet("ArticleAdminListsServlet") {
}

int32_t ArticleAdminListsServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, uint64_t, page_size, "page_size");
        DEFINE_AND_CHECK_TYPE(result, uint64_t, page_num, "page_num");
        DEFINE_AND_CHECK_TYPE(result, int, state, "state");

        int64_t uid = getUserId(request);
        auto role = UserMgr::GetInstance()->get(uid)->getRole();
        if (role != "admin") {
            result->setResult(403, "Access Denied");
            break;
        }

        uint64_t offset = (page_num - 1) * page_size;
        std::vector<data::ArticleInfo::ptr> articles;
        uint64_t total = ArticleMgr::GetInstance()->listByUserIdPages(articles, 0, offset, page_size, true, state);

        result->set("total", total);
        auto& list = result->jsondata["list"];
        for (const auto& atc : articles) {
            const auto& user = UserMgr::GetInstance()->get(atc->getUserId());
            Json::Value item;
            item["id"] = atc->getId();
            item["user_id"] = atc->getUserId();
            item["author"] = user->getName();
            item["email"] = user->getEmail();
            item["title"] = atc->getTitle();
            item["state"] = atc->getState();
            item["author_role"] = user->getRole();
            item["publish_time"] = atc->getPublishTime();
            item["summary"] = atc->getContent().substr(0, 100);
            list.append(item);
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}