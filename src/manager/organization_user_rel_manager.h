#ifndef __BLOG_MANAGER_ORGANIZATION_USER_REL_MANAGER_H__
#define __BLOG_MANAGER_ORGANIZATION_USER_REL_MANAGER_H__

#include <shared_mutex>
#include <unordered_map>
#include "blog/data/organization_user_rel_info.h"
#include <chen/singleton.h>

namespace blog {

class OrganizationUserRelManager {
public:
    bool loadAll();
    void add(data::OrganizationUserRelInfo::ptr info);
    data::OrganizationUserRelInfo::ptr get(int64_t id);
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