#include "chunk_upload_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

bool ChunkUploadManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<data::ChunkUploadInfo::ptr> results;
    if (blog::data::ChunkUploadInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "ChunkUploadManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, data::ChunkUploadInfo::ptr> datas;
    for (auto& i : results) {
        datas[i->getId()] = i;
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    return true;
}

void ChunkUploadManager::add(data::ChunkUploadInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
}

data::ChunkUploadInfo::ptr ChunkUploadManager::get(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    return it != m_datas.end() ? it->second : nullptr;
}


}