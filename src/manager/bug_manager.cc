#include "bug_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 500;

BugManager::BugManager()
    :m_cache(8, kCacheMaxSize, 0) {
}

data::BugInfo::ptr BugManager::parseRow(chen::ISQLData::ptr rt) {
    data::BugInfo::ptr v(new data::BugInfo);
    v->setId(rt->getInt64(0));
    v->setOrgId(rt->getInt64(1));
    v->setTitle(rt->getString(2));
    v->setDescription(rt->getString(3));
    v->setSeverity(rt->getInt32(4));
    v->setPriority(rt->getInt32(5));
    v->setStatus(rt->getInt32(6));
    v->setCreatorId(rt->getInt64(7));
    v->setAssigneeId(rt->getInt64(8));
    v->setRequirementId(rt->getInt64(9));
    v->setModule(rt->getString(10));
    v->setStepsToReproduce(rt->getString(11));
    v->setEnvironment(rt->getString(12));
    v->setIsDeleted(rt->getInt32(13));
    v->setCreateTime(rt->getTime(14));
    v->setUpdateTime(rt->getTime(15));
    return v;
}


void BugManager::add(data::BugInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

data::BugInfo::ptr BugManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::BugInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

uint64_t BugManager::listByPages(std::vector<data::BugInfo::ptr>& infos,
        uint64_t offset, uint64_t size, int32_t status, bool isValid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("bug");
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
        m_cache.set(info->getId(), info);
    }
    return total;
}

uint64_t BugManager::listByOrg(std::vector<data::BugInfo::ptr>& infos,
        int64_t orgId, uint64_t offset, uint64_t size, int32_t status, bool isValid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("bug");
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
        m_cache.set(info->getId(), info);
    }
    return total;
}

}
