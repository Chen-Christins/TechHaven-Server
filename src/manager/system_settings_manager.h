#ifndef __BLOG_MANAGER_SYSTEM_SETTINGS_MANAGER_H__
#define __BLOG_MANAGER_SYSTEM_SETTINGS_MANAGER_H__

#include <shared_mutex>

#include <chen/singleton.h>

#include "blog/data/system_settings_info.h"

namespace blog {

class SystemSettingsManager {
public:
    bool loadAll();
    void add(blog::data::SystemSettingsInfo::ptr info);
    blog::data::SystemSettingsInfo::ptr get();
    bool update(blog::data::SystemSettingsInfo::ptr info);
private:
    std::shared_mutex m_mutex;
    blog::data::SystemSettingsInfo::ptr m_data;
};

typedef chen::Singleton<SystemSettingsManager> SystemSettingsMgr;

}

#endif // __BLOG_MANAGER_SYSTEM_SETTINGS_MANAGER_H__
