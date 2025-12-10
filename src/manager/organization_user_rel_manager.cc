#include "organization_user_rel_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

bool OrganizationUserRelManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get SQLite3 connection fail";
        return false;
    }

    std::vector<data::OrganizationUserRelInfo::ptr> results;
    if (blog::data::OrganizationUserRelInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "OrganizationUserRelManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, data::OrganizationUserRelInfo::ptr> datas;
    std::unordered_map<int64_t, std::unordered_map<int64_t, data::OrganizationUserRelInfo::ptr>> org_user_datas;
    std::unordered_map<int64_t, std::unordered_map<int64_t, data::OrganizationUserRelInfo::ptr>> user_org_datas;
    for (auto& i : results) {
        datas[i->getId()] = i;
        org_user_datas[i->getOrgId()][i->getUserId()] = i;
        user_org_datas[i->getUserId()][i->getOrgId()] = i;
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_org_user_datas.swap(org_user_datas);
    m_user_org_datas.swap(user_org_datas);
    return true;
}

void OrganizationUserRelManager::add(data::OrganizationUserRelInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
    m_org_user_datas[info->getOrgId()][info->getUserId()] = info;
    m_user_org_datas[info->getUserId()][info->getOrgId()] = info;
}

data::OrganizationUserRelInfo::ptr OrganizationUserRelManager::get(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    if (it != m_datas.end()) {
        return it->second;
    }
    return nullptr;
}

data::OrganizationUserRelInfo::ptr OrganizationUserRelManager::getByOrgAndUser(int64_t o_id, int64_t u_id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_org_user_datas.find(o_id);
    if (it != m_org_user_datas.end()) {
        auto uit = it->second.find(u_id);
        if (uit != it->second.end()) {
            return uit->second;
        }
    }
    return nullptr;
}

int64_t OrganizationUserRelManager::getByPages(std::vector<data::OrganizationUserRelInfo::ptr>& results
        , int64_t id, uint64_t offset, uint64_t size, int32_t status, bool isValid) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);

    auto check = [&](data::OrganizationUserRelInfo::ptr info) {
        if (status != -1 && info->getStatus() != status) {
            return false;
        }
        if (isValid && info->getIsDeleted()) {
            return false;
        }
        return true;
    };

    std::vector<data::OrganizationUserRelInfo::ptr> tmp;

    for (auto& i : m_org_user_datas[id]) {
        if (check(i.second)) {
            tmp.push_back(i.second);
        }
    }

    if (offset < tmp.size()) {
        for (size_t i = offset; i < tmp.size(); ++i) {
            if (results.size() >= size) {
                break;
            }
            results.push_back(tmp[i]);
        }
    }
    
    return tmp.size();
}

int64_t OrganizationUserRelManager::getOrgByUserId(std::vector<data::OrganizationUserRelInfo::ptr>& results
        , int64_t u_id, int32_t status, bool isValid) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);

    auto check = [&](data::OrganizationUserRelInfo::ptr info) {
        if (status != -1 && info->getStatus() != status) {
            return false;
        }
        if (isValid && info->getIsDeleted()) {
            return false;
        }
        return true;
    };

    for (auto& i : m_user_org_datas[u_id]) {
        if (check(i.second)) {
            results.push_back(i.second);
        }
    }
    return results.size();
}

int64_t OrganizationUserRelManager::getMemberCount(int64_t o_id, int32_t status, bool isValid) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);

    auto check = [&](data::OrganizationUserRelInfo::ptr info) {
        if (status != -1 && info->getStatus() != status) {
            return false;
        }
        if (isValid && info->getIsDeleted()) {
            return false;
        }
        return true;
    };

    int64_t count = 0;
    for (auto& i : m_org_user_datas[o_id]) {
        if (check(i.second)) {
            ++count;
        }
    }
    return count;
}

}