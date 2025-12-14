#ifndef __BLOG_MANAGER_RESOURCE_MANAGER_H__
#define __BLOG_MANAGER_RESOURCE_MANAGER_H__

#include <unordered_map>
#include "blog/data/resource_info.h"
#include <chen/singleton.h>
#include <shared_mutex>

namespace blog {

class ResourceManager {
public:
    enum ResourceType {
        // 图片
        TYPE_IMAGE = 1,
        // 视频
        TYPE_VIDEO = 2,
        // 文档
        TYPE_DOCUMENT = 3,
        // 压缩包
        TYPE_COMPRESSED = 4,
        // 音频
        TYPE_AUDIO = 5,
        // 其他
        TYPE_OTHER = 6,
    };

    ResourceType GetResourceType(const std::string& filename);

    bool loadAll();

    void add(blog::data::ResourceInfo::ptr info);
private:
    std::shared_mutex m_mutex;
    // id -> 资源信息
    std::unordered_map<int64_t, data::ResourceInfo::ptr> m_datas;
};

typedef chen::Singleton<ResourceManager> ResourceMgr;

}

#endif // __BLOG_MANAGER_RESOURCE_MANAGER_H__