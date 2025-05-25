#include "article_delete_servlet.h"
#include "chen/log/log.h"
#include "blog/data/article_info.h"
#include "../manager/article_manager.h"
#include "../util.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

ArticleDeleteServlet::ArticleDeleteServlet()
    :BlogLoginedServlet("ArticleDeleteServlet") {
}

int32_t ArticleDeleteServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, ids, "ids");
        std::set<int64_t> art_ids;
        auto tmp = sylar::split(ids, ',');
        for (auto& i : tmp) {
            art_ids.insert(sylar::TypeUtil::Atoi(i));
        }
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }
        
        std::vector<data::ArticleInfo::ptr> infos;
        for (auto& id : art_ids) {
            auto info = ArticleMgr::GetInstance()->get(id);
            if (info->getUserId() != uid) {
                continue;
            }
            if (info->getIsDeleted()) {
                continue;
            }
            infos.push_back(info);
        }

        auto db = getDB();
        auto trans = db->openTransaction();
        if (!trans) {
            result->setResult(500, "open transaction fail");
            break;
        }
        time_t now = time(0);
        for (auto& i : infos) {
            i->setIsDeleted(1);
            i->setUpdateTime(now);
            data::ArticleInfoDao::Update(i, db);
        }
        if (!trans->commit()) {
            ERROR(logger) << "commit fail";
            result->setResult(500, "commit fail");

            for (auto& i : infos) {
                i->setIsDeleted(0);
            }
            break;
        }
        if (!infos.empty()) {
            auto& jids = result->jsondata["ids"];
            for (auto& i : infos) {
                jids.append(i->getId());
            }
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
};

}
}
