#include "label_manager.h"

#include "cache_util.h"
#include "../util.h"

#include <chen/log/log.h>

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 500;

LabelManager::LabelManager()
    :m_cache(8, kCacheMaxSize, 50) {
}

data::LabelInfo::ptr LabelManager::parseRow(chen::ISQLData::ptr rt) {
    return data::LabelInfoDao::ParseRow(rt);
}


void LabelManager::add(data::LabelInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

data::LabelInfo::ptr LabelManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::LabelInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

data::LabelInfo::ptr LabelManager::getByUserIdName(int64_t id, const std::string& name) {
    int64_t cachedId = getCachedIdMapping("lbl:uid_name:" + std::to_string(id) + ":" + name);
    if (cachedId > 0) {
        return get(cachedId);
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    auto info = data::LabelInfoDao::QueryByUserIdName(id, name, db);
    if (info) {
        m_cache.set(info->getId(), info);
        cacheIdMapping("lbl:uid_name:" + std::to_string(id) + ":" + name, info->getId());
    }
    return info;
}

bool LabelManager::listByUserId(std::vector<data::LabelInfo::ptr>& infos, int64_t id, bool valid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }
    auto qb = chen::QueryBuilder::Create("label");
    qb->where("user_id", "=", id);
    qb->whereIf(valid, "is_deleted", "=", (int64_t)0);
    qb->orderBy("id", "DESC");
    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return false;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (!rt) {
        return false;
    }
    while (rt->next()) {
        auto info = parseRow(rt);
        infos.push_back(info);
        if (!m_cache.exists(info->getId())) {
            m_cache.set(info->getId(), info);
        }
    }
    return true;
}

} // namespace blog
