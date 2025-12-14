#ifndef __BLOG_MANAGER_ORGANIZATION_USER_REL_MANAGER_H__
#define __BLOG_MANAGER_ORGANIZATION_USER_REL_MANAGER_H__

#include <shared_mutex>
#include <unordered_map>
#include "blog/data/organization_user_rel_info.h"
#include <chen/singleton.h>

namespace blog {

class OrganizationUserRelManager {
public:
    enum Status {
        PENDING = 0,
        APPROVED = 1,
        REJECTED = 2,
        EXITED = 3
    };
    bool loadAll();
    void add(data::OrganizationUserRelInfo::ptr info);
    data::OrganizationUserRelInfo::ptr get(int64_t id);
    data::OrganizationUserRelInfo::ptr getByOrgAndUser(int64_t o_id, int64_t u_id);
    int64_t getByPages(std::vector<data::OrganizationUserRelInfo::ptr>& results
        , int64_t o_id, uint64_t offset, uint64_t size, int32_t status, bool isValid);
    
    int64_t getOrgByUserId(std::vector<data::OrganizationUserRelInfo::ptr>& results
        , int64_t u_id, int32_t status, bool isValid);

    int64_t getMemberCount(int64_t o_id, int32_t status, bool isValid);
private:
    // 读写锁
    std::shared_mutex m_mutex;
    // id -> info
    std::unordered_map<int64_t, data::OrganizationUserRelInfo::ptr> m_datas;
    // o_id -> [u_id, info]
    std::unordered_map<int64_t, std::unordered_map<int64_t, data::OrganizationUserRelInfo::ptr>> m_org_user_datas;
    // u_id -> [o_id, info]
    std::unordered_map<int64_t, std::unordered_map<int64_t, data::OrganizationUserRelInfo::ptr>> m_user_org_datas;
};

typedef chen::Singleton<OrganizationUserRelManager> OrganizationUserRelMgr;

}

#endif // __BLOG_MANAGER_ORGANIZATION_USER_REL_MANAGER_H__