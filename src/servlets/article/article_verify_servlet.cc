#include "article_verify_servlet.h"
#include <chen/log/log.h>
#include "../../util.h"
#include "../../manager/article_manager.h"
#include "../../types.h"

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
        if (state != static_cast<int32_t>(types::Status::Article::PUBLISHED) 
                && state != static_cast<int32_t>(types::Status::Article::REJECTED)) {
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

        if (info->getState() != static_cast<int32_t>(types::Status::Article::CHECKING)) {
            result->setResult(401, "invalid article state");
            break;
        }

        if (state == static_cast<int32_t>(types::Status::Article::REJECTED)) {
            info->setState(state);
        } else if (state == static_cast<int32_t>(types::Status::Article::PUBLISHED)) {
            if (info->getPublishTime() <= time(0)) {
                info->setState(static_cast<int32_t>(types::Status::Article::PUBLISHED));
            } else {
                info->setState(static_cast<int32_t>(types::Status::Article::PRIVATE));
            }
        }
        info->setUpdateTime(time(0));

        auto db = getDB();
        if (data::ArticleInfoDao::Update(info, db)) {
            result->setResult(500, "update article fail");
            info->setState(static_cast<int32_t>(types::Status::Article::CHECKING));
            
            ERROR(logger) << "db error errno=" << db->getErrno()
                << " errstr=" << db->getErrStr();
            break;
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
