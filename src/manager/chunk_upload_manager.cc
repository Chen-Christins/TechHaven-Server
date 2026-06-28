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
    data::ChunkUploadInfo::ptr v(new data::ChunkUploadInfo);
    v->setId(rt->getInt64(0));
    v->setUploadId(rt->getString(1));
    v->setFilename(rt->getString(2));
    v->setTotalChunks(rt->getInt32(3));
    v->setUploadedChunks(rt->getInt32(4));
    v->setSize(rt->getInt64(5));
    v->setOwnerId(rt->getInt64(6));
    v->setStatus(rt->getInt32(7));
    v->setIsDeleted(rt->getInt32(8));
    v->setCreateTime(rt->getTime(9));
    v->setUpdateTime(rt->getTime(10));
    return v;
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
