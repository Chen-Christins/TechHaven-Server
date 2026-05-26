#ifndef __BLOG_MANAGER_USER_MANAGER_H__
#define __BLOG_MANAGER_USER_MANAGER_H__

#include "blog/data/user_info.h"
#include <chen/singleton.h>
#include <unordered_map>
#include <shared_mutex>

namespace blog {

class UserManager {
public:
    enum Role {
        USER = 1,
        ADMIN = 2,
        EDITOR = 3,
        CHECKER = 4
    };
    enum Status {
        INACTIVE = 0,
        ACTIVE = 1,
        BANNED = 2
    };
    bool loadAll();
    void add(blog::data::UserInfo::ptr info);
    void update(blog::data::UserInfo::ptr info, int32_t old_role, const std::string& old_account, const std::string& old_email);
    void getAllIds(std::vector<int64_t>& ids, bool isValid);
	
    uint64_t listByPages(std::vector<blog::data::UserInfo::ptr>& infos, uint64_t offset, uint64_t size
        , int32_t role, int32_t state, int32_t days, bool isValid);

	blog::data::UserInfo::ptr get(int64_t id);
    blog::data::UserInfo::ptr getByAccount(const std::string& v);
    blog::data::UserInfo::ptr getByEmail(const std::string& v);
    blog::data::UserInfo::ptr getByName(const std::string& v);

    static std::string GetToken(data::UserInfo::ptr info, int64_t us);
    static std::string generateToken();
private:
    std::unordered_map<int64_t, blog::data::UserInfo::ptr> m_datas;
    std::unordered_map<std::string, blog::data::UserInfo::ptr> m_accounts;
    std::unordered_map<std::string, blog::data::UserInfo::ptr> m_emails;
    std::unordered_map<std::string, blog::data::UserInfo::ptr> m_names;
    std::unordered_map<int32_t, std::unordered_map<int64_t, blog::data::UserInfo::ptr>> m_role_id_users;
    std::shared_mutex m_mutex;
};

typedef chen::Singleton<UserManager> UserMgr;

}

#endif // __BLOG_MANAGER_USER_MANAGER_H__