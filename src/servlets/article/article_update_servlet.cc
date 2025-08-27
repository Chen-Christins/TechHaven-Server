#include "article_update_servlet.h"
#include <chen/log/log.h>
#include "../../manager/article_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

ArticleUpdateServlet::ArticleUpdateServlet()
    :BlogLoginedServlet("ArticleUpdateServlet") {
}

int32_t ArticleUpdateServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
	do {
		DEFINE_AND_CHECK_TYPE(result, int32_t, id, "id");
		DEFINE_AND_CHECK_STRING(result, title, "title");
		DEFINE_AND_CHECK_STRING(result, content, "content");

		int64_t uid = getUserId(request);
		if (!uid) {
			result->setResult(500, "not login");
			break;
		}

		data::ArticleInfo::ptr info = ArticleMgr::GetInstance()->get(id);
		int32_t state = info->getState();

		if (state == 1) {
			result->setResult(500, "invalid state");
			break;
		}
		info->setTitle(title);
		info->setContent(content);
		
		if (state == 2) {
			info->setState((int32_t)State::VERIFYING);
		} else if (state == 3) {
			info->setState((int32_t)State::UNPUBLISH);
		}
		info->setUpdateTime(time(0));

		auto db = getDB();
        if (!db) {
            result->setResult(500, "get db connection fail");
            break;
        }
		if (data::ArticleInfoDao::Update(info, db)) {
            result->setResult(500, "update article fail");
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
