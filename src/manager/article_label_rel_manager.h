#ifndef __BLOG_MANAGER_ARTICLE_LABEL_REL_MANAGER_H__
#define __BLOG_MANAGER_ARTICLE_LABEL_REL_MANAGER_H__

#include <unordered_map>
#include <shared_mutex>
#include "blog/data/article_label_rel_info.h"
#include <chen/singleton.h>

namespace blog {

class ArticleLabelRelManager {
public:
    bool loadAll();
    void add(data::ArticleLabelRelInfo::ptr info);
    data::ArticleLabelRelInfo::ptr get(int64_t id);
    bool listByArticleId(std::vector<data::ArticleLabelRelInfo::ptr>& infos, int64_t id, bool valid);
    data::ArticleLabelRelInfo::ptr getByArticleIdLabelId(int64_t article_id, int64_t category_id);
private:
    std::shared_mutex m_mutex;
    std::unordered_map<int64_t, data::ArticleLabelRelInfo::ptr> m_datas;
    std::unordered_map<int64_t, std::map<int64_t, data::ArticleLabelRelInfo::ptr>> m_articles;
};

typedef sylar::Singleton<ArticleLabelRelManager> ArticleLabelRelMgr;

}

#endif // __BLOG_MANAGER_ARTICLE_LABEL_REL_MANAGER_H__