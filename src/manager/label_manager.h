#ifndef __BLOG_MANAGER_LABEL_MANAGER_H__
#define __BLOG_MANAGER_LABEL_MANAGER_H__

#include <shared_mutex>
#include <unordered_map>
#include "blog/data/label_info.h"
#include <chen/singleton.h>

namespace blog {

class LabelManager {
public:
    bool loadAll();
    void add(data::LabelInfo::ptr info);
    data::LabelInfo::ptr get(int64_t id);
    data::LabelInfo::ptr getByUserIdName(int64_t id, const std::string& name);
    bool listByUserId(std::vector<data::LabelInfo::ptr>& infos, int64_t id, bool valid);
private:
    std::shared_mutex m_mutex;
    std::unordered_map<int64_t, data::LabelInfo::ptr> m_datas;
    std::unordered_map<int64_t, std::map<std::string, data::LabelInfo::ptr>> m_users;
};

typedef chen::Singleton<LabelManager> LabelMgr;

}

#endif // __BLOG_MANAGER_LABEL_MANAGER_H__