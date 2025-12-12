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
private:
    std::shared_mutex m_mutex;
    std::unordered_map<int64_t, blog::data::AssignmentOrganizationRelInfo::ptr> m_datas;
};

typedef chen::Singleton<AssignmentOrganizationRelManager> AssignmentOrganizationRelMgr;

}

#endif // __BLOG_MANAGER_ASSIGNMENT_ORGANIZATION_REL_MANAGER_H__