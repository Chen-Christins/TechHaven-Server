#include "category_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

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

    for (auto& i : results) {
        datas[i->getId()] = i;
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    return true;
}

void CategoryManager::add(blog::data::CategoryInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
}

blog::data::CategoryInfo::ptr CategoryManager::get(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    return it == m_datas.end() ? nullptr : it->second;
}

void CategoryManager::listAll(std::vector<blog::data::CategoryInfo::ptr>& infos, bool isValid) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    for (auto& i : m_datas) {
        if (isValid && i.second->getIsDeleted()) {
            continue;
        }
        infos.push_back(i.second);
    }
}

blog::data::CategoryInfo::ptr CategoryManager::getByName(const std::string& name) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    for (auto& i : m_datas) {
        if (i.second->getName() == name) {
            return i.second;
        }
    }
    return nullptr;
}

}