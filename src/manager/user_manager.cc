#include "user_manager.h"

#include "cache_util.h"
#include "../util.h"

#include <chen/log/log.h>

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 1000;

UserManager::UserManager()
    :m_cache(32, kCacheMaxSize, 0) {
}

data::UserInfo::ptr UserManager::parseRow(chen::ISQLData::ptr rt) {
    data::UserInfo::ptr v(new data::UserInfo);
    v->setId(rt->getInt64(0));
    v->setName(rt->getString(1));
    v->setAccount(rt->getString(2));
    v->setAvatar(rt->getString(3));
    v->setEmail(rt->getString(4));
    v->setRole(rt->getInt32(5));
    v->setPasswd(rt->getString(6));
    v->setState(rt->getInt32(7));
    v->setBio(rt->getString(8));
    v->setWebsite(rt->getString(9));
    v->setLocation(rt->getString(10));
    v->setToken(rt->getString(11));
    v->setTokenTime(rt->getInt64(12));
    v->setLoginTime(rt->getTime(13));
    v->setIsDeleted(rt->getInt32(14));
    v->setCreateTime(rt->getTime(15));
    v->setUpdateTime(rt->getTime(16));
    return v;
}


void UserManager::add(blog::data::UserInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

void UserManager::update(blog::data::UserInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

void UserManager::getAllIds(std::vector<int64_t>& ids, bool isValid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return;
    }
    auto qb = chen::QueryBuilder::Create("user");
    qb->whereIf(isValid, "is_deleted", "=", (int64_t)0);
    qb->select("id");
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
    while (rt->next()) {
        ids.push_back(rt->getInt64(0));
    }
}

uint64_t UserManager::listByPages(std::vector<blog::data::UserInfo::ptr>& infos, uint64_t offset, uint64_t size
        , int32_t role, int32_t state, int32_t days, bool isValid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }

    int64_t start_time = 0;
    if (days > 0) {
        start_time = time(0) - days * 24 * 3600;
    }

    auto qb = chen::QueryBuilder::Create("user");
    qb->whereIf(role != -1, "role", "=", (int64_t)role);
    qb->whereIf(state != -1, "state", "=", (int64_t)state);
    qb->whereIf(days > 0, "create_time", ">=", start_time);
    qb->whereIf(isValid, "is_deleted", "=", (int64_t)0);
    qb->orderBy("id", "DESC");

    std::stringstream ck;
    ck << "usr:list:" << role << ":" << state << ":" << days << ":" << (isValid ? "1" : "0");
    int64_t total = executeCountCached(qb, db, ck.str());
    if (total == 0) {
        return 0;
    }

    qb->limit((int32_t)size);
    qb->offset((int32_t)offset);
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
        infos.push_back(info);
        if (!m_cache.exists(info->getId())) {
    m_cache.set(info->getId(), info);
        }
    }
    return total;
}

blog::data::UserInfo::ptr UserManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::UserInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

blog::data::UserInfo::ptr UserManager::getByAccount(const std::string& v) {
    int64_t cachedId = getCachedIdMapping("usr:acct:" + v);
    if (cachedId > 0) {
        return get(cachedId);
    }
    // Check cache first
    // Since cache is only keyed by id, query DB directly via DAO
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    auto info = data::UserInfoDao::QueryByAccount(v, db);
    if (info) {
        m_cache.set(info->getId(), info);
        cacheIdMapping("usr:acct:" + v, info->getId());
    }
    return info;
}

blog::data::UserInfo::ptr UserManager::getByEmail(const std::string& v) {
    int64_t cachedId = getCachedIdMapping("usr:eml:" + v);
    if (cachedId > 0) {
        return get(cachedId);
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    auto info = data::UserInfoDao::QueryByEmail(v, db);
    if (info) {
        m_cache.set(info->getId(), info);
        cacheIdMapping("usr:eml:" + v, info->getId());
    }
    return info;
}

blog::data::UserInfo::ptr UserManager::getByName(const std::string& v) {
    int64_t cachedId = getCachedIdMapping("usr:name:" + v);
    if (cachedId > 0) {
        return get(cachedId);
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    auto info = data::UserInfoDao::QueryByName(v, db);
    if (info) {
        m_cache.set(info->getId(), info);
        cacheIdMapping("usr:name:" + v, info->getId());
    }
    return info;
}

std::string UserManager::GetToken(data::UserInfo::ptr info, int64_t us) {
    std::stringstream ss;
    ss << info->getId()
       << "|" << info->getAccount()
       << "|" << info->getEmail()
       << "|" << info->getPasswd()
       << "|" << us;
    return chen::EncryptorUtil::MD5(ss.str());
}

std::string UserManager::generateToken() {
    std::stringstream ss;
    ss << std::hex << chen::GetCurrentUs() << rand() << rand();
    return chen::EncryptorUtil::MD5(ss.str());
}

}
