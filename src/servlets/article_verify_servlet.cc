#include "article_verify_servlet.h"
#include "chen/log/log.h"
#include "../util.h"
#include "../manager/article_manager.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

ArticleVerifyServlet::ArticleVerifyServlet()
    :BlogLoginedServlet("ArticleVerifyServlet") {
}

int32_t ArticleVerifyServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        DEFINE_AND_CHECK_TYPE(result, int64_t, state, "state");

        if (state != 2 && state != 3) {
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

        if (info->getState() != 1) {
            result->setResult(401, "invalid article state");
            break;
        }

        if (state == 3) {
            info->setState(state);
        } else if (state == 2) {
            if (info->getPublishTime() <= time(0)) {
                info->setState(2);
            } else {
                info->setState(4);
            }
        }
        info->setUpdateTime(time(0));

        auto db = getDB();
        if (data::ArticleInfoDao::Update(info, db)) {
            result->setResult(500, "update article fail");
            info->setState(1);
            break;
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
};

}
}
