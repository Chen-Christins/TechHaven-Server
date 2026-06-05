#include "organization_user_rel_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 1000;

OrganizationUserRelManager::OrganizationUserRelManager()
    :m_cache(8, kCacheMaxSize, 0) {
}

data::OrganizationUserRelInfo::ptr OrganizationUserRelManager::parseRow(chen::ISQLData::ptr rt) {
    data::OrganizationUserRelInfo::ptr v(new data::OrganizationUserRelInfo);
    v->setId(rt->getInt64(0));
    v->setOrgId(rt->getInt64(1));
    v->setUserId(rt->getInt64(2));
    v->setRole(rt->getInt32(3));
    v->setStatus(rt->getInt32(4));
    v->setIsDeleted(rt->getInt32(5));
    v->setCreateTime(rt->getTime(6));
    v->setUpdateTime(rt->getTime(7));
    return v;
}


void OrganizationUserRelManager::add(data::OrganizationUserRelInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

data::OrganizationUserRelInfo::ptr OrganizationUserRelManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::OrganizationUserRelInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

data::OrganizationUserRelInfo::ptr OrganizationUserRelManager::getByOrgAndUser(int64_t o_id, int64_t u_id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    return data::OrganizationUserRelInfoDao::QueryByOrgIdUserId(o_id, u_id, db);
}

int64_t OrganizationUserRelManager::getByPages(std::vector<data::OrganizationUserRelInfo::ptr>& results
        , int64_t o_id, uint64_t offset, uint64_t size, int32_t status, bool isValid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("organization_user_rel");
    qb->where("org_id", "=", o_id);
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
        results.push_back(parseRow(rt));
    }
    return total;
}

int64_t OrganizationUserRelManager::getOrgByUserId(std::vector<data::OrganizationUserRelInfo::ptr>& results
        , int64_t u_id, int32_t status, bool isValid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("organization_user_rel");
    qb->where("user_id", "=", u_id);
    qb->where("status", "!=", (int64_t)Status::PENDING);
    qb->whereIf(status != -1, "status", "=", (int64_t)status);
    qb->whereIf(isValid, "is_deleted", "=", (int64_t)0);
    qb->orderBy("id", "DESC");
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
        results.push_back(parseRow(rt));
    }
    return results.size();
}

int64_t OrganizationUserRelManager::getMemberCount(int64_t o_id, int32_t status, bool isValid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("organization_user_rel");
    qb->where("org_id", "=", o_id);
    qb->whereIf(status != -1, "status", "=", (int64_t)status);
    qb->whereIf(isValid, "is_deleted", "=", (int64_t)0);
    int64_t total = 0;
    if (qb->executeCount(total, db)) {
        ERROR(logger) << "executeCount fail errno=" << db->getErrno();
        return 0;
    }
    return total;
}

}
