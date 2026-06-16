#include "article_manager.h"
#include "cache_util.h"
#include "../util.h"
#include <chen/log/log.h>
#include <chen/iomanager/iomanager.h>
#include <chen/db/redis.h>

#include <ctime>
#include <set>

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 1000;

ArticleManager::ArticleManager()
    :m_cache(32, kCacheMaxSize, 0) {
}

data::ArticleInfo::ptr ArticleManager::parseRow(chen::ISQLData::ptr rt) {
    data::ArticleInfo::ptr v(new data::ArticleInfo);
    v->setId(rt->getInt64(0));
    v->setUserId(rt->getInt64(1));
    v->setTitle(rt->getString(2));
    v->setContent(rt->getString(3));
    v->setType(rt->getInt32(4));
    v->setState(rt->getInt32(5));
    v->setChannel(rt->getInt64(6));
    v->setIsDeleted(rt->getInt32(7));
    v->setPublishTime(rt->getTime(8));
    v->setWeight(rt->getInt64(9));
    v->setViews(rt->getInt64(10));
    v->setPraise(rt->getInt64(11));
    v->setFavorites(rt->getInt64(12));
    v->setCreateTime(rt->getTime(13));
    v->setUpdateTime(rt->getTime(14));
    return v;
}


void ArticleManager::add(blog::data::ArticleInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

blog::data::ArticleInfo::ptr ArticleManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::ArticleInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

bool ArticleManager::listByUserId(std::vector<data::ArticleInfo::ptr>& infos, int64_t id, bool valid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }
    auto qb = chen::QueryBuilder::Create("article");
    qb->where("user_id", "=", id);
    qb->whereIf(valid, "is_deleted", "=", (int64_t)0);
    qb->orderBy("id", "DESC");
    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return false;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (!rt) {
        return false;
    }
    while (rt->next()) {
        auto info = parseRow(rt);
        auto cached = m_cache.get(info->getId());
        if (cached) {
            infos.push_back(cached);
        } else {
            m_cache.set(info->getId(), info);
            infos.push_back(info);
        }
    }
    return true;
}

int64_t ArticleManager::listByUserIdPages(std::vector<data::ArticleInfo::ptr>& infos, int64_t id
        ,int32_t offset, int32_t size, bool valid, int state) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("article");
    qb->whereIf(id != 0, "user_id", "=", id);
    qb->whereIf(state != 0, "state", "=", (int64_t)state);
    qb->whereIf(valid, "is_deleted", "=", (int64_t)0);
    qb->orderBy("id", "DESC");
    qb->limit(size);
    qb->offset(offset);

    int64_t total = 0;
    if (qb->executeCount(total, db)) {
        ERROR(logger) << "executeCount fail errno=" << db->getErrno();
        return 0;
    }
    if (total == 0) {
        return 0;
    }

    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return 0;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (!rt) {
        return 0;
    }
    while (rt->next()) {
        auto info = parseRow(rt);
        auto cached = m_cache.get(info->getId());
        if (cached) {
            infos.push_back(cached);
        } else {
            m_cache.set(info->getId(), info);
            infos.push_back(info);
        }
    }
    return total;
}

int64_t ArticleManager::listByLabelPages(std::vector<data::ArticleInfo::ptr>& infos, int64_t label_id
        ,int32_t offset, int32_t size, bool valid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("article a");
    qb->select("a.*");
    qb->join("article_label_rel alr", "a.id = alr.article_id");
    qb->where("alr.label_id", "=", label_id);
    qb->where("alr.is_deleted", "=", (int64_t)0);
    qb->where("a.state", "=", (int64_t)Status::PUBLISHED);
    qb->whereIf(valid, "a.is_deleted", "=", (int64_t)0);
    qb->orderBy("a.id", "DESC");
    qb->limit(size);
    qb->offset(offset);

    // 结果缓存：仅对首页做缓存
    std::string listKey = "art:lbl:" + std::to_string(label_id) + ":" + (valid ? "1" : "0")
                        + ":" + std::to_string(offset) + ":" + std::to_string(size);
    std::vector<int64_t> cachedIds;
    if (getCachedListResult(listKey, cachedIds)) {
        for (auto id : cachedIds) {
            auto info = get(id);
            if (info) {
                infos.push_back(info);
            }
        }
        return executeCountCached(qb, db,
            "art:lbl:" + std::to_string(label_id) + ":" + (valid ? "1" : "0"));
    }

    int64_t total = executeCountCached(qb, db,
        "art:lbl:" + std::to_string(label_id) + ":" + (valid ? "1" : "0"));
    if (total == 0) {
        return 0;
    }

    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return 0;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (!rt) {
        return 0;
    }
    std::vector<int64_t> ids;
    while (rt->next()) {
        auto info = parseRow(rt);
        ids.push_back(info->getId());
        infos.push_back(info);
    }
    cacheListResult(listKey, ids);
    return total;
}

