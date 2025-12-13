#ifndef __BLOG_MANAGER_RESOURCE_MANAGER_H__
#define __BLOG_MANAGER_RESOURCE_MANAGER_H__

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
};

#endif // __BLOG_MANAGER_RESOURCE_MANAGER_H__