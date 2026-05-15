#include "comment_praise_rel_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

bool CommentPraiseRelManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "get db connection fail";
        return false;
    }
    std::vector<data::CommentPraiseRelInfo::ptr> results;
    if (data::CommentPraiseRelInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "CommentPraiseRelManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, data::CommentPraiseRelInfo::ptr> datas;
    std::unordered_map<int64_t, std::map<int64_t, data::CommentPraiseRelInfo::ptr>> userPraises;
    std::unordered_map<int64_t, std::map<int64_t, data::CommentPraiseRelInfo::ptr>> commentPraises;
    for (auto& i : results) {
        datas[i->getId()] = i;
        userPraises[i->getUserId()][i->getCommentId()] = i;
        commentPraises[i->getCommentId()][i->getUserId()] = i;
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_userPraises.swap(userPraises);
    m_commentPraises.swap(commentPraises);

    return true;
}

void CommentPraiseRelManager::add(data::CommentPraiseRelInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
    m_userPraises[info->getUserId()][info->getCommentId()] = info;
    m_commentPraises[info->getCommentId()][info->getUserId()] = info;
}

data::CommentPraiseRelInfo::ptr CommentPraiseRelManager::get(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    return it == m_datas.end() ? nullptr : it->second;
}

data::CommentPraiseRelInfo::ptr CommentPraiseRelManager::getByUserAndComment(
    int64_t user_id, int64_t comment_id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_userPraises.find(user_id);
    if (it != m_userPraises.end()) {
        auto iit = it->second.find(comment_id);
        return iit == it->second.end() ? nullptr : iit->second;
    }
    return nullptr;
}

data::CommentPraiseRelInfo::ptr CommentPraiseRelManager::praise(int64_t user_id, int64_t comment_id) {
    // check if already praising
    auto existing = getByUserAndComment(user_id, comment_id);
    if (existing) {
        if (!existing->getIsDeleted()) {
            return existing; // already praising
        }
        // re-praise: update existing record
        auto db = GetDB();
        if (!db) {
            ERROR(logger) << "get db connection fail";
            return nullptr;
        }
        existing->setIsDeleted(0);
        existing->setUpdateTime(time(0));
        if (data::CommentPraiseRelInfoDao::Update(existing, db)) {
            ERROR(logger) << "CommentPraiseRelManager praise Update fail";
            return nullptr;
        }
        // update in-memory maps
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        existing->setIsDeleted(0);
        return existing;
    }

    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "get db connection fail";
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

    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_datas[info->getId()] = info;
        m_userPraises[info->getUserId()][info->getCommentId()] = info;
        m_commentPraises[info->getCommentId()][info->getUserId()] = info;
    }

    return info;
}

bool CommentPraiseRelManager::unpraise(int64_t user_id, int64_t comment_id) {
    auto info = getByUserAndComment(user_id, comment_id);
    if (!info || info->getIsDeleted()) {
        return false;
    }

    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "get db connection fail";
        return false;
    }

    info->setIsDeleted(1);
    info->setUpdateTime(time(0));
    if (data::CommentPraiseRelInfoDao::Update(info, db)) {
        ERROR(logger) << "CommentPraiseRelManager unpraise Update fail";
        return false;
    }
    return true;
}

bool CommentPraiseRelManager::isPraising(int64_t user_id, int64_t comment_id) {
    auto info = getByUserAndComment(user_id, comment_id);
    return info && !info->getIsDeleted();
}

int64_t CommentPraiseRelManager::countByComment(int64_t comment_id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_commentPraises.find(comment_id);
    if (it == m_commentPraises.end()) {
        return 0;
    }
    int64_t count = 0;
    for (auto& [id, info] : it->second) {
        if (!info->getIsDeleted()) {
            count++;
        }
    }
    return count;
}

}
