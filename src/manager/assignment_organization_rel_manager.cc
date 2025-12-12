#include "assignment_organization_rel_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

bool AssignmentOrganizationRelManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<blog::data::AssignmentOrganizationRelInfo::ptr> results;
    if (blog::data::AssignmentOrganizationRelInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "AssignmentOrganizationRelManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, blog::data::AssignmentOrganizationRelInfo::ptr> datas;
    for (auto& i : results) {
        datas[i->getId()] = i;
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    return true;
}

void AssignmentOrganizationRelManager::add(blog::data::AssignmentOrganizationRelInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
}

blog::data::AssignmentOrganizationRelInfo::ptr AssignmentOrganizationRelManager::get(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    if (it != m_datas.end()) {
        return it->second;
    }
    return nullptr;
}

}