int64_t ArticleManager::listByCategoryPages(std::vector<data::ArticleInfo::ptr>& infos, int64_t category_id
        ,int32_t offset, int32_t size, bool valid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("article a");
    qb->select("a.*");
    qb->join("article_category_rel acr", "a.id = acr.article_id");
    qb->where("acr.category_id", "=", category_id);
    qb->where("acr.is_deleted", "=", (int64_t)0);
    qb->where("a.state", "=", (int64_t)Status::PUBLISHED);
    qb->whereIf(valid, "a.is_deleted", "=", (int64_t)0);
    qb->orderBy("a.id", "DESC");
    qb->limit(size);
    qb->offset(offset);

    // 结果缓存：仅对首页做缓存
    std::string listKey = "art:cat:" + std::to_string(category_id) + ":" + (valid ? "1" : "0")
                        + ":" + std::to_string(offset) + ":" + std::to_string(size);
    std::vector<int64_t> cachedIds;
    if (getCachedListResult(listKey, cachedIds)) {
        for (auto id : cachedIds) {
            auto info = get(id);
            if (info) {
                infos.push_back(info);
            }
        }
        return executeCountCached(qb, db,
            "art:cat:" + std::to_string(category_id) + ":" + (valid ? "1" : "0"));
    }

    int64_t total = executeCountCached(qb, db,
        "art:cat:" + std::to_string(category_id) + ":" + (valid ? "1" : "0"));
    if (total == 0) {
        return 0;
    }

    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return 0;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (!rt) {
        return 0;
    }
    std::vector<int64_t> ids;
    while (rt->next()) {
        auto info = parseRow(rt);
        ids.push_back(info->getId());
        infos.push_back(info);
    }
    cacheListResult(listKey, ids);
    return total;
}

int64_t ArticleManager::listByPages(std::vector<data::ArticleInfo::ptr>& infos, int32_t offset, int state
        , int category, int32_t role, int32_t days, int32_t size, bool valid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }

    auto qb = chen::QueryBuilder::Create("article a");
    qb->select("a.*");

    if (category > 0) {
        qb->join("article_category_rel acr", "a.id = acr.article_id");
        qb->where("acr.category_id", "=", (int64_t)category);
        qb->where("acr.is_deleted", "=", (int64_t)0);
    }
    if (role != -1) {
        qb->join("user u", "a.user_id = u.id");
        qb->where("u.role", "=", (int64_t)role);
        qb->where("u.is_deleted", "=", (int64_t)0);
    }
    qb->whereIf(state != 0, "a.state", "=", (int64_t)state);
    qb->whereIf(valid, "a.is_deleted", "=", (int64_t)0);
    if (days > 0) {
        time_t now = time(0);
        qb->whereSQL("a.create_time >= ?", (int64_t)(now - days * 24 * 3600));
    }
    qb->orderBy("a.id", "DESC");
    qb->limit(size);
    qb->offset(offset);

    {
        std::stringstream ck;
        ck << "art:list:" << state << ":" << category << ":" << role << ":" << days << ":" << (valid ? "1" : "0");
        int64_t total = executeCountCached(qb, db, ck.str());
        if (total == 0) {
            return 0;
        }

        std::string sql = qb->buildQuerySQL();
        auto stmt = db->prepare(sql);
        if (!stmt) {
            ERROR(logger) << "stmt=" << sql
                     << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
            return 0;
        }
        qb->bindParams(stmt);
        auto rt = stmt->query();
        if (!rt) {
            return 0;
        }
        while (rt->next()) {
            auto info = parseRow(rt);
            auto cached = m_cache.get(info->getId());
            if (cached) {
                infos.push_back(cached);
            } else {
                m_cache.set(info->getId(), info);
                infos.push_back(info);
            }
        }
        return total;
    }
}

