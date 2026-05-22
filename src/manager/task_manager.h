#ifndef __BLOG_MANAGER_TASK_MANAGER_H__
#define __BLOG_MANAGER_TASK_MANAGER_H__

#include <memory>
#include <unordered_map>
#include <vector>
#include "blog/data/task_info.h"
#include <shared_mutex>
#include <chen/singleton.h>

namespace blog {

class TaskManager {
public:
    enum Priority {
        PRIORITY_LOW    = 1,  // 低
        PRIORITY_MEDIUM = 2,  // 中
        PRIORITY_HIGH   = 3,  // 高
        PRIORITY_URGENT = 4   // 紧急
    };
    enum Status {
        STATUS_TODO       = 0,  // 待办
        STATUS_INPROGRESS = 1,  // 进行中
        STATUS_DONE       = 2,  // 已完成
        STATUS_CLOSED     = 3   // 已关闭
    };

    bool loadAll();
    void add(data::TaskInfo::ptr info);
    data::TaskInfo::ptr get(int64_t id);
    uint64_t listByPages(std::vector<data::TaskInfo::ptr>& infos,
        uint64_t offset, uint64_t size, int32_t status, bool isValid);
    uint64_t listByOrg(std::vector<data::TaskInfo::ptr>& infos,
        int64_t orgId, uint64_t offset, uint64_t size, int32_t status, bool isValid);

private:
    std::shared_mutex m_mutex;
    // id -> info
    std::unordered_map<int64_t, data::TaskInfo::ptr> m_datas;
    // org_id -> [id -> info]
    std::unordered_map<int64_t, std::unordered_map<int64_t, data::TaskInfo::ptr>> m_org_datas;
};

typedef chen::Singleton<TaskManager> TaskMgr;

}

#endif // __BLOG_MANAGER_TASK_MANAGER_H__
