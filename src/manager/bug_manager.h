#ifndef __BLOG_MANAGER_BUG_MANAGER_H__
#define __BLOG_MANAGER_BUG_MANAGER_H__

#include <memory>
#include <unordered_map>
#include <vector>
#include "blog/data/bug_info.h"
#include <shared_mutex>
#include <chen/singleton.h>

namespace blog {

class BugManager {
public:
    enum Severity {
        SEVERITY_MINOR   = 1,  // 轻微
        SEVERITY_NORMAL  = 2,  // 一般
        SEVERITY_MAJOR   = 3,  // 严重
        SEVERITY_CRITICAL = 4  // 致命
    };
    enum Priority {
        PRIORITY_LOW    = 1,  // 低
        PRIORITY_MEDIUM = 2,  // 中
        PRIORITY_HIGH   = 3,  // 高
        PRIORITY_URGENT = 4   // 紧急
    };
    enum Status {
        STATUS_PENDING    = 0,  // 待处理
        STATUS_INPROGRESS = 1,  // 进行中
        STATUS_FIXED      = 2,  // 已修复
        STATUS_CLOSED     = 3,  // 已关闭
        STATUS_REOPENED   = 4   // 重新打开
    };

    bool loadAll();
    void add(data::BugInfo::ptr info);
    data::BugInfo::ptr get(int64_t id);
    uint64_t listByPages(std::vector<data::BugInfo::ptr>& infos,
        uint64_t offset, uint64_t size, int32_t status, bool isValid);
    uint64_t listByOrg(std::vector<data::BugInfo::ptr>& infos,
        int64_t orgId, uint64_t offset, uint64_t size, int32_t status, bool isValid);

private:
    std::shared_mutex m_mutex;
    // id -> info
    std::unordered_map<int64_t, data::BugInfo::ptr> m_datas;
    // org_id -> [id -> info]
    std::unordered_map<int64_t, std::unordered_map<int64_t, data::BugInfo::ptr>> m_org_datas;
};

typedef chen::Singleton<BugManager> BugMgr;

}

#endif // __BLOG_MANAGER_BUG_MANAGER_H__
