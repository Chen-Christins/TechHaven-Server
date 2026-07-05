#include "article_search_servlet.h"

#include <chen/log/log.h>

#include "../../index.h"
#include "../../manager/article_manager.h"
#include "../../manager/user_manager.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

ArticleSearchServlet::ArticleSearchServlet()
    :BlogServlet("ArticleSearchServlet") {
}

int32_t ArticleSearchServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        // keyword 预留，等 jiebacpp 接入后再启用 WORD 索引
        std::string keyword = request->getParam("keyword");

        int64_t category_id = request->getParamAs<int64_t>("category_id");
        int64_t label_id = request->getParamAs<int64_t>("label_id");
        int64_t state = request->getParamAs<int64_t>("state", 2);  // 默认只搜已发布
        int64_t channel = request->getParamAs<int64_t>("channel");
        int64_t user_id = request->getParamAs<int64_t>("user_id");
        std::string year_month = request->getParam("year_month");

        int64_t page = request->getParamAs<int64_t>("page", 1);
        int64_t page_size = request->getParamAs<int64_t>("page_size", 20);
        if (page < 1) page = 1;
        if (page_size < 1) page_size = 20;
        if (page_size > 100) page_size = 100;

        // 构建索引查询参数
        std::map<uint64_t, std::set<uint64_t>> params;

        if (category_id > 0) {
            params[(uint64_t)IndexType::CAT_ID].insert(category_id);
        }
        if (label_id > 0) {
            params[(uint64_t)IndexType::LABEL_ID].insert(label_id);
        }
        if (state > 0) {
            params[(uint64_t)IndexType::STATE].insert(state);
        }
        if (channel > 0) {
            params[(uint64_t)IndexType::CHANNEL].insert(channel);
        }
        if (user_id > 0) {
            params[(uint64_t)IndexType::USER_ID].insert(user_id);
        }
        if (!year_month.empty()) {
            params[(uint64_t)IndexType::YEAR_MON].insert(Index::StrHash(year_month));
        }

        // keyword 分词后加入 WORD 索引
        if (!keyword.empty()) {
            std::vector<std::string> words;
            IndexMgr::GetInstance()->cutWord(keyword, words);
            auto& wordKeys = params[(uint64_t)IndexType::WORD];
            for (auto& w : words) {
                if (w.size() >= 2) {
                    wordKeys.insert(Index::StrHash(w));
                }
            }
        }

        auto index = IndexMgr::GetInstance();

        // 无过滤条件时直接分页查 DB
        if (params.empty()) {
            int offset = (page - 1) * page_size;
            std::vector<data::ArticleInfo::ptr> infos;
            int64_t total = ArticleMgr::GetInstance()->listByUserIdPages(infos, 0, offset, page_size, true, 0);
            result->set("total", total);
            Json::Value list(Json::arrayValue);
            for (auto& i : infos) {
                Json::Value item;
                auto uinfo = UserMgr::GetInstance()->get(i->getUserId());
                item["id"] = i->getId();
                item["author"] = uinfo ? uinfo->getName() : "";
                item["title"] = i->getTitle();
                item["summary"] = i->getContent().substr(0, 100);
                item["type"] = i->getType();
                item["state"] = i->getState();
                item["views"] = i->getViews();
                item["praise"] = i->getPraise();
                item["favorites"] = i->getFavorites();
                item["publish_time"] = i->getPublishTime();
                list.append(item);
            }
            result->jsondata["list"] = list;
            break;
        }

        // 有过滤条件时，走位图索引
        uint32_t max_need = page * page_size;
        std::vector<uint64_t> ids;
        int32_t total = index->search(ids, params, max_need);

        if (total < 0) {
            result->set("total", 0);
            result->jsondata["list"] = Json::Value(Json::arrayValue);
            break;
        }

        result->set("total", total);

        // 分页切片
        int offset = (page - 1) * page_size;
        Json::Value list(Json::arrayValue);
        for (size_t i = offset; i < ids.size() && i < (size_t)(offset + page_size); ++i) {
            auto info = ArticleMgr::GetInstance()->get(ids[i]);
            if (!info) {
                continue;
            }
            Json::Value item;
            auto uinfo = UserMgr::GetInstance()->get(info->getUserId());
            item["id"] = info->getId();
            item["author"] = uinfo ? uinfo->getName() : "";
            item["title"] = info->getTitle();
            item["summary"] = info->getContent().substr(0, 100);
            item["type"] = info->getType();
            item["state"] = info->getState();
            item["views"] = info->getViews();
            item["praise"] = info->getPraise();
            item["favorites"] = info->getFavorites();
            item["publish_time"] = info->getPublishTime();
            list.append(item);
        }
        result->jsondata["list"] = list;
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
