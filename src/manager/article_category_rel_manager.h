#ifndef __BLOG_MANAGER_ARTICLE_CATEGORY_REL_MANAGER_H__
#define __BLOG_MANAGER_ARTICLE_CATEGORY_REL_MANAGER_H__

#include <shared_mutex>
#include <unordered_map>
#include "blog/data/article_category_rel_info.h"
#include <chen/singleton.h>

namespace blog {

class ArticleCategoryRelManager {
public:
    bool loadAll();
    void add(data::ArticleCategoryRelInfo::ptr info);
    data::ArticleCategoryRelInfo::ptr get(int64_t id);
    bool listByArticleId(std::vector<data::ArticleCategoryRelInfo::ptr>& infos, int64_t id, bool valid);
    data::ArticleCategoryRelInfo::ptr getByArticleIdCategoryId(int64_t article_id, int64_t category_id);
private:
    std::shared_mutex m_mutex;
    std::unordered_map<int64_t, data::ArticleCategoryRelInfo::ptr> m_datas;
    std::unordered_map<int64_t, std::map<int64_t, data::ArticleCategoryRelInfo::ptr>> m_articles;
};

typedef sylar::Singleton<ArticleCategoryRelManager> ArticleCategoryRelMgr;

}

#endif // __BLOG_MANAGER_ARTICLE_CATEGORY_REL_MANAGER_H__