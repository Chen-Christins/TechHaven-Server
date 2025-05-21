#include "article_label_rel_manager.h"
#include "chen/log/log.h"
#include "../util.h"

namespace blog {

static sylar::Logger::ptr logger = LOG_ROOT();

bool ArticleLabelRelManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "get db connection fail";
        return false;
    }
    std::vector<data::ArticleLabelRelInfo::ptr> results;
    if (data::ArticleLabelRelInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "ArticleLabelManager loadAll fail";
        return false;
    }
    
    std::unordered_map<int64_t, data::ArticleLabelRelInfo::ptr> datas;
    std::unordered_map<int64_t, std::map<int64_t, data::ArticleLabelRelInfo::ptr>> articles;
    for (auto& i : results) {
        datas[i->getId()] = i;
        articles[i->getArticleId()][i->getLabelId()] = i;
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_articles.swap(articles);
    
    return true;
}

void ArticleLabelRelManager::add(data::ArticleLabelRelInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
    m_articles[info->getArticleId()][info->getLabelId()] = info;
}

data::ArticleLabelRelInfo::ptr ArticleLabelRelManager::get(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    return it == m_datas.end() ? nullptr : it->second;
}

bool ArticleLabelRelManager::listByArticleId(std::vector<data::ArticleLabelRelInfo::ptr>& infos
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

data::ArticleLabelRelInfo::ptr ArticleLabelRelManager::getByArticleIdLabelId(int64_t article_id
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