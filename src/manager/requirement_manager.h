#ifndef __BLOG_MANAGER_REQUIREMENT_MANAGER_H__
#define __BLOG_MANAGER_REQUIREMENT_MANAGER_H__

#include <memory>
#include <unordered_map>
#include <vector>
#include "blog/data/requirement_info.h"
#include <shared_mutex>
#include <chen/singleton.h>

namespace blog {

class RequirementManager {
public:
    enum Priority {
        PRIORITY_LOW    = 1,  // 低
        PRIORITY_MEDIUM = 2,  // 中
        PRIORITY_HIGH   = 3,  // 高
        PRIORITY_URGENT = 4   // 紧急
    };
    enum Status {
        STATUS_DRAFT     = 0,  // 草稿
        STATUS_PENDING   = 1,  // 待处理
        STATUS_INPROGRESS = 2, // 进行中
        STATUS_DONE      = 3,  // 已完成
        STATUS_CLOSED    = 4   // 已关闭
    };

    bool loadAll();
    void add(data::RequirementInfo::ptr info);
    data::RequirementInfo::ptr get(int64_t id);
    uint64_t listByPages(std::vector<data::RequirementInfo::ptr>& infos,
        uint64_t offset, uint64_t size, int32_t status, bool isValid);
    uint64_t listByOrg(std::vector<data::RequirementInfo::ptr>& infos,
        int64_t orgId, uint64_t offset, uint64_t size, int32_t status, bool isValid);

private:
    std::shared_mutex m_mutex;
    // id -> info
    std::unordered_map<int64_t, data::RequirementInfo::ptr> m_datas;
    // org_id -> [id -> info]
    std::unordered_map<int64_t, std::unordered_map<int64_t, data::RequirementInfo::ptr>> m_org_datas;
};

typedef chen::Singleton<RequirementManager> RequirementMgr;

}

#endif // __BLOG_MANAGER_REQUIREMENT_MANAGER_H__
