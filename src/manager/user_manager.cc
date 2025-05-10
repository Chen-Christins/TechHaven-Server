#include "user_manager.h"
#include "../my_module.h"
#include "log/log.h"


namespace blog {

static sylar::Logger::ptr logger = LOG_ROOT();

bool UserManager::loadAll() {
    auto db = blog::GetSQLite3();
    if (!db) {
        return false;
    }
    std::vector<data::UserInfo::ptr> results;
    if (blog::data::UserInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "UserManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, blog::data::UserInfo::ptr> datas;
    std::unordered_map<std::string, blog::data::UserInfo::ptr> accounts;
    std::unordered_map<std::string, blog::data::UserInfo::ptr> emails;
    std::unordered_map<std::string, blog::data::UserInfo::ptr> names;

    for (auto& i : results) {
        datas[i->getId()] = i;
        accounts[i->getAccount()] = i;
        emails[i->getEmail()] = i;
        names[i->getName()] = i;
    }
    
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_accounts.swap(accounts);
    m_emails.swap(emails);
    m_names.swap(names);

    return true;
}

void UserManager::add(blog::data::UserInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
    m_accounts[info->getAccount()] = info;
    m_emails[info->getEmail()] = info;
    m_names[info->getName()] = info;
}

#define XX(map, key)                                   \
    std::shared_lock<std::shared_mutex> lock(m_mutex); \
    auto it = map.find(key);                           \
    return it == map.end() ? nullptr : it->second;

blog::data::UserInfo::ptr UserManager::get(int64_t id) {
    XX(m_datas, id);
}

blog::data::UserInfo::ptr UserManager::getByAccount(const std::string& v) {
    XX(m_accounts, v);
}

blog::data::UserInfo::ptr UserManager::getByEmail(const std::string& v) {
    XX(m_emails, v);
}

blog::data::UserInfo::ptr UserManager::getByName(const std::string& v) {
    XX(m_names, v);
}

#undef XX

}