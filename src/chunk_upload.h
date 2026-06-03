/**
 * @file chunk_upload.h
 * @brief 分块上传会话管理
 * @author Christins
 * @date 2026-06-03
 * @copyright Apache 2.0
 */

#pragma once

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include <chen/singleton.h>

/**
 * @brief 管理每个上传会话的状态
 */
struct ChunkUploadSession {
    typedef std::shared_ptr<ChunkUploadSession> ptr;
    // 会话唯一标识
    std::string upload_id;
    // 文件名
    std::string file_name;
    // 文件总大小
    size_t total_size = 0;
    // 每个分块大小
    size_t chunk_size = 0;
    // 总分块数
    size_t total_chunks = 0;
    // 已接收的分块状态
    std::vector<bool> received_chunks;
    // 可选：已接收的分块数据
    std::vector<std::string> chunk_data;
    // 已接收分块数量
    size_t received_count = 0;
    // 是否完成上传
    bool completed = false;
    // 业务类型
    std::string biz_type;
    // 业务ID
    int64_t biz_id = 0;
    // 存储目录
    std::string dir_name;
    // 互斥锁，保护会话数据
    std::mutex m_mtx;
};

/**
 * @brief 管理所有会话
 */
class ChunkUploadManager {
public:
    /**
     * @brief 创建一个新的上传会话
     * @param upload_id 会话唯一标识
     * @param file_name 文件名
     * @param total_size 文件总大小
     * @param chunk_size 每个分块大小
     * @param total_chunks 总分块数
     * @return ChunkUploadSession::ptr
     */
    ChunkUploadSession::ptr createSession(const std::string& upload_id
        , const std::string& file_name, size_t total_size, size_t chunk_size, size_t total_chunks
        , const std::string& biz_type, int64_t biz_id, const std::string& dir_name);

    /**
     * @brief 获取上传会话
     * @param upload_id 会话唯一标识
     * @return ChunkUploadSession::ptr
     */
    ChunkUploadSession::ptr getSession(const std::string& upload_id);

    /**
     * @brief 移除上传会话
     * @param upload_id 会话唯一标识
     */
    void removeSession(const std::string& upload_id);
private:
    // 存储所有上传会话
    std::unordered_map<std::string, ChunkUploadSession::ptr> m_sessions;
    // 保护 m_sessions 的读写锁
    std::shared_mutex m_mtx;
};

typedef chen::Singleton<ChunkUploadManager> ChunkUploadMgr;

