#ifndef __BLOG_MANAGER_ASSIGNMENT_MANAGER_H__
#define __BLOG_MANAGER_ASSIGNMENT_MANAGER_H__

#include <memory>
#include <unordered_map>
#include "blog/data/assignment_info.h"
#include <shared_mutex>
#include <chen/singleton.h>

namespace blog {

class AssignmentManager {
public:
    typedef std::shared_ptr<AssignmentManager> ptr;

    enum Status {
        DRAFT = 0,
        ACTIVE = 1,
        INACTIVE = 2,
    };

    bool loadAll();
    void add(data::AssignmentInfo::ptr info);
    uint64_t listByPages(std::vector<data::AssignmentInfo::ptr>& infos, uint64_t offset
        , uint64_t size, int32_t status, bool isValid);
    data::AssignmentInfo::ptr get(int64_t id);
    data::AssignmentInfo::ptr getByName(const std::string& subject_name, const std::string& name);

    struct AssignmentStats {
        int64_t total = 0;
        int64_t active = 0;
        int64_t closed = 0;
        int64_t draft = 0;
    };
    AssignmentStats getStats();

private:
    std::shared_mutex m_mutex;
    // 作业id -> data
    std::unordered_map<int64_t, data::AssignmentInfo::ptr> m_datas;
    // [科目名称, 作业名称] -> 作业
    std::unordered_map<std::string, std::unordered_map<std::string, data::AssignmentInfo::ptr>> m_subject_name_datas;
};

typedef chen::Singleton<AssignmentManager> AssignmentMgr;

}

#endif // __BLOG_MANAGER_ASSIGNMENT_MANAGER_H__