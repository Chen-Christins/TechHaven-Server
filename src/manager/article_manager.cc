#include "article_manager.h"
#include "../util.h"
#include "chen/log/log.h"

namespace blog {

static sylar::Logger::ptr logger = LOG_ROOT();

bool ArticleManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get Sqlite3 connection fail";
        return false;
    }
    std::vector<data::ArticleInfo::ptr> results;
    if (blog::data::ArticleInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "ArticleManager loadAll fail";
        return false;
    }

    std::map<int64_t, blog::data::ArticleInfo::ptr> datas;
    std::unordered_map<int64_t, std::map<int64_t, blog::data::ArticleInfo::ptr>> users;
    std::map<int64_t, blog::data::ArticleInfo::ptr> verifys;

    for (auto& i : results) {
        datas[i->getId()] = i;
        users[i->getUserId()][i->getId()] = i;
        if (i->getState() == 1) {
            verifys[i->getId()] = i;
        }
    }
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_users.swap(users);
    m_verifys.swap(verifys);
    return true;
}

void ArticleManager::add(blog::data::ArticleInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
    m_users[info->getUserId()][info->getId()] = info;
    if (info->getState() == 1 && info->getIsDeleted() == 0) {
        m_verifys[info->getId()] = info;
    }
}

#define XX(map, key)                                   \
    std::shared_lock<std::shared_mutex> lock(m_mutex); \
    auto it = map.find(key);                           \
    return it == map.end() ? nullptr : it->second;

blog::data::ArticleInfo::ptr ArticleManager::get(int64_t id) {
    XX(m_datas, id);
}

bool ArticleManager::listByUserId(std::vector<data::ArticleInfo::ptr>& infos, int64_t id, bool valid) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_users.find(id);
    if (it == m_users.end()) {
        return false;
    }
    for (auto& i : it->second) {
        if (!valid || !i.second->getIsDeleted()) {
            infos.push_back(i.second);
        }
    }
    return true;
}

int64_t ArticleManager::listByUserIdPages(std::vector<data::ArticleInfo::ptr>& infos, int64_t id
        ,int32_t offset, int32_t size, bool valid, int state) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    if (id == 0) {
        if (offset >= (int32_t)m_datas.size()) {
            return m_datas.size();
        }
        auto it = m_datas.rbegin();
        std::advance(it, offset);
        for (; (int32_t)infos.size() < size && it != m_datas.rend(); ++it) {
            if (!valid || !it->second->getIsDeleted()) {
                if (!state || it->second->getState() == state) {
                    infos.push_back(it->second);
                }
            }
        }
        return m_datas.size();
    } else {
        auto uit = m_users.find(id);
        if (uit == m_users.end()) {
            return 0;
        }
        if (offset >= (int32_t)uit->second.size()) {
            return uit->second.size();
        }
        auto it = uit->second.rbegin();
        std::advance(it, offset);
        for (; (int32_t)infos.size() < size && it != m_datas.rend(); ++it) {
            if (!valid || !it->second->getIsDeleted()) {
                if (!state || it->second->getState() == state) {
                    infos.push_back(it->second);
                }
            }
        }
        return uit->second.size();
    }
}

void ArticleManager::delVerify(int64_t id) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_verifys.erase(id);
}

void ArticleManager::addVerify(data::ArticleInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_verifys[info->getId()] = info;
}

int64_t ArticleManager::listVerifyPages(std::vector<data::ArticleInfo::ptr>& infos, int32_t offset, int32_t size) {
    return 0;
}

std::pair<data::ArticleInfo::ptr, data::ArticleInfo::ptr> ArticleManager::nearby(int64_t id) {
    return {};
}

std::string ArticleManager::statusString() {
    return "";
}

void ArticleManager::start() {
}

void ArticleManager::stop() {
}

bool ArticleManager::incViews(uint64_t id, const std::string& cookie_id, uint64_t user_id) {
    return true;
}

bool ArticleManager::incPraise(uint64_t id, const std::string& cookie_id, uint64_t user_id) {
    return true;
}

bool ArticleManager::incFavorites(uint64_t id, const std::string& cookie_id, uint64_t user_id) {
    return true;
}

bool ArticleManager::decPraise(uint64_t id, const std::string& cookie_id, uint64_t user_id) {
    return true;
}

bool ArticleManager::decFavorites(uint64_t id, const std::string& cookie_id, uint64_t user_id) {
    return true;
}

bool ArticleManager::listUserFav(int64_t id, std::map<int64_t, int64_t>& articles) {
    return true;
}

bool ArticleManager::listUserPra(int64_t id, std::map<int64_t, int64_t>& articles) {
    return true;
}

bool ArticleManager::listArticleFav(int64_t id, std::map<int64_t, int64_t>& users) {
    return true;
}

bool ArticleManager::listArticlePra(int64_t id, std::map<int64_t, int64_t>& users) {
    return true;
}

void ArticleManager::onTimer() {
}

void ArticleManager::onUpdateTimer() {
}

bool ArticleManager::addViews(uint64_t id, const std::string& cookie_id) {
    return true;
}

void ArticleManager::addUpdate(int64_t id) {
}

#undef XX

}