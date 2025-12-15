#include "assignment_user_rel_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

bool AssignmentUserRelManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<blog::data::AssignmentUserRelInfo::ptr> results;
    if (blog::data::AssignmentUserRelInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "AssignmentUserRelManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, blog::data::AssignmentUserRelInfo::ptr> datas;
    std::unordered_map<int64_t, std::unordered_map<int64_t, blog::data::AssignmentUserRelInfo::ptr>> assign_user;
    for (auto& i : results) {
        datas[i->getId()] = i;
        assign_user[i->getAssignmentId()][i->getUserId()] = i;
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_assign_user.swap(assign_user);
    return true;
}

void AssignmentUserRelManager::add(blog::data::AssignmentUserRelInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
    m_assign_user[info->getAssignmentId()][info->getUserId()] = info;
}

blog::data::AssignmentUserRelInfo::ptr AssignmentUserRelManager::get(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    if (it != m_datas.end()) {
        return it->second;
    }
    return nullptr;
}

blog::data::AssignmentUserRelInfo::ptr AssignmentUserRelManager::getByAssignAndUser(int64_t assign_id, int64_t user_id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_assign_user.find(assign_id);
    if (it != m_assign_user.end()) {
        auto uit = it->second.find(user_id);
        if (uit != it->second.end()) {
            return uit->second;
        }
    }
    return nullptr;
}

}