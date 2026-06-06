#include "article_category_rel_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 500;

ArticleCategoryRelManager::ArticleCategoryRelManager()
    :m_cache(8, kCacheMaxSize, 0) {
}

data::ArticleCategoryRelInfo::ptr ArticleCategoryRelManager::parseRow(chen::ISQLData::ptr rt) {
    data::ArticleCategoryRelInfo::ptr v(new data::ArticleCategoryRelInfo);
    v->setId(rt->getInt64(0));
    v->setArticleId(rt->getInt64(1));
    v->setCategoryId(rt->getInt64(2));
    v->setIsDeleted(rt->getInt32(3));
    v->setPublishTime(rt->getTime(4));
    v->setCreateTime(rt->getTime(5));
    v->setUpdateTime(rt->getTime(6));
    return v;
}


void ArticleCategoryRelManager::add(data::ArticleCategoryRelInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

data::ArticleCategoryRelInfo::ptr ArticleCategoryRelManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::ArticleCategoryRelInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

bool ArticleCategoryRelManager::listByArticleId(std::vector<data::ArticleCategoryRelInfo::ptr>& infos
        ,int64_t id, bool valid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }
    auto qb = chen::QueryBuilder::Create("article_category_rel");
    qb->where("article_id", "=", id);
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
        auto info = parseRow(rt);
        infos.push_back(info);
        m_cache.set(info->getId(), info);
    }
    return true;
}

bool ArticleCategoryRelManager::listByCategoryId(std::vector<data::ArticleCategoryRelInfo::ptr>& infos
        ,int64_t category_id, bool valid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }
    auto qb = chen::QueryBuilder::Create("article_category_rel");
    qb->where("category_id", "=", category_id);
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
        auto info = parseRow(rt);
        infos.push_back(info);
        m_cache.set(info->getId(), info);
    }
    return true;
}

data::ArticleCategoryRelInfo::ptr ArticleCategoryRelManager::getByArticleIdCategoryId(int64_t article_id
        ,int64_t category_id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    auto info = data::ArticleCategoryRelInfoDao::QueryByArticleIdCategoryId(article_id, category_id, db);
    if (info) {
        m_cache.set(info->getId(), info);
    }
    return info;
}

}