void ArticleManager::delVerify(int64_t id) {
    m_cache.del(id);
}

void ArticleManager::addVerify(data::ArticleInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

int64_t ArticleManager::listVerifyPages(std::vector<data::ArticleInfo::ptr>& infos, int32_t offset, int32_t size) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("article");
    qb->where("state", "=", (int64_t)Status::CHECKING);
    qb->orderBy("id", "DESC");
    qb->limit(size);
    qb->offset(offset);

    int64_t total = executeCountCached(qb, db, "art:verify", 60);
    if (total == 0) {
        return 0;
    }

    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return 0;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (!rt) {
        return 0;
    }
    while (rt->next()) {
        auto info = parseRow(rt);
        if (info->getIsDeleted()) {
            m_cache.del(info->getId());
            continue;
        }
        infos.push_back(info);
    }
    return total;
}

std::pair<data::ArticleInfo::ptr, data::ArticleInfo::ptr> ArticleManager::nearby(int64_t id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return std::pair(nullptr, nullptr);
    }

    // 查找上一篇（id 最大的小于给定 id 的已发布文章）
    data::ArticleInfo::ptr prev;
    {
        auto qb = chen::QueryBuilder::Create("article");
        qb->whereSQL("id < ?", id);
        qb->where("state", "=", (int64_t)Status::PUBLISHED);
        qb->where("is_deleted", "=", (int64_t)0);
        qb->orderBy("id", "DESC");
        qb->limit(1);
        std::string sql = qb->buildQuerySQL();
        auto stmt = db->prepare(sql);
        if (stmt) {
            qb->bindParams(stmt);
            auto rt = stmt->query();
            if (rt && rt->next()) {
                prev = parseRow(rt);
                auto cached = m_cache.get(prev->getId());
                if (cached) {
                    prev = cached;
                } else {
                    m_cache.set(prev->getId(), prev);
                }
            }
        }
    }

    // 查找下一篇（id 最小的大于给定 id 的已发布文章）
    data::ArticleInfo::ptr next;
    {
        auto qb = chen::QueryBuilder::Create("article");
        qb->whereSQL("id > ?", id);
        qb->where("state", "=", (int64_t)Status::PUBLISHED);
        qb->where("is_deleted", "=", (int64_t)0);
        qb->orderBy("id", "ASC");
        qb->limit(1);
        std::string sql = qb->buildQuerySQL();
        auto stmt = db->prepare(sql);
        if (stmt) {
            qb->bindParams(stmt);
            auto rt = stmt->query();
            if (rt && rt->next()) {
                next = parseRow(rt);
                auto cached = m_cache.get(next->getId());
                if (cached) {
                    next = cached;
                } else {
                    m_cache.set(next->getId(), next);
                }
            }
        }
    }

    return std::pair(prev, next);
}

