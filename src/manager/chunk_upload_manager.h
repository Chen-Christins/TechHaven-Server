#ifndef __BLOG_MANAGER_CHUNK_UPLOAD_MANAGER_H__
#define __BLOG_MANAGER_CHUNK_UPLOAD_MANAGER_H__

#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <chen/singleton.h>
#include "blog/data/chunk_upload_info.h"

namespace blog {

class ChunkUploadManager {
public:
    typedef std::shared_ptr<ChunkUploadManager> ptr;

    bool loadAll();
    void add(data::ChunkUploadInfo::ptr info);
    data::ChunkUploadInfo::ptr get(int64_t id);

private:
    std::shared_mutex m_mutex;
    // id -> ChunkUploadInfo
    std::unordered_map<int64_t, data::ChunkUploadInfo::ptr> m_datas;
};

typedef chen::Singleton<ChunkUploadManager> ChunkUploadMgr;

}

#endif // __BLOG_MANAGER_CHUNK_UPLOAD_MANAGER_H__