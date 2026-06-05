#pragma once

#include <mutex>
#include "blog/data/system_settings_info.h"
#include <chen/singleton.h>

namespace blog {

class SystemSettingsManager {
public:
    bool loadAll();
    void add(blog::data::SystemSettingsInfo::ptr info);
    blog::data::SystemSettingsInfo::ptr get();
    bool update(blog::data::SystemSettingsInfo::ptr info);
private:
    std::mutex m_mutex;
    blog::data::SystemSettingsInfo::ptr m_data;
};

typedef chen::Singleton<SystemSettingsManager> SystemSettingsMgr;

}
