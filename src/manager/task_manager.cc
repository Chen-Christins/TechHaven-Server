#include "task_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

bool TaskManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<data::TaskInfo::ptr> results;
    if (blog::data::TaskInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "TaskManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, data::TaskInfo::ptr> datas;
    std::unordered_map<int64_t, std::unordered_map<int64_t, data::TaskInfo::ptr>> org_datas;
    for (auto& i : results) {
        datas[i->getId()] = i;
        org_datas[i->getOrgId()][i->getId()] = i;
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_org_datas.swap(org_datas);
    return true;
}

void TaskManager::add(data::TaskInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
    m_org_datas[info->getOrgId()][info->getId()] = info;
}

data::TaskInfo::ptr TaskManager::get(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    return it == m_datas.end() ? nullptr : it->second;
}

uint64_t TaskManager::listByPages(std::vector<data::TaskInfo::ptr>& infos,
        uint64_t offset, uint64_t size, int32_t status, bool isValid) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);

    auto check = [&](auto info) -> bool {
        if (isValid && info->getIsDeleted()) {
            return false;
        }
        if (status != -1 && info->getStatus() != status) {
            return false;
        }
        return true;
    };

    std::vector<data::TaskInfo::ptr> temp;
    for (auto& i : m_datas) {
        if (check(i.second)) {
            temp.emplace_back(i.second);
        }
    }

    if (offset < temp.size()) {
        for (size_t i = offset; i < temp.size(); ++i) {
            if (infos.size() >= size) {
                break;
            }
            infos.emplace_back(temp[i]);
        }
    }
    return temp.size();
}

uint64_t TaskManager::listByOrg(std::vector<data::TaskInfo::ptr>& infos,
        int64_t orgId, uint64_t offset, uint64_t size, int32_t status, bool isValid) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);

    auto it = m_org_datas.find(orgId);
    if (it == m_org_datas.end()) {
        return 0;
    }

    auto check = [&](auto info) -> bool {
        if (isValid && info->getIsDeleted()) {
            return false;
        }
        if (status != -1 && info->getStatus() != status) {
            return false;
        }
        return true;
    };

    std::vector<data::TaskInfo::ptr> temp;
    for (auto& i : it->second) {
        if (check(i.second)) {
            temp.emplace_back(i.second);
        }
    }

    if (offset < temp.size()) {
        for (size_t i = offset; i < temp.size(); ++i) {
            if (infos.size() >= size) {
                break;
            }
            infos.emplace_back(temp[i]);
        }
    }
    return temp.size();
}

}