ArticleManager::ArticleStats ArticleManager::getStats(int32_t category, int32_t role, int32_t days, const std::string& keyword) {
    ArticleStats stats;

    // 无过滤条件时走缓存
    if (category <= 0 && role == -1 && days <= 0 && keyword.empty()) {
        std::string cached;
        if (getCachedStringResult("art:stats", cached)) {
            std::stringstream ss(cached);
            std::string token;
            auto next = [&]() -> int64_t {
                std::getline(ss, token, '|');
                return chen::TypeUtil::Atoi(token);
            };
            stats.total = next();
            stats.pending = next();
            stats.published = next();
            stats.rejected = next();
            stats.reported = next();
            return stats;
        }
    }

    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return stats;
    }

    auto qb = chen::QueryBuilder::Create("article a");
    qb->select("a.state, COUNT(*) as cnt");

    if (category > 0) {
        qb->join("article_category_rel acr", "a.id = acr.article_id");
        qb->where("acr.category_id", "=", (int64_t)category);
        qb->where("acr.is_deleted", "=", (int64_t)0);
    }
    if (role != -1) {
        qb->join("user u", "a.user_id = u.id");
        qb->where("u.role", "=", (int64_t)role);
        qb->where("u.is_deleted", "=", (int64_t)0);
    }
    qb->where("a.is_deleted", "=", (int64_t)0);
    if (days > 0) {
        time_t now = time(0);
        qb->whereSQL("a.create_time >= ?", (int64_t)(now - days * 24 * 3600));
    }
    if (!keyword.empty()) {
        qb->where("a.title", "LIKE", "%" + keyword + "%");
    }
    qb->groupBy("a.state");

    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return stats;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (!rt) {
        return stats;
    }

    while (rt->next()) {
        int32_t st = rt->getInt32(0);
        int64_t cnt = rt->getInt64(1);
        stats.total += cnt;
        switch (st) {
        case Status::CHECKING:
            stats.pending += cnt;
            break;
        case Status::PUBLISHED:
            stats.published += cnt;
            break;
        case Status::REJECTED:
            stats.rejected += cnt;
            break;
        }
    }

    // 无过滤条件时缓存结果
    if (category <= 0 && role == -1 && days <= 0 && keyword.empty()) {
        std::stringstream ss;
        ss << stats.total << "|" << stats.pending << "|" << stats.published << "|" << stats.rejected << "|" << stats.reported;
        cacheStringResult("art:stats", ss.str());
    }

    return stats;
}

std::string ArticleManager::statusString() {
    std::stringstream ss;
    ss << "ArticleManager cache=" << m_cache.toStatusString();
    return ss.str();
}

void ArticleManager::start() {
    // 定时任务已迁移至 BlogModule::onTick() 统一调度
}

void ArticleManager::stop() {
    // 停止前将脏数据（浏览/点赞/收藏数）刷新到 DB，避免 reload/stop 时丢失
    flushDirty();
}

void ArticleManager::flushDirty() {
    std::set<int64_t> updates;
    {
        std::unique_lock<std::shared_mutex> lock(m_viewsMutex);
        updates.swap(m_updates);
    }

    if (updates.empty()) {
        return;
    }
    auto conn = GetDB();
    if (!conn) {
        ERROR(logger) << "flushDirty: get db connect fail";

        std::unique_lock<std::shared_mutex> lock(m_viewsMutex);
        for (auto& i : updates) {
            m_updates.insert(i);
        }
        return;
    }
    auto trans = conn->openTransaction();
    for (auto& i : updates) {
        auto info = get(i);
        if (info) {
            if (data::ArticleInfoDao::Update(info, conn)) {
                addUpdate(i);
            }
        }
    }
    trans->commit();
    INFO(logger) << "flushDirty: flushed " << updates.size() << " articles";
}

bool ArticleManager::incViews(uint64_t id, const std::string& cookie_id, uint64_t user_id) {
    auto info = get(id);
    if (!info) {
        return false;
    }
    bool v = addViews(id, cookie_id);
    if (v) {
        info->setViews(info->getViews() + 1);
        addUpdate(id);
        chen::IOManager::GetThis()->schedule([user_id]() {
            chen::RedisUtil::Cmd("blog", "incr blog:total_visits");
            auto rpy = chen::RedisUtil::Cmd("blog", "incr blog:today_visits");
            if (rpy && rpy->integer == 1) {
                int64_t now = time(0);
                int64_t tomorrow_midnight = now - (now % 86400) + 86400;
                chen::RedisUtil::Cmd("blog", "expireat blog:today_visits %lld", tomorrow_midnight);
            }
            chen::RedisUtil::Cmd("blog", "pfadd blog:visitors %lld", user_id);
        });
    }
    return true;
}

