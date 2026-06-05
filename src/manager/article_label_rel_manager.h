#pragma once

#include <shared_mutex>
#include "blog/data/article_label_rel_info.h"
#include <chen/ds/lru_cache.h>
#include <chen/db/query_builder.h>
#include <chen/singleton.h>

namespace blog {

class ArticleLabelRelManager {
public:
    ArticleLabelRelManager();

    bool loadAll();
    void add(data::ArticleLabelRelInfo::ptr info);
    data::ArticleLabelRelInfo::ptr get(int64_t id);
    bool listByArticleId(std::vector<data::ArticleLabelRelInfo::ptr>& infos, int64_t id, bool valid);
    bool listByLabelId(std::vector<data::ArticleLabelRelInfo::ptr>& infos, int64_t label_id, bool valid);
    data::ArticleLabelRelInfo::ptr getByArticleIdLabelId(int64_t article_id, int64_t label_id);
private:
    static data::ArticleLabelRelInfo::ptr parseRow(chen::ISQLData::ptr rt);

    std::shared_mutex m_mutex;
    chen::ds::LruCache<int64_t, data::ArticleLabelRelInfo::ptr> m_cache;
};

typedef chen::Singleton<ArticleLabelRelManager> ArticleLabelRelMgr;

}
