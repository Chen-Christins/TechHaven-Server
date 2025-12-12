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
    std::unordered_map<int64_t, std::unordered_map<int64_t, blog::data::AssignmentOrganizationRelInfo::ptr>> org_assign_datas;
    for (auto& i : results) {
        datas[i->getId()] = i;
        org_assign_datas[i->getOrganizationId()][i->getAssignmentId()] = i;
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_org_assign_datas.swap(org_assign_datas);
    return true;
}

void AssignmentOrganizationRelManager::add(blog::data::AssignmentOrganizationRelInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
    m_org_assign_datas[info->getOrganizationId()][info->getAssignmentId()] = info;
}

blog::data::AssignmentOrganizationRelInfo::ptr AssignmentOrganizationRelManager::get(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    if (it != m_datas.end()) {
        return it->second;
    }
    return nullptr;
}

blog::data::AssignmentOrganizationRelInfo::ptr AssignmentOrganizationRelManager::getByOrgAndAssign(int64_t org_id, int64_t assign_id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto org_it = m_org_assign_datas.find(org_id);
    if (org_it != m_org_assign_datas.end()) {
        auto assign_it = org_it->second.find(assign_id);
        if (assign_it != org_it->second.end()) {
            return assign_it->second;
        }
    }
    return nullptr;
}

int64_t AssignmentOrganizationRelManager::getByPages(std::vector<data::AssignmentOrganizationRelInfo::ptr>& results
        , int64_t o_id, uint64_t offset, uint64_t size, int32_t status, bool isValid) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    auto check = [&](data::AssignmentOrganizationRelInfo::ptr info) {
        if (status != -1 && info->getStatus() != status) {
            return false;
        }
        if (isValid && info->getIsDeleted()) {
            return false;
        }
        return true;
    };

    std::vector<data::AssignmentOrganizationRelInfo::ptr> tmp;

    for (auto& i : m_org_assign_datas[o_id]) {
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

}