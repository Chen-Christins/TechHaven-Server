#pragma once

#include "blog/data/article_info.h"
#include <chen/ds/lru_cache.h>
#include <chen/db/query_builder.h>
#include <chen/singleton.h>
#include <chen/timer/timer.h>   
#include <map>
#include <mutex>
#include <set>
#include <shared_mutex>

namespace blog {

class ArticleManager {
public:
    enum Status {
        UNKNOWN = 0,
        CHECKING = 1,
        PUBLISHED = 2,
        REJECTED = 3,
        PRIVATE = 4
    };
    enum Type {
        ORIGINAL = 1,
        REPRINT = 2
    };

    ArticleManager();

    void add(blog::data::ArticleInfo::ptr info);
    blog::data::ArticleInfo::ptr get(int64_t id);
    bool listByUserId(std::vector<data::ArticleInfo::ptr>& infos, int64_t id, bool valid);
    int64_t listByUserIdPages(std::vector<data::ArticleInfo::ptr>& infos, int64_t id
                             ,int32_t offset, int32_t size, bool valid, int state);
    int64_t listByLabelPages(std::vector<data::ArticleInfo::ptr>& infos, int64_t label_id
                             ,int32_t offset, int32_t size, bool valid);
    int64_t listByCategoryPages(std::vector<data::ArticleInfo::ptr>& infos, int64_t category_id
                                ,int32_t offset, int32_t size, bool valid);

    void delVerify(int64_t id);
    void addVerify(data::ArticleInfo::ptr info);

    int64_t listVerifyPages(std::vector<data::ArticleInfo::ptr>& infos, int32_t offset, int32_t size);
    int64_t listByPages(std::vector<data::ArticleInfo::ptr>& infos, int32_t offset, int state
        , int category, int32_t role, int32_t days, int32_t size, bool valid);

    std::pair<data::ArticleInfo::ptr, data::ArticleInfo::ptr> nearby(int64_t id);
    std::string statusString();
    void start();
    void stop();

    bool incViews(uint64_t id, const std::string& cookie_id, uint64_t user_id);
    bool incPraise(uint64_t id, const std::string& cookie_id, uint64_t user_id);
    bool incFavorites(uint64_t id, const std::string& cookie_id, uint64_t user_id);
    bool decPraise(uint64_t id, const std::string& cookie_id, uint64_t user_id);
    bool decFavorites(uint64_t id, const std::string& cookie_id, uint64_t user_id);

    void incPraiseCount(int64_t id);
    void decPraiseCount(int64_t id);

    bool listUserFav(int64_t id, std::map<int64_t, int64_t>& articles);
    bool listUserPra(int64_t id, std::map<int64_t, int64_t>& articles);
    bool listArticleFav(int64_t id, std::map<int64_t, int64_t>& users);
    bool listArticlePra(int64_t id, std::map<int64_t, int64_t>& users);

    struct ArticleStats {
        int64_t total = 0;
        int64_t pending = 0;
        int64_t published = 0;
        int64_t rejected = 0;
        int64_t reported = 0;
    };
    ArticleStats getStats(int32_t category, int32_t role, int32_t days, const std::string& keyword);

    int64_t getTodayViews();
    int64_t getTotalViews();
    int64_t getTotalVisitors();

private:
    void onTimer();
    void onUpdateTimer();
    bool addViews(uint64_t id, const std::string& cookie_id);
    void addUpdate(int64_t id);

    static data::ArticleInfo::ptr parseRow(chen::ISQLData::ptr rt);

private:
    /// 定时器锁
    std::mutex m_mutex;
    /// 文章浏览数锁
    std::shared_mutex m_viewsMutex;
    /// LRU 文章缓存（最多 1000 条）
    chen::ds::HashLruCache<int64_t, data::ArticleInfo::ptr> m_cache;
    /// 文章浏览数
    std::map<int64_t, std::map<std::string, int64_t>> m_viewsCache;
    /// 待更新到 DB 的文章 ID 集合
    std::set<int64_t> m_updates;
    /// 定时发布文章定时器
    chen::Timer::ptr m_timer;
    /// 定时 flush 脏数据定时器
    chen::Timer::ptr m_updateTimer;
};

typedef chen::Singleton<ArticleManager> ArticleMgr;

}
