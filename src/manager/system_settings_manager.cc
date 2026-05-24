#include "system_settings_manager.h"

#include <chen/log/log.h>

#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

bool SystemSettingsManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<data::SystemSettingsInfo::ptr> results;
    if (blog::data::SystemSettingsInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "SystemSettingsManager loadAll fail";
        return false;
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    if (results.empty()) {
        m_data.reset(new data::SystemSettingsInfo);
        m_data->setId(1);
        if (blog::data::SystemSettingsInfoDao::Insert(m_data, db)) {
            ERROR(logger) << "SystemSettingsManager insert default row fail";
            return false;
        }
    } else {
        m_data = results[0];
    }
    return true;
}

void SystemSettingsManager::add(blog::data::SystemSettingsInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_data = info;
}

blog::data::SystemSettingsInfo::ptr SystemSettingsManager::get() {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    return m_data;
}

bool SystemSettingsManager::update(blog::data::SystemSettingsInfo::ptr info) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get SQLite3 connection fail";
        return false;
    }
    info->setId(1);
    if (blog::data::SystemSettingsInfoDao::InsertOrUpdate(info, db)) {
        ERROR(logger) << "SystemSettingsManager update fail";
        return false;
    }
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_data = info;
    return true;
}

}
