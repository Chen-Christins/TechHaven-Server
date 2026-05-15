#ifndef __BLOG_MANAGER_ASSIGNMENT_ORGANIZATION_REL_MANAGER_H__
#define __BLOG_MANAGER_ASSIGNMENT_ORGANIZATION_REL_MANAGER_H__

#include <shared_mutex>
#include <unordered_map>
#include "blog/data/assignment_organization_rel_info.h"
#include <chen/singleton.h>

namespace blog {

class AssignmentOrganizationRelManager {
public:
    bool loadAll();
    void add(blog::data::AssignmentOrganizationRelInfo::ptr info);
    blog::data::AssignmentOrganizationRelInfo::ptr get(int64_t id);
    blog::data::AssignmentOrganizationRelInfo::ptr getByOrgAndAssign(int64_t org_id, int64_t assign_id);
    int64_t getByAssignmentId(std::vector<data::AssignmentOrganizationRelInfo::ptr>& results, int64_t assign_id);
    int64_t getByPages(std::vector<data::AssignmentOrganizationRelInfo::ptr>& results
        , int64_t o_id, uint64_t offset, uint64_t size, int32_t status, bool isValid);
private:
    std::shared_mutex m_mutex;
    // id -> info
    std::unordered_map<int64_t, blog::data::AssignmentOrganizationRelInfo::ptr> m_datas;
    // o_id -> [a_id, info]
    std::unordered_map<int64_t, std::unordered_map<int64_t, blog::data::AssignmentOrganizationRelInfo::ptr>> m_org_assign_datas;
    // a_id -> [o_id, info]
    std::unordered_map<int64_t, std::unordered_map<int64_t, blog::data::AssignmentOrganizationRelInfo::ptr>> m_assign_org_datas;
};

typedef chen::Singleton<AssignmentOrganizationRelManager> AssignmentOrganizationRelMgr;

}

#endif // __BLOG_MANAGER_ASSIGNMENT_ORGANIZATION_REL_MANAGER_H__