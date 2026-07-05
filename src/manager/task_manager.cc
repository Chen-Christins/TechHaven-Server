#include "task_manager.h"

#include "../util.h"

#include <chen/log/log.h>

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 500;

TaskManager::TaskManager()
    :m_cache(8, kCacheMaxSize, 0) {
}

data::TaskInfo::ptr TaskManager::parseRow(chen::ISQLData::ptr rt) {
    return data::TaskInfoDao::ParseRow(rt);
}


void TaskManager::add(data::TaskInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

data::TaskInfo::ptr TaskManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::TaskInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

uint64_t TaskManager::listByPages(std::vector<data::TaskInfo::ptr>& infos,
        uint64_t offset, uint64_t size, int32_t status, bool isValid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("task");
    qb->whereIf(status != -1, "status", "=", (int64_t)status);
    qb->whereIf(isValid, "is_deleted", "=", (int64_t)0);
    qb->orderBy("id", "DESC");

    int64_t total = 0;
    if (qb->executeCount(total, db)) {
        ERROR(logger) << "executeCount fail errno=" << db->getErrno();
        return 0;
    }

    if (size < (uint64_t)INT32_MAX) {
        qb->limit((int32_t)size);
        qb->offset((int32_t)offset);
    }
    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
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

uint64_t TaskManager::listByOrg(std::vector<data::TaskInfo::ptr>& infos,
        int64_t orgId, uint64_t offset, uint64_t size, int32_t status, bool isValid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("task");
    qb->where("org_id", "=", orgId);
    qb->whereIf(status != -1, "status", "=", (int64_t)status);
    qb->whereIf(isValid, "is_deleted", "=", (int64_t)0);
    qb->orderBy("id", "DESC");

    int64_t total = 0;
    if (qb->executeCount(total, db)) {
        ERROR(logger) << "executeCount fail errno=" << db->getErrno();
        return 0;
    }

    if (size < (uint64_t)INT32_MAX) {
        qb->limit((int32_t)size);
        qb->offset((int32_t)offset);
    }
    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
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
}
