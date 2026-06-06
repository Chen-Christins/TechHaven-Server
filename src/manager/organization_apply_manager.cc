#include "organization_apply_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 500;

OrganizationApplyManager::OrganizationApplyManager()
    :m_cache(8, kCacheMaxSize, 0) {
}

data::OrganizationApplyInfo::ptr OrganizationApplyManager::parseRow(chen::ISQLData::ptr rt) {
    data::OrganizationApplyInfo::ptr v(new data::OrganizationApplyInfo);
    v->setId(rt->getInt64(0));
    v->setUserId(rt->getInt64(1));
    v->setOrgName(rt->getString(2));
    v->setOrgType(rt->getString(3));
    v->setOrgDescription(rt->getString(4));
    v->setStatus(rt->getInt32(5));
    v->setReviewReason(rt->getString(6));
    v->setCreatedAt(rt->getInt64(7));
    v->setReviewedAt(rt->getInt64(8));
    v->setIsDeleted(rt->getInt32(9));
    return v;
}


void OrganizationApplyManager::add(data::OrganizationApplyInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

void OrganizationApplyManager::update(data::OrganizationApplyInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

data::OrganizationApplyInfo::ptr OrganizationApplyManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::OrganizationApplyInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

int64_t OrganizationApplyManager::listByPages(std::vector<data::OrganizationApplyInfo::ptr>& results
        , uint64_t offset, uint64_t limit, int32_t status, bool isValid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("organization_apply");
    qb->whereIf(status != -1, "status", "=", (int64_t)status);
    qb->whereIf(isValid, "is_deleted", "=", (int64_t)0);
    qb->orderBy("created_at", "DESC");

    int64_t total = 0;
    if (qb->executeCount(total, db)) {
        ERROR(logger) << "executeCount fail errno=" << db->getErrno();
        return 0;
    }

    qb->limit((int32_t)limit);
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
        results.push_back(info);
        m_cache.set(info->getId(), info);
    }
    return total;
}

int64_t OrganizationApplyManager::listByUserId(std::vector<data::OrganizationApplyInfo::ptr>& results
        , int64_t user_id, uint64_t offset, uint64_t limit, bool isValid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("organization_apply");
    qb->where("user_id", "=", user_id);
    qb->whereIf(isValid, "is_deleted", "=", (int64_t)0);
    qb->orderBy("created_at", "DESC");

    int64_t total = 0;
    if (qb->executeCount(total, db)) {
        ERROR(logger) << "executeCount fail errno=" << db->getErrno();
        return 0;
    }

    qb->limit((int32_t)limit);
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
        results.push_back(info);
        m_cache.set(info->getId(), info);
    }
    return total;
}

}
