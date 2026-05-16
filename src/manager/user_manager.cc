#include "user_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

bool UserManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get SQLite3 connection fail";
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
    std::unordered_map<int32_t, std::unordered_map<int64_t, blog::data::UserInfo::ptr>> role_id_users;

    for (auto& i : results) {
        datas[i->getId()] = i;
        accounts[i->getAccount()] = i;
        emails[i->getEmail()] = i;
        names[i->getName()] = i;
        role_id_users[i->getRole()][i->getId()] = i;
    }
    
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_accounts.swap(accounts);
    m_emails.swap(emails);
    m_names.swap(names);
    m_role_id_users.swap(role_id_users);
    return true;
}

void UserManager::getAllIds(std::vector<int64_t>& ids, bool isValid) {
	std::shared_lock<std::shared_mutex> lock(m_mutex);
	for (auto [id, user] : m_datas) {
		if (isValid && user->getIsDeleted()) {
			continue;
		}
		ids.emplace_back(id);
	}
}

uint64_t UserManager::listByPages(std::vector<blog::data::UserInfo::ptr>& infos, uint64_t offset, uint64_t size
        , int32_t role, int32_t state, int32_t days, bool isValid) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    std::vector<blog::data::UserInfo::ptr> temp;
    
    int64_t start_time = 0;
    if (days > 0) {
        start_time = time(0) - days * 24 * 3600;
    }

    auto check = [&](blog::data::UserInfo::ptr info) -> bool {
        if (isValid && info->getIsDeleted()) {
            return false;
        }
        if (state != -1 && info->getState() != state) {
            return false;
        }
        if (start_time && info->getCreateTime() < start_time) {
            return false;
        }
        return true;
    };

    if (role != -1) {
        auto it = m_role_id_users.find(role);
        if (it != m_role_id_users.end()) {
            for (auto& i : it->second) {
                if (check(i.second)) {
                    temp.push_back(i.second);
                }
            }
        }
    } else {
        for (auto& i : m_datas) {
            if (check(i.second)) {
                temp.push_back(i.second);
            }
        }
    }

    // std::sort(temp.begin(), temp.end(), [](const auto& a, const auto& b) {
    //     return a->getId() > b->getId();
    // });

    if (offset < temp.size()) {
        for (size_t i = offset; i < temp.size(); ++i) {
            if (infos.size() >= size) {
                break;
            }
            infos.push_back(temp[i]);
        }
    }
    return temp.size();
}

void UserManager::add(blog::data::UserInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
    m_accounts[info->getAccount()] = info;
    m_emails[info->getEmail()] = info;
    m_names[info->getName()] = info;
    m_role_id_users[info->getRole()][info->getId()] = info;
}

std::string UserManager::GetToken(data::UserInfo::ptr info, int64_t us) {
    std::stringstream ss;
    ss << info->getId()
       << "|" << info->getAccount()
       << "|" << info->getEmail()
       << "|" << info->getPasswd()
       << "|" << us;
    return chen::md5(ss.str());
}

std::string UserManager::generateToken() {
    std::stringstream ss;
    ss << std::hex << chen::GetCurrentUs() << rand() << rand();
    return chen::md5(ss.str());
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