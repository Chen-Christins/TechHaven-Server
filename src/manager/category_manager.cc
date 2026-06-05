#include "category_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 100;

CategoryManager::CategoryManager()
    :m_cache(kCacheMaxSize, 0, nullptr) {
}

data::CategoryInfo::ptr CategoryManager::parseRow(chen::ISQLData::ptr rt) {
    data::CategoryInfo::ptr v(new data::CategoryInfo);
    v->setId(rt->getInt64(0));
    v->setName(rt->getString(1));
    v->setColor(rt->getString(2));
    v->setDescription(rt->getString(3));
    v->setUrl(rt->getString(4));
    v->setIcon(rt->getString(5));
    v->setParentId(rt->getInt64(6));
    v->setStatus(rt->getInt32(7));
    v->setIsDeleted(rt->getInt32(8));
    v->setCreateTime(rt->getTime(9));
    v->setUpdateTime(rt->getTime(10));
    return v;
}


void CategoryManager::add(blog::data::CategoryInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

blog::data::CategoryInfo::ptr CategoryManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::CategoryInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

void CategoryManager::listAll(std::vector<blog::data::CategoryInfo::ptr>& infos, bool isValid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return;
    }
    auto qb = chen::QueryBuilder::Create("category");
    qb->whereIf(isValid, "is_deleted", "=", (int64_t)0);
    qb->orderBy("id", "ASC");
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
        infos.push_back(parseRow(rt));
    }
}

blog::data::CategoryInfo::ptr CategoryManager::getByName(const std::string& name) {
    // 先扫缓存
    // LRU 缓存只支持按 id 查找，所以直接用 DAO 查 DB
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    return data::CategoryInfoDao::QueryByName(name, db);
}

}
