#include "assignment_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 200;

AssignmentManager::AssignmentManager()
    :m_cache(4, kCacheMaxSize, 0) {
}

data::AssignmentInfo::ptr AssignmentManager::parseRow(chen::ISQLData::ptr rt) {
    data::AssignmentInfo::ptr v(new data::AssignmentInfo);
    v->setId(rt->getInt64(0));
    v->setName(rt->getString(1));
    v->setSubjectName(rt->getString(2));
    v->setPriority(rt->getInt32(3));
    v->setStatus(rt->getInt32(4));
    v->setDescription(rt->getString(5));
    v->setMaxSize(rt->getInt32(6));
    v->setFileType(rt->getString(7));
    v->setDeadline(rt->getTime(8));
    v->setIsDeleted(rt->getInt32(9));
    v->setCreateTime(rt->getTime(10));
    v->setUpdateTime(rt->getTime(11));
    return v;
}


void AssignmentManager::add(data::AssignmentInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

data::AssignmentInfo::ptr AssignmentManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::AssignmentInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

data::AssignmentInfo::ptr AssignmentManager::getByName(const std::string& subject_name, const std::string& name) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    std::vector<data::AssignmentInfo::ptr> results;
    if (data::AssignmentInfoDao::QueryBySubjectNameName(results, subject_name, name, db)) {
        return nullptr;
    }
    if (!results.empty()) {
        m_cache.set(results[0]->getId(), results[0]);
        return results[0];
    }
    return nullptr;
}

uint64_t AssignmentManager::listByPages(std::vector<data::AssignmentInfo::ptr>& infos
        , uint64_t offset, uint64_t size, int32_t status, bool isValid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("assignment");
    qb->whereIf(status != -1, "status", "=", (int64_t)status);
    qb->whereIf(isValid, "is_deleted", "=", (int64_t)0);
    qb->orderBy("id", "DESC");

    int64_t total = 0;
    if (qb->executeCount(total, db)) {
        ERROR(logger) << "listByPages executeCount fail errno=" << db->getErrno();
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

AssignmentManager::AssignmentStats AssignmentManager::getStats() {
    AssignmentStats stats;
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return stats;
    }

    // Query status counts with GROUP BY
    {
        auto qb = chen::QueryBuilder::Create("assignment");
        qb->select("status, COUNT(*) AS cnt");
        qb->where("is_deleted", "=", (int64_t)0);
        qb->groupBy("status");
        std::string sql = qb->buildQuerySQL();
        auto stmt = db->prepare(sql);
        if (stmt) {
            qb->bindParams(stmt);
            auto rt = stmt->query();
            if (rt) {
                while (rt->next()) {
                    int32_t s = rt->getInt32(0);
                    int64_t cnt = rt->getInt64(1);
                    stats.total += cnt;
                    switch (s) {
                    case Status::ACTIVE:
                        stats.active = cnt;
                        break;
                    case Status::INACTIVE:
                        stats.closed = cnt;
                        break;
                    case Status::DRAFT:
                        stats.draft = cnt;
                        break;
                    }
                }
            }
        }
    }

    return stats;
}

}
