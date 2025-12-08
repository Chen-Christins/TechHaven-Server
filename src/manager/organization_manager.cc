#include "organization_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

bool OrganizationManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get SQLite3 connection fail";
        return false;
    }

    std::vector<data::OrganizationInfo::ptr> results;
    if (blog::data::OrganizationInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "OrganizationManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, data::OrganizationInfo::ptr> datas;
    std::unordered_map<std::string, data::OrganizationInfo::ptr> names;
    std::unordered_map<int64_t, std::set<data::OrganizationInfo::ptr>> userOrganizations;
    for (auto& i : results) {
        datas[i->getId()] = i;
        names[i->getName()] = i;
        userOrganizations[i->getOwnerId()].insert(i);
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_names.swap(names);
    m_userOrganizations.swap(userOrganizations);
    return true;
}

void OrganizationManager::add(data::OrganizationInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
    m_names[info->getName()] = info;
    m_userOrganizations[info->getOwnerId()].insert(info);
}

data::OrganizationInfo::ptr OrganizationManager::get(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    if (it != m_datas.end()) {
        return it->second;
    }
    return nullptr;
}

data::OrganizationInfo::ptr OrganizationManager::getByName(const std::string& name) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_names.find(name);
    if (it != m_names.end()) {
        return it->second;
    }
    return nullptr;
}

int64_t OrganizationManager::listByPages(std::vector<data::OrganizationInfo::ptr>& orgs
        , uint64_t offset, uint64_t limit, int32_t status, bool isValid) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);

    std::vector<data::OrganizationInfo::ptr> tmp;

    auto check = [&](data::OrganizationInfo::ptr info) {
        if (isValid && info->getIsDeleted()) {
            return false;
        }
        if (status != -1 && info->getStatus() != status) {
            return false;
        }
        return true;
    };

    for (auto& [id, info] : m_datas) {
        if (check(info)) {
            tmp.push_back(info);
        }
    }

    if (offset < tmp.size()) {
        for (size_t i = offset; i < tmp.size(); ++i) {
            if (orgs.size() >= limit) {
                break;
            }
            orgs.push_back(tmp[i]);
        }
    }

    return tmp.size();
}

} // namespace blog