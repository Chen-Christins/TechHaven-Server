#include "article_praise_rel_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

bool ArticlePraiseRelManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "get db connection fail";
        return false;
    }
    std::vector<data::ArticlePraiseRelInfo::ptr> results;
    if (data::ArticlePraiseRelInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "ArticlePraiseRelManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, data::ArticlePraiseRelInfo::ptr> datas;
    std::unordered_map<int64_t, std::map<int64_t, data::ArticlePraiseRelInfo::ptr>> userPraises;
    std::unordered_map<int64_t, std::map<int64_t, data::ArticlePraiseRelInfo::ptr>> articlePraises;
    for (auto& i : results) {
        datas[i->getId()] = i;
        userPraises[i->getUserId()][i->getArticleId()] = i;
        articlePraises[i->getArticleId()][i->getUserId()] = i;
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_userPraises.swap(userPraises);
    m_articlePraises.swap(articlePraises);

    return true;
}

void ArticlePraiseRelManager::add(data::ArticlePraiseRelInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
    m_userPraises[info->getUserId()][info->getArticleId()] = info;
    m_articlePraises[info->getArticleId()][info->getUserId()] = info;
}

data::ArticlePraiseRelInfo::ptr ArticlePraiseRelManager::get(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    return it == m_datas.end() ? nullptr : it->second;
}

data::ArticlePraiseRelInfo::ptr ArticlePraiseRelManager::getByUserAndArticle(
    int64_t user_id, int64_t article_id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_userPraises.find(user_id);
    if (it != m_userPraises.end()) {
        auto iit = it->second.find(article_id);
        return iit == it->second.end() ? nullptr : iit->second;
    }
    return nullptr;
}

data::ArticlePraiseRelInfo::ptr ArticlePraiseRelManager::praise(int64_t user_id, int64_t article_id) {
    // check if already praising
    auto existing = getByUserAndArticle(user_id, article_id);
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
        if (data::ArticlePraiseRelInfoDao::Update(existing, db)) {
            ERROR(logger) << "ArticlePraiseRelManager praise Update fail";
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

    auto info = std::make_shared<data::ArticlePraiseRelInfo>();
    info->setUserId(user_id);
    info->setArticleId(article_id);
    info->setIsDeleted(0);
    info->setCreateTime(time(0));
    info->setUpdateTime(time(0));

    if (data::ArticlePraiseRelInfoDao::Insert(info, db)) {
        ERROR(logger) << "ArticlePraiseRelManager praise Insert fail";
        return nullptr;
    }

    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_datas[info->getId()] = info;
        m_userPraises[info->getUserId()][info->getArticleId()] = info;
        m_articlePraises[info->getArticleId()][info->getUserId()] = info;
    }

    return info;
}

bool ArticlePraiseRelManager::unpraise(int64_t user_id, int64_t article_id) {
    auto info = getByUserAndArticle(user_id, article_id);
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
    if (data::ArticlePraiseRelInfoDao::Update(info, db)) {
        ERROR(logger) << "ArticlePraiseRelManager unpraise Update fail";
        return false;
    }
    return true;
}

bool ArticlePraiseRelManager::isPraising(int64_t user_id, int64_t article_id) {
    auto info = getByUserAndArticle(user_id, article_id);
    return info && !info->getIsDeleted();
}

void ArticlePraiseRelManager::listByArticle(std::vector<data::ArticlePraiseRelInfo::ptr>& results,
    int64_t article_id, uint64_t offset, uint64_t size) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_articlePraises.find(article_id);
    if (it == m_articlePraises.end()) {
        return;
    }
    auto& praiseMap = it->second;
    uint64_t idx = 0;
    for (auto rit = praiseMap.rbegin(); rit != praiseMap.rend(); ++rit) {
        if (rit->second->getIsDeleted()) {
            continue;
        }
        if (idx >= offset && results.size() < size) {
            results.push_back(rit->second);
        }
        idx++;
        if (results.size() >= size) {
            break;
        }
    }
}

void ArticlePraiseRelManager::listByUser(std::vector<data::ArticlePraiseRelInfo::ptr>& results,
    int64_t user_id, uint64_t offset, uint64_t size) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_userPraises.find(user_id);
    if (it == m_userPraises.end()) {
        return;
    }
    auto& praiseMap = it->second;
    uint64_t idx = 0;
    for (auto rit = praiseMap.rbegin(); rit != praiseMap.rend(); ++rit) {
        if (rit->second->getIsDeleted()) {
            continue;
        }
        if (idx >= offset && results.size() < size) {
            results.push_back(rit->second);
        }
        idx++;
        if (results.size() >= size) {
            break;
        }
    }
}

int64_t ArticlePraiseRelManager::countByArticle(int64_t article_id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_articlePraises.find(article_id);
    if (it == m_articlePraises.end()) {
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

int64_t ArticlePraiseRelManager::countByUser(int64_t user_id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_userPraises.find(user_id);
    if (it == m_userPraises.end()) {
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
