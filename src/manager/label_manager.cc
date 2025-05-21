#include "label_manager.h"
#include "chen/log/log.h"
#include "../util.h"

namespace blog {

static sylar::Logger::ptr logger = LOG_ROOT();

bool LabelManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "get db connection fail";
        return false;
    }
    std::vector<data::LabelInfo::ptr> results;
    if (data::LabelInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "LabelManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, data::LabelInfo::ptr> datas;
    std::unordered_map<int64_t, std::map<std::string, data::LabelInfo::ptr>> users;
    for (auto& i : results) {
        datas[i->getId()] = i;
        users[i->getId()][i->getName()] = i;
    }
    
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_users.swap(users);

    return true;
}

void LabelManager::add(data::LabelInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
    m_users[info->getId()][info->getName()] = info;
}

data::LabelInfo::ptr LabelManager::get(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    return it == m_datas.end() ? nullptr : it->second;
}

data::LabelInfo::ptr LabelManager::getByUserIdName(int64_t id, const std::string& name) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_users.find(id);
    if (it != m_users.end()) {
        auto iit = it->second.find(name);
        return iit == it->second.end() ? nullptr : iit->second;
    }
    return nullptr;
}

bool LabelManager::listByUserId(std::vector<data::LabelInfo::ptr>& infos, int64_t id, bool valid) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_users.find(id);
    if (it == m_users.end()) {
        return false;
    }
    if (valid) {
        for (auto& i : it->second) {
            if (i.second->getIsDeleted() == 0) {
                infos.push_back(i.second);
            }
        }
    } else {
        for (auto& i : it->second) {
            infos.push_back(i.second);
        }
    }
    return true;
}


}