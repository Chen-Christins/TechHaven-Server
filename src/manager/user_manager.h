#ifndef __BLOG_MANAGER_USER_MANAGER_H__
#define __BLOG_MANAGER_USER_MANAGER_H__

#include "blog/data/user_info.h"
#include <chen/singleton.h>
#include <unordered_map>
#include <shared_mutex>

namespace blog {

class UserManager {
public:
    bool loadAll();
    void add(blog::data::UserInfo::ptr info);
    void getAllIds(std::vector<int64_t>& ids, bool isValid);
	
	blog::data::UserInfo::ptr get(int64_t id);
    blog::data::UserInfo::ptr getByAccount(const std::string& v);
    blog::data::UserInfo::ptr getByEmail(const std::string& v);
    blog::data::UserInfo::ptr getByName(const std::string& v);

    static std::string GetToken(data::UserInfo::ptr info, int64_t us);
private:
    std::unordered_map<int64_t, blog::data::UserInfo::ptr> m_datas;
    std::unordered_map<std::string, blog::data::UserInfo::ptr> m_accounts;
    std::unordered_map<std::string, blog::data::UserInfo::ptr> m_emails;
    std::unordered_map<std::string, blog::data::UserInfo::ptr> m_names;
    std::shared_mutex m_mutex;
};

typedef chen::Singleton<UserManager> UserMgr;

}

#endif // __BLOG_MANAGER_USER_MANAGER_H__