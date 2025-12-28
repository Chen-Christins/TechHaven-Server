#include "chunk_upload.h"

ChunkUploadSession::ptr ChunkUploadManager::createSession(const std::string& uploadId
        , const std::string& fileName, size_t totalSize, size_t chunkSize, size_t totalChunks) {
    std::lock_guard<std::mutex> lock(mtx_);

    auto session = std::make_shared<ChunkUploadSession>();
    session->uploadId = uploadId;
    session->fileName = fileName;
    session->totalSize = totalSize;
    session->chunkSize = chunkSize;
    session->totalChunks = totalChunks;
    session->receivedChunks.resize(totalChunks, false);
    session->chunkData.resize(totalChunks);

    sessions_[uploadId] = session;
    return session;
}

ChunkUploadSession::ptr ChunkUploadManager::getSession(const std::string& uploadId) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = sessions_.find(uploadId);
    if (it != sessions_.end()) {
        return it->second;
    }
    return nullptr;
}

void ChunkUploadManager::removeSession(const std::string& uploadId) {
    std::lock_guard<std::mutex> lock(mtx_);
    sessions_.erase(uploadId);
}
