#ifndef __BLOG_CHUNK_PARSER_H__
#define __BLOG_CHUNK_PARSER_H__

#include <string>
#include <vector>
#include <memory>

/**
 * @brief HTTP Chunked 数据解析器
 * @details 用于解析前端大文件上传时的 chunked 数据流
 *          ChunkParser 支持分块上传场景，按 index/offset 追加 chunk 并合并
 */
class ChunkParser {
public:
    typedef std::shared_ptr<ChunkParser> ptr;

    ChunkParser();
    // 添加一个分块
    // 参数: chunkIndex 分块索引, offset 偏移, data 数据
    void addChunk(size_t chunkIndex, size_t offset, const std::string& data);

    // 查询某个分块是否已收到
    bool hasChunk(size_t chunkIndex) const;

    // 获取所有已收到的分块索引
    std::vector<size_t> getReceivedChunks() const;

    // 合并所有分块，返回完整数据
    std::string mergeChunks() const;

    // 设置总分块数
    void setTotalChunks(size_t totalChunks);
    size_t getTotalChunks() const;

    // 查询是否全部分块已收到
    bool isComplete() const;

    // 重置解析器状态
    void reset();

private:
    // 总分块数
    size_t totalChunks_ = 0;
    // 分块数据存储
    std::vector<std::string> chunks_;
    // 分块接收状态
    std::vector<bool> received_;
};

#endif // __BLOG_CHUNK_PARSER_H__