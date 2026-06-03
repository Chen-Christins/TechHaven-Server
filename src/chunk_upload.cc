#include "chunk_upload.h"

ChunkUploadSession::ptr ChunkUploadManager::createSession(const std::string& upload_id
        , const std::string& file_name, size_t total_size, size_t chunk_size, size_t total_chunks
        , const std::string& biz_type, int64_t biz_id, const std::string& dir_name) {
    std::lock_guard<std::shared_mutex> lock(m_mtx);

    auto session = std::make_shared<ChunkUploadSession>();
    session->upload_id = upload_id;
    session->file_name = file_name;
    session->total_size = total_size;
    session->chunk_size = chunk_size;
    session->total_chunks = total_chunks;
    session->biz_type = biz_type;
    session->biz_id = biz_id;
    session->dir_name = dir_name;
    session->received_chunks.resize(total_chunks, false);
    session->chunk_data.resize(total_chunks);

    m_sessions[upload_id] = session;
    return session;
}

ChunkUploadSession::ptr ChunkUploadManager::getSession(const std::string& upload_id) {
    std::shared_lock<std::shared_mutex> lock(m_mtx);
    auto it = m_sessions.find(upload_id);
    if (it != m_sessions.end()) {
        return it->second;
    }
    return nullptr;
}

void ChunkUploadManager::removeSession(const std::string& upload_id) {
    std::lock_guard<std::shared_mutex> lock(m_mtx);
    m_sessions.erase(upload_id);
}
