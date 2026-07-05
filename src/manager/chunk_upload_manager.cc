#include "chunk_upload_manager.h"

#include <chen/log/log.h>

#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 200;

ChunkUploadManager::ChunkUploadManager()
    :m_cache(4, kCacheMaxSize, 0) {
}

data::ChunkUploadInfo::ptr ChunkUploadManager::parseRow(chen::ISQLData::ptr rt) {
    return data::ChunkUploadInfoDao::ParseRow(rt);
}


void ChunkUploadManager::add(data::ChunkUploadInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

data::ChunkUploadInfo::ptr ChunkUploadManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::ChunkUploadInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

}
