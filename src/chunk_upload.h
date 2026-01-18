#ifndef __BLOG_CHUNK_UPLOAD_H__
#define __BLOG_CHUNK_UPLOAD_H__

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <memory>
#include <chen/singleton.h>

/**
 * @brief 管理每个上传会话的状态
 */
struct ChunkUploadSession {
    typedef std::shared_ptr<ChunkUploadSession> ptr;
    // 会话唯一标识
    std::string uploadId;
    // 文件名
    std::string fileName;
    // 文件总大小
    size_t totalSize = 0;
    // 每个分块大小
    size_t chunkSize = 0;
    // 总分块数
    size_t totalChunks = 0;
    // 已接收的分块状态
    std::vector<bool> receivedChunks;
    // 可选：已接收的分块数据
    std::vector<std::string> chunkData;
    // 已接收分块数量
    size_t receivedCount = 0;
    // 是否完成上传
    bool completed = false;
    // 业务类型
    std::string bizType;
    // 业务ID
    int64_t bizId = 0;
    // 存储目录
    std::string dirName;
    // 互斥锁，保护会话数据
    std::mutex mtx;
};

/**
 * @brief 管理所有会话
 */
class ChunkUploadManager {
public:
    /**
     * @brief 创建一个新的上传会话
     * @param uploadId 会话唯一标识
     * @param fileName 文件名
     * @param totalSize 文件总大小
     * @param chunkSize 每个分块大小
     * @param totalChunks 总分块数
     * @return ChunkUploadSession::ptr 
     */
    ChunkUploadSession::ptr createSession(const std::string& uploadId
        , const std::string& fileName, size_t totalSize, size_t chunkSize, size_t totalChunks
        , const std::string& bizType, int64_t bizId, const std::string& dirName);

    /**
     * @brief 获取上传会话
     * @param uploadId 会话唯一标识
     * @return ChunkUploadSession::ptr 
     */
    ChunkUploadSession::ptr getSession(const std::string& uploadId);

    /**
     * @brief 移除上传会话
     * @param uploadId 会话唯一标识
     */
    void removeSession(const std::string& uploadId);
private:
    // 存储所有上传会话
    std::unordered_map<std::string, ChunkUploadSession::ptr> sessions_;
    // 保护 sessions_ 的读写锁
    std::shared_mutex mtx_;
};

typedef chen::Singleton<ChunkUploadManager> ChunkUploadMgr;

#endif // __BLOG_CHUNK_UPLOAD_H__