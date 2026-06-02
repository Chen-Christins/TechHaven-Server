#ifndef __BLOG_MANAGER_ORGANIZATION_APPLY_MANAGER_H__
#define __BLOG_MANAGER_ORGANIZATION_APPLY_MANAGER_H__

#include <shared_mutex>
#include <unordered_map>
#include "blog/data/organization_apply_info.h"
#include <chen/singleton.h>

namespace blog {

class OrganizationApplyManager {
public:
    enum Status {
        PENDING  = 0,
        APPROVED = 1,
        REJECTED = 2
    };

    bool loadAll();
    void add(data::OrganizationApplyInfo::ptr info);
    void update(data::OrganizationApplyInfo::ptr info);
    data::OrganizationApplyInfo::ptr get(int64_t id);

    int64_t listByPages(std::vector<data::OrganizationApplyInfo::ptr>& results
        , uint64_t offset, uint64_t limit, int32_t status, bool isValid);

    int64_t listByUserId(std::vector<data::OrganizationApplyInfo::ptr>& results
        , int64_t user_id, uint64_t offset, uint64_t limit, bool isValid);

private:
    std::shared_mutex m_mutex;
    std::unordered_map<int64_t, data::OrganizationApplyInfo::ptr> m_datas;
};

typedef chen::Singleton<OrganizationApplyManager> OrganizationApplyMgr;

}

#endif // __BLOG_MANAGER_ORGANIZATION_APPLY_MANAGER_H__
