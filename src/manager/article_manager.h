#ifndef __BLOG_MANAGER_ARTICLE_MANAGER_H__
#define __BLOG_MANAGER_ARTICLE_MANAGER_H__

#include <shared_mutex>
#include "chen/singleton.h"
#include "blog/data/article_info.h"
#include <map>
#include <unordered_map>
#include <set>
#include "chen/timer/timer.h"

namespace blog {

class ArticleManager {
public:
    bool loadAll();
    void add(blog::data::ArticleInfo::ptr info);
    blog::data::ArticleInfo::ptr get(int64_t id);
    bool listByUserId(std::vector<data::ArticleInfo::ptr>& infos, int64_t id, bool valid);
    int64_t listByUserIdPages(std::vector<data::ArticleInfo::ptr>& infos, int64_t id
                             ,int32_t offset, int32_t size, bool valid, int state);
    
    void delVerify(int64_t id);
    void addVerify(data::ArticleInfo::ptr info);

    int64_t listVerifyPages(std::vector<data::ArticleInfo::ptr>& infos, int32_t offset, int32_t size);

    std::pair<data::ArticleInfo::ptr, data::ArticleInfo::ptr> nearby(int64_t id);
    std::string statusString();
    void start();
    void stop();
private:
    std::shared_mutex m_mutex;
    std::shared_mutex m_viewsMutex;
    std::map<int64_t, blog::data::ArticleInfo::ptr> m_datas;
    std::unordered_map<int64_t, std::map<int64_t, blog::data::ArticleInfo::ptr>> m_users;
    std::map<int64_t, blog::data::ArticleInfo::ptr> m_verifys;
    std::map<int64_t, std::map<std::string, int64_t>> m_viewsCache;
    std::set<int64_t> m_updates;
    sylar::Timer::ptr m_timer;
    sylar::Timer::ptr m_updateTimer;
};

typedef sylar::Singleton<ArticleManager> ArticleMgr;

}

#endif // __BLOG_MANAGER_ARTICLE_MANAGER_H__