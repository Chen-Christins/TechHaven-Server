#include "organization_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 200;

OrganizationManager::OrganizationManager()
    :m_cache(kCacheMaxSize, 0, nullptr) {
}

data::OrganizationInfo::ptr OrganizationManager::parseRow(chen::ISQLData::ptr rt) {
    data::OrganizationInfo::ptr v(new data::OrganizationInfo);
    v->setId(rt->getInt64(0));
    v->setName(rt->getString(1));
    v->setType(rt->getString(2));
    v->setDescription(rt->getString(3));
    v->setOwnerId(rt->getInt64(4));
    v->setStatus(rt->getInt32(5));
    v->setIsDeleted(rt->getInt32(6));
    v->setCreateTime(rt->getTime(7));
    v->setUpdateTime(rt->getTime(8));
    return v;
}

bool OrganizationManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }
    INFO(logger) << "OrganizationManager loadAll: DB connection verified, no preloading needed";
    return true;
}

void OrganizationManager::add(data::OrganizationInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

data::OrganizationInfo::ptr OrganizationManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::OrganizationInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

data::OrganizationInfo::ptr OrganizationManager::getByName(const std::string& name) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    return data::OrganizationInfoDao::QueryByName(name, db);
}

int64_t OrganizationManager::listByPages(std::vector<data::OrganizationInfo::ptr>& orgs
        , uint64_t offset, uint64_t limit, int32_t status, bool isValid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("organization");
    qb->whereIf(status != -1, "status", "=", (int64_t)status);
    qb->whereIf(isValid, "is_deleted", "=", (int64_t)0);
    qb->orderBy("id", "DESC");

    int64_t total = 0;
    if (qb->executeCount(total, db)) {
        ERROR(logger) << "listByPages executeCount fail errno=" << db->getErrno();
        return 0;
    }

    if (limit < (uint64_t)INT32_MAX) {
        qb->limit((int32_t)limit);
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
        orgs.push_back(parseRow(rt));
    }
    return total;
}

OrganizationManager::OrganizationStats OrganizationManager::getStats() {
    OrganizationStats stats;
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return stats;
    }

    // Query total (non-deleted)
    {
        auto qb = chen::QueryBuilder::Create("organization");
        qb->where("is_deleted", "=", (int64_t)0);
        int64_t total = 0;
        if (qb->executeCount(total, db) == 0) {
            stats.total = total;
        }
    }

    // Query active
    {
        auto qb = chen::QueryBuilder::Create("organization");
        qb->where("is_deleted", "=", (int64_t)0);
        qb->where("status", "=", (int64_t)Status::ACTIVE);
        int64_t active = 0;
        if (qb->executeCount(active, db) == 0) {
            stats.active = active;
        }
    }

    // Query inactive
    {
        auto qb = chen::QueryBuilder::Create("organization");
        qb->where("is_deleted", "=", (int64_t)0);
        qb->where("status", "=", (int64_t)Status::INACTIVE);
        int64_t inactive = 0;
        if (qb->executeCount(inactive, db) == 0) {
            stats.inactive = inactive;
        }
    }

    return stats;
}

}
