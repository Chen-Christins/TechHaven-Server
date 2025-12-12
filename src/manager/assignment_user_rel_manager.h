#ifndef __BLOG_MANAGER_ASSIGNMENT_USER_REL_MANAGER_H__
#define __BLOG_MANAGER_ASSIGNMENT_USER_REL_MANAGER_H__

#include <shared_mutex>
#include <unordered_map>
#include "blog/data/assignment_user_rel_info.h"
#include <chen/singleton.h>

namespace blog {

class AssignmentUserRelManager {
public:
    bool loadAll();
    void add(blog::data::AssignmentUserRelInfo::ptr info);
    blog::data::AssignmentUserRelInfo::ptr get(int64_t id);
private:
    std::shared_mutex m_mutex;
    std::unordered_map<int64_t, blog::data::AssignmentUserRelInfo::ptr> m_datas;
};

typedef chen::Singleton<AssignmentUserRelManager> AssignmentUserRelMgr;

}

#endif // __BLOG_MANAGER_ASSIGNMENT_USER_REL_MANAGER_H__