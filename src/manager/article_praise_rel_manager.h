#ifndef __BLOG_MANAGER_ARTICLE_PRAISE_REL_MANAGER_H__
#define __BLOG_MANAGER_ARTICLE_PRAISE_REL_MANAGER_H__

#include <shared_mutex>
#include <unordered_map>
#include <map>
#include "blog/data/article_praise_rel_info.h"
#include <chen/singleton.h>

namespace blog {

class ArticlePraiseRelManager {
public:
    bool loadAll();
    void add(data::ArticlePraiseRelInfo::ptr info);
    data::ArticlePraiseRelInfo::ptr get(int64_t id);
    data::ArticlePraiseRelInfo::ptr getByUserAndArticle(int64_t user_id, int64_t article_id);

    // praise/unpraise
    data::ArticlePraiseRelInfo::ptr praise(int64_t user_id, int64_t article_id);
    bool unpraise(int64_t user_id, int64_t article_id);
    bool isPraising(int64_t user_id, int64_t article_id);

    // queries
    void listByArticle(std::vector<data::ArticlePraiseRelInfo::ptr>& results,
        int64_t article_id, uint64_t offset, uint64_t size);
    void listByUser(std::vector<data::ArticlePraiseRelInfo::ptr>& results,
        int64_t user_id, uint64_t offset, uint64_t size);

    int64_t countByArticle(int64_t article_id);
    int64_t countByUser(int64_t user_id);

private:
    std::shared_mutex m_mutex;
    std::unordered_map<int64_t, data::ArticlePraiseRelInfo::ptr> m_datas;
    // user_id -> (article_id -> info)
    std::unordered_map<int64_t, std::map<int64_t, data::ArticlePraiseRelInfo::ptr>> m_userPraises;
    // article_id -> (user_id -> info)
    std::unordered_map<int64_t, std::map<int64_t, data::ArticlePraiseRelInfo::ptr>> m_articlePraises;
};

typedef chen::Singleton<ArticlePraiseRelManager> ArticlePraiseRelMgr;

}

#endif // __BLOG_MANAGER_ARTICLE_PRAISE_REL_MANAGER_H__
