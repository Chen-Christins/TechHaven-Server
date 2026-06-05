#pragma once

#include <shared_mutex>
#include <chen/ds/lru_cache.h>
#include <chen/db/query_builder.h>
#include <chen/singleton.h>
#include "blog/data/chunk_upload_info.h"

namespace blog {

class ChunkUploadManager {
public:
    typedef std::shared_ptr<ChunkUploadManager> ptr;

    ChunkUploadManager();

    void add(data::ChunkUploadInfo::ptr info);
    data::ChunkUploadInfo::ptr get(int64_t id);

private:
    static data::ChunkUploadInfo::ptr parseRow(chen::ISQLData::ptr rt);

    std::shared_mutex m_mutex;
    chen::ds::LruCache<int64_t, data::ChunkUploadInfo::ptr> m_cache;
};

typedef chen::Singleton<ChunkUploadManager> ChunkUploadMgr;

}
