#include "comment_praise_rel_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 1000;

CommentPraiseRelManager::CommentPraiseRelManager()
    :m_cache(kCacheMaxSize, 0, nullptr) {
}

data::CommentPraiseRelInfo::ptr CommentPraiseRelManager::parseRow(chen::ISQLData::ptr rt) {
    data::CommentPraiseRelInfo::ptr v(new data::CommentPraiseRelInfo);
    v->setId(rt->getInt64(0));
    v->setUserId(rt->getInt64(1));
    v->setCommentId(rt->getInt64(2));
    v->setIsDeleted(rt->getInt32(3));
    v->setCreateTime(rt->getTime(4));
    v->setUpdateTime(rt->getTime(5));
    return v;
}

bool CommentPraiseRelManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }
    INFO(logger) << "CommentPraiseRelManager loadAll: DB connection verified, no preloading needed";
    return true;
}

void CommentPraiseRelManager::add(data::CommentPraiseRelInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

data::CommentPraiseRelInfo::ptr CommentPraiseRelManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::CommentPraiseRelInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

data::CommentPraiseRelInfo::ptr CommentPraiseRelManager::getByUserAndComment(
    int64_t user_id, int64_t comment_id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    return data::CommentPraiseRelInfoDao::QueryByUserIdCommentId(user_id, comment_id, db);
}

data::CommentPraiseRelInfo::ptr CommentPraiseRelManager::praise(int64_t user_id, int64_t comment_id) {
    auto existing = getByUserAndComment(user_id, comment_id);
    if (existing) {
        if (!existing->getIsDeleted()) {
            return existing;
        }
        auto db = GetDB();
        if (!db) {
            ERROR(logger) << "Get DB connection fail";
            return nullptr;
        }
        existing->setIsDeleted(0);
        existing->setUpdateTime(time(0));
        if (data::CommentPraiseRelInfoDao::Update(existing, db)) {
            ERROR(logger) << "CommentPraiseRelManager praise Update fail";
            return nullptr;
        }
        m_cache.set(existing->getId(), existing);
        return existing;
    }

    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }

    auto info = std::make_shared<data::CommentPraiseRelInfo>();
    info->setUserId(user_id);
    info->setCommentId(comment_id);
    info->setIsDeleted(0);
    info->setCreateTime(time(0));
    info->setUpdateTime(time(0));

    if (data::CommentPraiseRelInfoDao::Insert(info, db)) {
        ERROR(logger) << "CommentPraiseRelManager praise Insert fail";
        return nullptr;
    }

    m_cache.set(info->getId(), info);
    return info;
}

bool CommentPraiseRelManager::unpraise(int64_t user_id, int64_t comment_id) {
    auto info = getByUserAndComment(user_id, comment_id);
    if (!info || info->getIsDeleted()) {
        return false;
    }

    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }

    info->setIsDeleted(1);
    info->setUpdateTime(time(0));
    if (data::CommentPraiseRelInfoDao::Update(info, db)) {
        ERROR(logger) << "CommentPraiseRelManager unpraise Update fail";
        return false;
    }
    m_cache.set(info->getId(), info);
    return true;
}

bool CommentPraiseRelManager::isPraising(int64_t user_id, int64_t comment_id) {
    auto info = getByUserAndComment(user_id, comment_id);
    return info && !info->getIsDeleted();
}

int64_t CommentPraiseRelManager::countByComment(int64_t comment_id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("comment_praise_rel");
    qb->where("comment_id", "=", comment_id);
    qb->where("is_deleted", "=", (int64_t)0);
    int64_t total = 0;
    if (qb->executeCount(total, db)) {
        ERROR(logger) << "executeCount fail errno=" << db->getErrno();
        return 0;
    }
    return total;
}

}
