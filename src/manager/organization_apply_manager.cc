#include "organization_apply_manager.h"
#include <algorithm>
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

bool OrganizationApplyManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get SQLite3 connection fail";
        return false;
    }

    std::vector<data::OrganizationApplyInfo::ptr> results;
    if (blog::data::OrganizationApplyInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "OrganizationApplyManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, data::OrganizationApplyInfo::ptr> datas;
    for (auto& i : results) {
        datas[i->getId()] = i;
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    return true;
}

void OrganizationApplyManager::add(data::OrganizationApplyInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
}

void OrganizationApplyManager::update(data::OrganizationApplyInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
}

data::OrganizationApplyInfo::ptr OrganizationApplyManager::get(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    if (it != m_datas.end()) {
        return it->second;
    }
    return nullptr;
}

int64_t OrganizationApplyManager::listByPages(std::vector<data::OrganizationApplyInfo::ptr>& results
        , uint64_t offset, uint64_t limit, int32_t status, bool isValid) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);

    std::vector<data::OrganizationApplyInfo::ptr> tmp;

    auto check = [&](data::OrganizationApplyInfo::ptr info) {
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

    // Sort by created_at descending (newest first)
    std::sort(tmp.begin(), tmp.end(), [](data::OrganizationApplyInfo::ptr a, data::OrganizationApplyInfo::ptr b) {
        return a->getCreatedAt() > b->getCreatedAt();
    });

    if (offset < tmp.size()) {
        for (size_t i = offset; i < tmp.size(); ++i) {
            if (results.size() >= limit) {
                break;
            }
            results.push_back(tmp[i]);
        }
    }

    return tmp.size();
}

int64_t OrganizationApplyManager::listByUserId(std::vector<data::OrganizationApplyInfo::ptr>& results
        , int64_t user_id, uint64_t offset, uint64_t limit, bool isValid) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);

    std::vector<data::OrganizationApplyInfo::ptr> tmp;

    auto check = [&](data::OrganizationApplyInfo::ptr info) {
        if (isValid && info->getIsDeleted()) {
            return false;
        }
        if (info->getUserId() != user_id) {
            return false;
        }
        return true;
    };

    for (auto& [id, info] : m_datas) {
        if (check(info)) {
            tmp.push_back(info);
        }
    }

    // Sort by created_at descending (newest first)
    std::sort(tmp.begin(), tmp.end(), [](data::OrganizationApplyInfo::ptr a, data::OrganizationApplyInfo::ptr b) {
        return a->getCreatedAt() > b->getCreatedAt();
    });

    if (offset < tmp.size()) {
        for (size_t i = offset; i < tmp.size(); ++i) {
            if (results.size() >= limit) {
                break;
            }
            results.push_back(tmp[i]);
        }
    }

    return tmp.size();
}

} // namespace blog
