#pragma once

#include <shared_mutex>
#include "blog/data/comment_praise_rel_info.h"
#include <chen/ds/lru_cache.h>
#include <chen/db/query_builder.h>
#include <chen/singleton.h>

namespace blog {

class CommentPraiseRelManager {
public:
    CommentPraiseRelManager();

    bool loadAll();
    void add(data::CommentPraiseRelInfo::ptr info);
    data::CommentPraiseRelInfo::ptr get(int64_t id);
    data::CommentPraiseRelInfo::ptr getByUserAndComment(int64_t user_id, int64_t comment_id);

    // praise/unpraise
    data::CommentPraiseRelInfo::ptr praise(int64_t user_id, int64_t comment_id);
    bool unpraise(int64_t user_id, int64_t comment_id);
    bool isPraising(int64_t user_id, int64_t comment_id);

    // count praises for a comment
    int64_t countByComment(int64_t comment_id);

private:
    static data::CommentPraiseRelInfo::ptr parseRow(chen::ISQLData::ptr rt);

    std::shared_mutex m_mutex;
    chen::ds::LruCache<int64_t, data::CommentPraiseRelInfo::ptr> m_cache;
};

typedef chen::Singleton<CommentPraiseRelManager> CommentPraiseRelMgr;

}
