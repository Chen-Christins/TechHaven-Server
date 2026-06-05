#include "user_follow_rel_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 1000;

UserFollowRelManager::UserFollowRelManager()
    :m_cache(kCacheMaxSize, 0, nullptr) {
}

data::UserFollowRelInfo::ptr UserFollowRelManager::parseRow(chen::ISQLData::ptr rt) {
    data::UserFollowRelInfo::ptr v(new data::UserFollowRelInfo);
    v->setId(rt->getInt64(0));
    v->setFollowerId(rt->getInt64(1));
    v->setFollowingId(rt->getInt64(2));
    v->setIsDeleted(rt->getInt32(3));
    v->setCreateTime(rt->getTime(4));
    v->setUpdateTime(rt->getTime(5));
    return v;
}

bool UserFollowRelManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }
    INFO(logger) << "UserFollowRelManager loadAll: DB connection verified, no preloading needed";
    return true;
}

void UserFollowRelManager::add(data::UserFollowRelInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

data::UserFollowRelInfo::ptr UserFollowRelManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::UserFollowRelInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

data::UserFollowRelInfo::ptr UserFollowRelManager::getByFollowerAndFollowing(
    int64_t follower_id, int64_t following_id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    return data::UserFollowRelInfoDao::QueryByFollowerIdFollowingId(follower_id, following_id, db);
}

data::UserFollowRelInfo::ptr UserFollowRelManager::follow(int64_t follower_id, int64_t following_id) {
    auto existing = getByFollowerAndFollowing(follower_id, following_id);
    if (existing) {
        if (!existing->getIsDeleted()) {
            return existing;
        }
        auto db = GetDB();
        if (!db) {
            ERROR(logger) << "Get DB connection fail";
            return nullptr;
        }
        existing->setIsDeleted(0);
        existing->setUpdateTime(time(0));
        if (data::UserFollowRelInfoDao::Update(existing, db)) {
            ERROR(logger) << "UserFollowRelManager follow Update fail";
            return nullptr;
        }
        m_cache.set(existing->getId(), existing);
        return existing;
    }

    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }

    auto info = std::make_shared<data::UserFollowRelInfo>();
    info->setFollowerId(follower_id);
    info->setFollowingId(following_id);
    info->setIsDeleted(0);
    info->setCreateTime(time(0));
    info->setUpdateTime(time(0));

    if (data::UserFollowRelInfoDao::Insert(info, db)) {
        ERROR(logger) << "UserFollowRelManager follow Insert fail";
        return nullptr;
    }

    m_cache.set(info->getId(), info);
    return info;
}

bool UserFollowRelManager::unfollow(int64_t follower_id, int64_t following_id) {
    auto info = getByFollowerAndFollowing(follower_id, following_id);
    if (!info || info->getIsDeleted()) {
        return false;
    }

    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }

    info->setIsDeleted(1);
    info->setUpdateTime(time(0));
    if (data::UserFollowRelInfoDao::Update(info, db)) {
        ERROR(logger) << "UserFollowRelManager unfollow Update fail";
        return false;
    }
    m_cache.set(info->getId(), info);
    return true;
}

bool UserFollowRelManager::isFollowing(int64_t follower_id, int64_t following_id) {
    auto info = getByFollowerAndFollowing(follower_id, following_id);
    return info && !info->getIsDeleted();
}

void UserFollowRelManager::listFollowing(std::vector<data::UserFollowRelInfo::ptr>& results,
    int64_t follower_id, uint64_t offset, uint64_t size) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return;
    }
    auto qb = chen::QueryBuilder::Create("user_follow_rel");
    qb->where("follower_id", "=", follower_id);
    qb->where("is_deleted", "=", (int64_t)0);
    qb->orderBy("id", "DESC");
    qb->limit((int32_t)size);
    qb->offset((int32_t)offset);
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
        results.push_back(parseRow(rt));
    }
}

void UserFollowRelManager::listFollowers(std::vector<data::UserFollowRelInfo::ptr>& results,
    int64_t following_id, uint64_t offset, uint64_t size) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return;
    }
    auto qb = chen::QueryBuilder::Create("user_follow_rel");
    qb->where("following_id", "=", following_id);
    qb->where("is_deleted", "=", (int64_t)0);
    qb->orderBy("id", "DESC");
    qb->limit((int32_t)size);
    qb->offset((int32_t)offset);
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
        results.push_back(parseRow(rt));
    }
}

int64_t UserFollowRelManager::countFollowing(int64_t follower_id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("user_follow_rel");
    qb->where("follower_id", "=", follower_id);
    qb->where("is_deleted", "=", (int64_t)0);
    int64_t total = 0;
    if (qb->executeCount(total, db)) {
        ERROR(logger) << "executeCount fail errno=" << db->getErrno();
        return 0;
    }
    return total;
}

int64_t UserFollowRelManager::countFollowers(int64_t following_id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("user_follow_rel");
    qb->where("following_id", "=", following_id);
    qb->where("is_deleted", "=", (int64_t)0);
    int64_t total = 0;
    if (qb->executeCount(total, db)) {
        ERROR(logger) << "executeCount fail errno=" << db->getErrno();
        return 0;
    }
    return total;
}

}
