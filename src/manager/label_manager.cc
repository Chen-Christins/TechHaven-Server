#include "label_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 500;

LabelManager::LabelManager()
    :m_cache(8, kCacheMaxSize, 0) {
}

data::LabelInfo::ptr LabelManager::parseRow(chen::ISQLData::ptr rt) {
    data::LabelInfo::ptr v(new data::LabelInfo);
    v->setId(rt->getInt64(0));
    v->setUserId(rt->getInt64(1));
    v->setName(rt->getString(2));
    v->setColor(rt->getString(3));
    v->setDescription(rt->getString(4));
    v->setIsDeleted(rt->getInt32(5));
    v->setCreateTime(rt->getTime(6));
    v->setUpdateTime(rt->getTime(7));
    return v;
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
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    return data::LabelInfoDao::QueryByUserIdName(id, name, db);
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
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return false;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (!rt) {
        return false;
    }
    while (rt->next()) {
        infos.push_back(parseRow(rt));
    }
    return true;
}

}
