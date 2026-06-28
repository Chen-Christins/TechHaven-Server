#include "assignment_user_rel_manager.h"

#include <chen/log/log.h>

#include "cache_util.h"
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 500;

AssignmentUserRelManager::AssignmentUserRelManager()
    :m_cache(8, kCacheMaxSize, 0) {
}

data::AssignmentUserRelInfo::ptr AssignmentUserRelManager::parseRow(chen::ISQLData::ptr rt) {
    data::AssignmentUserRelInfo::ptr v(new data::AssignmentUserRelInfo);
    v->setId(rt->getInt64(0));
    v->setAssignmentId(rt->getInt64(1));
    v->setUserId(rt->getInt64(2));
    v->setStatus(rt->getInt32(3));
    v->setScore(rt->getInt32(4));
    v->setSubmitTime(rt->getInt64(5));
    v->setIsDeleted(rt->getInt32(6));
    v->setCreateTime(rt->getTime(7));
    v->setUpdateTime(rt->getTime(8));
    return v;
}

void AssignmentUserRelManager::add(blog::data::AssignmentUserRelInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

blog::data::AssignmentUserRelInfo::ptr AssignmentUserRelManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::AssignmentUserRelInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

blog::data::AssignmentUserRelInfo::ptr AssignmentUserRelManager::getByAssignAndUser(int64_t assign_id, int64_t user_id) {
    int64_t cachedId = getCachedIdMapping("assign_usr:" + std::to_string(assign_id) + ":" + std::to_string(user_id));
    if (cachedId > 0) {
        return get(cachedId);
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    auto info = data::AssignmentUserRelInfoDao::QueryByAssignmentIdUserId(assign_id, user_id, db);
    if (info) {
        m_cache.set(info->getId(), info);
        cacheIdMapping("assign_usr:" + std::to_string(assign_id) + ":" + std::to_string(user_id), info->getId());
    }
    return info;
}

}
