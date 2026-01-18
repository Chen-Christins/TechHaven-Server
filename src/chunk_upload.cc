#include "chunk_upload.h"

ChunkUploadSession::ptr ChunkUploadManager::createSession(const std::string& uploadId
        , const std::string& fileName, size_t totalSize, size_t chunkSize, size_t totalChunks
        , const std::string& bizType, int64_t bizId, const std::string& dirName) {
    std::lock_guard<std::shared_mutex> lock(mtx_);

    auto session = std::make_shared<ChunkUploadSession>();
    session->uploadId = uploadId;
    session->fileName = fileName;
    session->totalSize = totalSize;
    session->chunkSize = chunkSize;
    session->totalChunks = totalChunks;
    session->bizType = bizType;
    session->bizId = bizId;
    session->dirName = dirName;
    session->receivedChunks.resize(totalChunks, false);
    session->chunkData.resize(totalChunks);

    sessions_[uploadId] = session;
    return session;
}

ChunkUploadSession::ptr ChunkUploadManager::getSession(const std::string& uploadId) {
    std::shared_lock<std::shared_mutex> lock(mtx_);
    auto it = sessions_.find(uploadId);
    if (it != sessions_.end()) {
        return it->second;
    }
    return nullptr;
}

void ChunkUploadManager::removeSession(const std::string& uploadId) {
    std::lock_guard<std::shared_mutex> lock(mtx_);
    sessions_.erase(uploadId);
}
