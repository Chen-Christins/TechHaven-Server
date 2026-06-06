#include "article_label_rel_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 500;

ArticleLabelRelManager::ArticleLabelRelManager()
    :m_cache(8, kCacheMaxSize, 0) {
}

data::ArticleLabelRelInfo::ptr ArticleLabelRelManager::parseRow(chen::ISQLData::ptr rt) {
    data::ArticleLabelRelInfo::ptr v(new data::ArticleLabelRelInfo);
    v->setId(rt->getInt64(0));
    v->setArticleId(rt->getInt64(1));
    v->setLabelId(rt->getInt64(2));
    v->setIsDeleted(rt->getInt32(3));
    v->setCreateTime(rt->getTime(4));
    v->setUpdateTime(rt->getTime(5));
    return v;
}


void ArticleLabelRelManager::add(data::ArticleLabelRelInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

data::ArticleLabelRelInfo::ptr ArticleLabelRelManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::ArticleLabelRelInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

bool ArticleLabelRelManager::listByArticleId(std::vector<data::ArticleLabelRelInfo::ptr>& infos
        ,int64_t id, bool valid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }
    auto qb = chen::QueryBuilder::Create("article_label_rel");
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

bool ArticleLabelRelManager::listByLabelId(std::vector<data::ArticleLabelRelInfo::ptr>& infos
        ,int64_t label_id, bool valid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }
    auto qb = chen::QueryBuilder::Create("article_label_rel");
    qb->where("label_id", "=", label_id);
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

data::ArticleLabelRelInfo::ptr ArticleLabelRelManager::getByArticleIdLabelId(int64_t article_id
        ,int64_t label_id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    auto info = data::ArticleLabelRelInfoDao::QueryByArticleIdLabelId(article_id, label_id, db);
    if (info) {
        m_cache.set(info->getId(), info);
    }
    return info;
}

}