bool ArticleManager::incPraise(uint64_t id, const std::string& cookie_id, uint64_t user_id) {
    auto info = get(id);
    if (!info) {
        return false;
    }
    auto rpy = chen::RedisUtil::Cmd("blog", "hexist pra_a2u:%lld %lld", id, user_id);
    if (!rpy) {
        ERROR(logger) << "hexists fail";
        return false;
    }
    if (rpy->integer == 1) {
        return true;
    }
    rpy = chen::RedisUtil::Cmd("blog", "hset pra_a2u:%lld %lld %lld", id, user_id, time(0));
    if (!rpy) {
        ERROR(logger) << "hset fail";
        return false;
    }
    chen::IOManager::GetThis()->schedule([id, user_id]() {
        chen::RedisUtil::Cmd("blog", "hset pra_u2a:%lld %lld %lld", user_id, id, time(0));
    });
    info->setPraise(info->getPraise() + 1);
    addUpdate(id);

    return true;
}

bool ArticleManager::incFavorites(uint64_t id, const std::string& cookie_id, uint64_t user_id) {
    auto info = get(id);
    if (!info) {
        return false;
    }
    auto rpy = chen::RedisUtil::Cmd("blog", "hexist fav_a2u:%lld %lld", id, user_id);
    if (!rpy) {
        ERROR(logger) << "hexists fail";
        return false;
    }
    if (rpy->integer == 1) {
        return true;
    }
    rpy = chen::RedisUtil::Cmd("blog", "hset fav_a2u:%lld %lld %lld", id, user_id, time(0));
    if (!rpy) {
        ERROR(logger) << "hset fail";
        return false;
    }
    chen::IOManager::GetThis()->schedule([id, user_id]() {
        chen::RedisUtil::Cmd("blog", "hset fav_u2a:%lld %lld %lld", user_id, id, time(0));
    });
    info->setFavorites(info->getFavorites() + 1);
    addUpdate(id);

    return true;
}

bool ArticleManager::decPraise(uint64_t id, const std::string& cookie_id, uint64_t user_id) {
    auto info = get(id);
    if (!info) {
        return false;
    }
    bool v = false;
    auto rpy = chen::RedisUtil::Cmd("blog", "hdel pra_a2u:%lld %lld", id, user_id);
    if (!rpy) {
        ERROR(logger) << "hdel fail";
        return false;
    }
    if (rpy->integer == 1) {
        v = true;
    }
    rpy = chen::RedisUtil::Cmd("blog", "hdel pra_u2a:%lld %lld", user_id, id);
    if (!rpy) {
        ERROR(logger) << "hdel fail";
        return false;
    }
    if (rpy->integer == 1) {
        v = true;
    }
    if (v) {
        info->setPraise(info->getPraise() - 1);
        addUpdate(id);
    }

    return true;
}

bool ArticleManager::decFavorites(uint64_t id, const std::string& cookie_id, uint64_t user_id) {
    auto info = get(id);
    if (!info) {
        return false;
    }
    bool v = false;
    auto rpy = chen::RedisUtil::Cmd("blog", "hdel fav_a2u:%lld %lld", id, user_id);
    if (!rpy) {
        ERROR(logger) << "hdel fail";
        return false;
    }
    if (rpy->integer == 1) {
        v = true;
    }
    rpy = chen::RedisUtil::Cmd("blog", "hset fav_u2a:%lld %lld", user_id, id);
    if (!rpy) {
        ERROR(logger) << "hset fail";
        return false;
    }
    if (rpy->integer == 1) {
        v = true;
    }
    if (v) {
        info->setFavorites(info->getFavorites() - 1);
        addUpdate(id);
    }

    return true;
}

bool ArticleManager::listUserFav(int64_t id, std::map<int64_t, int64_t>& articles) {
#define PROC(id, mask, articles)                               \
    auto rpy = chen::RedisUtil::Cmd("blog", mask, id);        \
    if (!rpy) {                                                \
        ERROR(logger) << "hgetall fail";                       \
        return false;                                          \
    }                                                          \
    for (size_t i = 0; i < rpy->elements; i += 2) {            \
        articles[chen::TypeUtil::Atoi(rpy->element[i]->str)]  \
            = chen::TypeUtil::Atoi(rpy->element[i + 1]->str); \
    }                                                          \
    return true;

    PROC(id, "hgetall fav_u2a:%lld", articles);
}

bool ArticleManager::listUserPra(int64_t id, std::map<int64_t, int64_t>& articles) {
    PROC(id, "hgetall pra_u2a:%lld", articles);
}

