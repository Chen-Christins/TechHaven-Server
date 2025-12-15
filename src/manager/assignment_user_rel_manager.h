#ifndef __BLOG_MANAGER_ASSIGNMENT_USER_REL_MANAGER_H__
#define __BLOG_MANAGER_ASSIGNMENT_USER_REL_MANAGER_H__

#include <shared_mutex>
#include <unordered_map>
#include "blog/data/assignment_user_rel_info.h"
#include <chen/singleton.h>

namespace blog {

class AssignmentUserRelManager {
public:
    enum Status {
        SUBMITTED = 1,
        GRADED = 2,
        LATE = 3
    };
    bool loadAll();
    void add(blog::data::AssignmentUserRelInfo::ptr info);
    blog::data::AssignmentUserRelInfo::ptr get(int64_t id);
    blog::data::AssignmentUserRelInfo::ptr getByAssignAndUser(int64_t assign_id, int64_t user_id);
private:
    std::shared_mutex m_mutex;
    // id -> info
    std::unordered_map<int64_t, blog::data::AssignmentUserRelInfo::ptr> m_datas;
    // (assign_id, user_id) -> info
    std::unordered_map<int64_t, std::unordered_map<int64_t, blog::data::AssignmentUserRelInfo::ptr>> m_assign_user;
};

typedef chen::Singleton<AssignmentUserRelManager> AssignmentUserRelMgr;

}

#endif // __BLOG_MANAGER_ASSIGNMENT_USER_REL_MANAGER_H__