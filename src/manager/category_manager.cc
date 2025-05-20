#include "category_manager.h"
#include "chen/log/log.h"
#include "../util.h"

namespace blog {

static sylar::Logger::ptr logger = LOG_ROOT();

bool CategoryManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<data::CategoryInfo::ptr> results;
    if (blog::data::CategoryInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "CategoryManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, data::CategoryInfo::ptr> datas;
    std::unordered_map<int64_t, std::map<std::string, data::CategoryInfo::ptr>> users;

    for (auto& i : results) {
        datas[i->getId()] = i;
        users[i->getUserId()][i->getName()] = i;
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_users.swap(users);
    return true;
}

void CategoryManager::add(blog::data::CategoryInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
    m_users[info->getUserId()][info->getName()] = info;
}

blog::data::CategoryInfo::ptr CategoryManager::get(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    return it == m_datas.end() ? nullptr : it->second;
}

bool CategoryManager::listByUserId(std::vector<blog::data::CategoryInfo::ptr>& infos, int64_t id, bool valid) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_users.find(id);
    if (it == m_users.end()) {
        return false;
    }
    if (valid) {
        for (auto& [name, category] : it->second) {
            if (category->getIsDeleted() == 0) {
                infos.push_back(category);
            }
        }
    } else {
        for (auto& [name, category] : it->second) {
            infos.push_back(category);
        }
    }
    return true;
}

blog::data::CategoryInfo::ptr CategoryManager::getByUserIdName(int64_t id, const std::string& name) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_users.find(id);
    if (it != m_users.end()) {
        auto iit = it->second.find(name);
        return iit != it->second.end() ? iit->second : nullptr;
    }
    return nullptr;
}

bool CategoryManager::exists(int64_t id, const std::string& name) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_users.find(id);
    if (it != m_users.end()) {
        return it->second.find(name) != it->second.end();
    }
    return false;
}

}