bool ArticleManager::listArticleFav(int64_t id, std::map<int64_t, int64_t>& users) {
    PROC(id, "hgetall fav_a2u:%lld", users);
}

bool ArticleManager::listArticlePra(int64_t id, std::map<int64_t, int64_t>& users) {
    PROC(id, "hgetall pra_a2u:%lld", users);
}
#undef PROC

void ArticleManager::onTimer() {
    time_t now = time(0);
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "getDB error";
        return;
    }

    auto qb = chen::QueryBuilder::Create("article");
    qb->where("state", "!=", (int64_t)Status::PUBLISHED);
    qb->whereSQL("publish_time <= ?", (int64_t)now);

    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (!rt) {
        return;
    }

    std::vector<data::ArticleInfo::ptr> infos;
    while (rt->next()) {
        auto row = parseRow(rt);
        // 优先使用缓存中的版本（可能已有累积的浏览/点赞/收藏数），
        // 避免用 DB 中的旧值覆盖内存中的正确计数
        auto cached = m_cache.get(row->getId());
        auto info = cached ? cached : row;
        info->setState(Status::PUBLISHED);
        info->setUpdateTime(now);
        infos.push_back(info);
    }

    if (infos.empty()) {
        return;
    }

    auto trans = db->openTransaction();
    for (auto& i : infos) {
        if (data::ArticleInfoDao::Update(i, db)) {
            ERROR(logger) << "Update error errno=" << errno
                << db->getErrno() << " errstr=" << db->getErrStr()
                << " data=" << i->toJsonString();
        }
        m_cache.set(i->getId(), i);
        // 文章定时发布后，清除对应月份的日历缓存
        clearCalendarCache(i->getUserId(), i->getPublishTime());
    }
    trans->commit();
}

void ArticleManager::onUpdateTimer() {
    flushDirty();
}

bool ArticleManager::addViews(uint64_t id, const std::string& cookie_id) {
    time_t now = time(0);
    std::unique_lock<std::shared_mutex> lock(m_viewsMutex);
    auto it = m_viewsCache.find(id);
    if (it != m_viewsCache.end()) {
        auto iit = it->second.find(cookie_id);
        if (iit != it->second.end() && (now - iit->second) < 10 * 60) {
            return false;
        }
    }
    m_viewsCache[id][cookie_id] = now;
    return true;
}

void ArticleManager::addUpdate(int64_t id) {
    std::unique_lock<std::shared_mutex> lock(m_viewsMutex);
    m_updates.insert(id);
}

void ArticleManager::incPraiseCount(int64_t id) {
    auto info = get(id);
    if (!info) {
        return;
    }
    info->setPraise(info->getPraise() + 1);
    addUpdate(id);
}

void ArticleManager::decPraiseCount(int64_t id) {
    auto info = get(id);
    if (!info) {
        return;
    }
    if (info->getPraise() > 0) {
        info->setPraise(info->getPraise() - 1);
        addUpdate(id);
    }
}

int64_t ArticleManager::getTodayViews() {
    auto rpy = chen::RedisUtil::Cmd("blog", "get blog:today_visits");
    if (rpy && rpy->str) {
        return chen::TypeUtil::Atoi(rpy->str);
    }
    return 0;
}

