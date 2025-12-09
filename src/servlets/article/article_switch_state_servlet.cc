#include "article_switch_state_servlet.h"
#include "../../manager/user_manager.h"
#include "../../manager/article_manager.h"
#include "../../util.h"
#include <chen/log/log.h>
#include "../../types.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

ArticleSwitchStateServlet::ArticleSwitchStateServlet()
    :BlogLoginedServlet("ArticleSwitchStateServlet") {
}

int32_t ArticleSwitchStateServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, uint64_t, id, "id");
        DEFINE_AND_CHECK_TYPE(result, int, new_state, "new_state");

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }
        
        auto article = ArticleMgr::GetInstance()->get(id);
        if (!article) {
            result->setResult(404, "article not found");
            break;
        }

        int32_t role = UserMgr::GetInstance()->get(uid)->getRole();
        if (role != (int32_t)types::Role::System::ADMIN) {
            if (article->getUserId() != uid) {
                result->setResult(403, "Access Denied");
                break;
            } else if (new_state != (int32_t)State::UNPUBLISH) {
                result->setResult(400, "invalid new_state");
                break;
            }
        }

        article->setState(new_state);
        article->setUpdateTime(time(0));
        article->setPublishTime(0);

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db connection fail");
            break;
        }
        if (data::ArticleInfoDao::Update(article, db)) {
            result->setResult(500, "update article fail");
            break;
        }
        ArticleMgr::GetInstance()->add(article);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
