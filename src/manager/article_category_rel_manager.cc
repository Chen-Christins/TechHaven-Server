#include "article_category_rel_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

bool ArticleCategoryRelManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "get db connection fail";
        return false;
    }
    std::vector<data::ArticleCategoryRelInfo::ptr> results;
    if (data::ArticleCategoryRelInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "ArticleCategoryManager loadAll fail";
        return false;
    }
    
    std::unordered_map<int64_t, data::ArticleCategoryRelInfo::ptr> datas;
    std::unordered_map<int64_t, std::map<int64_t, data::ArticleCategoryRelInfo::ptr>> articles;
    for (auto& i : results) {
        datas[i->getId()] = i;
        articles[i->getArticleId()][i->getCategoryId()] = i;
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_articles.swap(articles);
    
    return true;
}

void ArticleCategoryRelManager::add(data::ArticleCategoryRelInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
    m_articles[info->getArticleId()][info->getCategoryId()] = info;
}

data::ArticleCategoryRelInfo::ptr ArticleCategoryRelManager::get(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    return it == m_datas.end() ? nullptr : it->second;
}

bool ArticleCategoryRelManager::listByArticleId(std::vector<data::ArticleCategoryRelInfo::ptr>& infos
        ,int64_t id, bool valid) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_articles.find(id);
    if (it == m_articles.end()) {
        return false;
    }

    for (auto& i : it->second) {
        if (!valid || !i.second->getIsDeleted()) {
            infos.push_back(i.second);
        }
    }
    return true;
}

data::ArticleCategoryRelInfo::ptr ArticleCategoryRelManager::getByArticleIdCategoryId(int64_t article_id
        ,int64_t category_id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_articles.find(article_id);
    if (it != m_articles.end()) {
        auto iit = it->second.find(category_id);
        return iit == it->second.end() ? nullptr : iit->second;
    }
    return nullptr;
}

}