int64_t ArticleManager::getTotalViews() {
    auto rpy = chen::RedisUtil::Cmd("blog", "get blog:total_visits");
    if (rpy && rpy->str) {
        return chen::TypeUtil::Atoi(rpy->str);
    }
    // Redis 未初始化 — 从 DB 计算并回填
    auto db = GetDB();
    if (!db) {
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("article");
    qb->select("CAST(COALESCE(SUM(views), 0) AS SIGNED)");
    qb->where("state", "=", (int64_t)Status::PUBLISHED);
    qb->where("is_deleted", "=", (int64_t)0);
    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        return 0;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    int64_t total = 0;
    if (rt && rt->next()) {
        total = rt->getInt64(0);
    }
    chen::RedisUtil::Cmd("blog", "set blog:total_visits %lld", total);
    return total;
}

int64_t ArticleManager::getTotalVisitors() {
    auto rpy = chen::RedisUtil::Cmd("blog", "pfcount blog:visitors");
    if (rpy) {
        return rpy->integer;
    }
    return 0;
}

void ArticleManager::syncStatsFromDB() {
    // 启动时从 DB（唯一权威数据源）同步统计计数到 Redis，
    // 覆盖上一个实例残留在 Redis 中的过期数据。
    //
    // 策略：直接删除 Redis 中的统计 key，然后调用 getTotalViews()，
    // 它会自动从 DB 计算 SUM(views) 并回填到 Redis。

    // 1. 删除所有统计 key，让它们从 DB 重建
    chen::RedisUtil::Cmd("blog", "del blog:total_visits");
    chen::RedisUtil::Cmd("blog", "del blog:today_visits");
    chen::RedisUtil::Cmd("blog", "del blog:visitors");

    // 2. 触发 getTotalViews() 从 DB 计算 SUM(views) 并回填 Redis
    int64_t total = getTotalViews();
    INFO(logger) << "syncStatsFromDB: blog:total_visits recalculated from DB = " << total;
}

void ArticleManager::getCalendarDays(int64_t user_id, int32_t year, int32_t month, std::vector<int32_t>& days) {
    // 先查 Redis 缓存
    {
        auto rpy = chen::RedisUtil::Cmd("blog", "GET calendar:%lld:%d:%d", user_id, year, month);
        if (rpy && rpy->str && strlen(rpy->str) > 0) {
            Json::Value cached;
            if (chen::JsonUtil::FromString(cached, rpy->str) && cached.isArray() && cached.size() > 0) {
                for (auto& v : cached) {
                    days.push_back(v.asInt());
                }
                return;
            }
        }
    }

    // 缓存未命中：查数据库
    // 直接用日期字符串比较，避免 mktime/localtime_r 与 MySQL UNIX_TIMESTAMP() 之间
    // 的时区不一致问题（前者用服务器本地时区，后者用 MySQL session 时区）
    char start_str[20], end_str[20];
    snprintf(start_str, sizeof(start_str), "%04d-%02d-01 00:00:00", year, month);
    int next_month = month + 1;
    int next_year = year;
    if (next_month > 12) {
        next_month = 1;
        next_year++;
    }
    snprintf(end_str, sizeof(end_str), "%04d-%02d-01 00:00:00", next_year, next_month);

    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "getCalendarDays: Get DB connection fail";
        return;
    }

    auto qb = chen::QueryBuilder::Create("article");
    qb->select("DISTINCT DAY(publish_time) as d");
    qb->where("user_id", "=", user_id);
    qb->where("state", "=", (int64_t)Status::PUBLISHED);
    qb->where("is_deleted", "=", (int64_t)0);
    qb->where("publish_time", ">=", std::string(start_str));
    qb->where("publish_time", "<", std::string(end_str));
    qb->orderBy("d", "ASC");

    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "getCalendarDays: stmt=" << sql
                 << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (!rt) {
        ERROR(logger) << "getCalendarDays: query returned null";
        return;
    }

    while (rt->next()) {
        days.push_back(static_cast<int32_t>(rt->getInt64(0)));
    }

    // 写入 Redis 缓存（TTL 5 分钟），空结果不缓存，避免因首次查询无数据而导致
    // 后续文章发布后仍返回空
    if (!days.empty()) {
        Json::Value daysJson(Json::arrayValue);
        for (auto d : days) {
            daysJson.append(d);
        }
        std::string jsonStr = chen::JsonUtil::ToString(daysJson);
        chen::RedisUtil::Cmd("blog", "SETEX calendar:%lld:%d:%d 300 %s", user_id, year, month, jsonStr.c_str());
    }
}

void ArticleManager::clearCalendarCache(int64_t user_id, int64_t publishTime) {
    time_t pt = static_cast<time_t>(publishTime);
    struct tm tm_pt = {};
    localtime_r(&pt, &tm_pt);
    int32_t year = tm_pt.tm_year + 1900;
    int32_t month = tm_pt.tm_mon + 1;
    chen::RedisUtil::Cmd("blog", "DEL calendar:%lld:%d:%d", user_id, year, month);
}

}
