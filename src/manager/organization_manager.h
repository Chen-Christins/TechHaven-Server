#ifndef __BLOG_MANAGER_ORGANIZATION_MANAGER_H__
#define __BLOG_MANAGER_ORGANIZATION_MANAGER_H__

#include <shared_mutex>
#include <unordered_map>
#include "blog/data/organization_info.h"
#include <chen/singleton.h>
#include <set>

namespace blog {

class OrganizationManager {
public:
    enum Role {
        MEMBER = 1,
        ADMIN = 2,
        OWNER = 3
    };
    enum Status {
        INACTIVE = 0,
        ACTIVE = 1,
    };
    bool loadAll();
    void add(data::OrganizationInfo::ptr info);
    data::OrganizationInfo::ptr get(int64_t id);
    data::OrganizationInfo::ptr getByName(const std::string& name);
    int64_t listByPages(std::vector<data::OrganizationInfo::ptr>& orgs
        , uint64_t offset, uint64_t limit, int32_t status, bool isValid);

    struct OrganizationStats {
        int64_t total = 0;
        int64_t active = 0;
        int64_t inactive = 0;
    };
    OrganizationStats getStats();
private:
    // 读写锁
    std::shared_mutex m_mutex;
    // id -> info
    std::unordered_map<int64_t, data::OrganizationInfo::ptr> m_datas;
    // name -> info
    std::unordered_map<std::string, data::OrganizationInfo::ptr> m_names;
    // user_id -> set<organization>
    std::unordered_map<int64_t, std::set<data::OrganizationInfo::ptr>> m_userOrganizations;
};

typedef chen::Singleton<OrganizationManager> OrganizationMgr;

}

#endif // __BLOG_MANAGER_ORGANIZATION_MANAGER_H__