#include "article_manager.h"
#include "user_manager.h"
#include "../util.h"
#include <chen/log/log.h>
#include <chen/iomanager/iomanager.h>
#include <chen/db/redis.h>

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 1000;

ArticleManager::ArticleManager()
    :m_cache(kCacheMaxSize, 0, nullptr) {
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
        infos.push_back(parseRow(rt));
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
        infos.push_back(parseRow(rt));
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
        infos.push_back(parseRow(rt));
    }
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
        infos.push_back(parseRow(rt));
    }
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
    qb->whereIf(state != 0, "a.state", "=", (int64_t)state);
    qb->whereIf(valid, "a.is_deleted", "=", (int64_t)0);
    if (days > 0) {
        time_t now = time(0);
        qb->whereSQL("a.create_time >= ?", (int64_t)(now - days * 24 * 3600));
    }
    qb->orderBy("a.id", "DESC");
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
        // 角色筛选：需要关联 UserMgr，在 DB 外完成
        if (role != -1) {
            auto user = UserMgr::GetInstance()->get(info->getUserId());
            if (!user || user->getRole() != role) {
                --total;
                continue;
            }
        }
        if (infos.size() < (size_t)size) {
            infos.push_back(info);
        }
    }
    return total;
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
            }
        }
    }

    return std::pair(prev, next);
}

ArticleManager::ArticleStats ArticleManager::getStats(int32_t category, int32_t role, int32_t days, const std::string& keyword) {
    ArticleStats stats;
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

    // 如果需要角色筛选，先收集所有涉及的 userId 检查角色
    std::set<int64_t> roleFilterUserIds;
    if (role != -1) {
        // 角色筛选需要在分组结果上做，先单独查一次获取匹配角色的文章
        auto qb2 = chen::QueryBuilder::Create("article a");
        qb2->select("a.id, a.user_id, a.state");
        qb2->where("a.is_deleted", "=", (int64_t)0);
        if (category > 0) {
            qb2->join("article_category_rel acr", "a.id = acr.article_id");
            qb2->where("acr.category_id", "=", (int64_t)category);
            qb2->where("acr.is_deleted", "=", (int64_t)0);
        }
        if (days > 0) {
            time_t now = time(0);
            qb2->whereSQL("a.create_time >= ?", (int64_t)(now - days * 24 * 3600));
        }
        if (!keyword.empty()) {
            qb2->where("a.title", "LIKE", "%" + keyword + "%");
        }
        std::string sql2 = qb2->buildQuerySQL();
        auto stmt2 = db->prepare(sql2);
        if (stmt2) {
            qb2->bindParams(stmt2);
            auto rt2 = stmt2->query();
            if (rt2) {
                while (rt2->next()) {
                    auto userId = rt2->getInt64(1);
                    auto artState = rt2->getInt32(2);
                    auto user = UserMgr::GetInstance()->get(userId);
                    if (!user || user->getRole() != role) {
                        continue;
                    }
                    stats.total++;
                    switch (artState) {
                    case Status::CHECKING:
                        stats.pending++;
                        break;
                    case Status::PUBLISHED:
                        stats.published++;
                        break;
                    case Status::REJECTED:
                        stats.rejected++;
                        break;
                    }
                }
            }
        }
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
    return stats;
}

std::string ArticleManager::statusString() {
    std::stringstream ss;
    ss << "ArticleManager cache=" << m_cache.toStatusString();
    return ss.str();
}

void ArticleManager::start() {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_timer) {
        return;
    }
    m_timer = chen::IOManager::GetThis()->addTimer(60 * 1000
            ,std::bind(&ArticleManager::onTimer, this), true);
    m_updateTimer = chen::IOManager::GetThis()->addTimer(60 * 1000
            ,std::bind(&ArticleManager::onUpdateTimer, this), true);
}

void ArticleManager::stop() {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!m_timer) {
        return;
    }
    m_timer->cancel();
    m_timer = nullptr;

    m_updateTimer->cancel();
    m_updateTimer = nullptr;
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
        auto info = parseRow(rt);
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
    }
    trans->commit();
}

void ArticleManager::onUpdateTimer() {
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
        ERROR(logger) << "get db connect fail";

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
    qb->select("COALESCE(SUM(views), 0)");
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

}
