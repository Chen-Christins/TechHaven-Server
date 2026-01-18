#ifndef BLOG_DATACHUNK_UPLOAD_INFO_H
#define BLOG_DATACHUNK_UPLOAD_INFO_H

#include <json/json.h>
#include <vector>
#include "chen/db/db.h"
#include "chen/util/util.h"


namespace blog {
namespace data {

class ChunkUploadInfoDao;
class ChunkUploadInfo {
friend class ChunkUploadInfoDao;
public:
    typedef std::shared_ptr<ChunkUploadInfo> ptr;

    ChunkUploadInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const std::string& getUploadId() { return m_uploadId; }
    void setUploadId(const std::string& v);

    const std::string& getFilename() { return m_filename; }
    void setFilename(const std::string& v);

    const int32_t& getTotalChunks() { return m_totalChunks; }
    void setTotalChunks(const int32_t& v);

    const int32_t& getUploadedChunks() { return m_uploadedChunks; }
    void setUploadedChunks(const int32_t& v);

    const int64_t& getSize() { return m_size; }
    void setSize(const int64_t& v);

    const int64_t& getOwnerId() { return m_ownerId; }
    void setOwnerId(const int64_t& v);

    const int32_t& getStatus() { return m_status; }
    void setStatus(const int32_t& v);

    const int32_t& getIsDeleted() { return m_isDeleted; }
    void setIsDeleted(const int32_t& v);

    const int64_t& getCreateTime() { return m_createTime; }
    void setCreateTime(const int64_t& v);

    const int64_t& getUpdateTime() { return m_updateTime; }
    void setUpdateTime(const int64_t& v);

    std::string toJsonString() const;

private:
    int32_t m_totalChunks;
    int32_t m_uploadedChunks;
    int32_t m_status;
    int32_t m_isDeleted;
    int64_t m_id;
    int64_t m_size;
    int64_t m_ownerId;
    std::string m_uploadId;
    std::string m_filename;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class ChunkUploadInfoDao {
public:
    typedef std::shared_ptr<ChunkUploadInfoDao> ptr;
    static int Update(ChunkUploadInfo::ptr info, chen::IDB::ptr conn);
    static int Insert(ChunkUploadInfo::ptr info, chen::IDB::ptr conn);
    static int InsertOrUpdate(ChunkUploadInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(ChunkUploadInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(const int64_t& id, chen::IDB::ptr conn);
    static int DeleteById( const int64_t& id, chen::IDB::ptr conn);
    static int DeleteByUploadId( const std::string& upload_id, chen::IDB::ptr conn);
    static int DeleteByOwnerId( const int64_t& owner_id, chen::IDB::ptr conn);
    static int QueryAll(std::vector<ChunkUploadInfo::ptr>& results, chen::IDB::ptr conn);
    static ChunkUploadInfo::ptr Query( const int64_t& id, chen::IDB::ptr conn);
    static ChunkUploadInfo::ptr QueryByUploadId( const std::string& upload_id, chen::IDB::ptr conn);
    static int QueryByOwnerId(std::vector<ChunkUploadInfo::ptr>& results,  const int64_t& owner_id, chen::IDB::ptr conn);
    static int CreateTableSQLite3(chen::IDB::ptr info);
    static int CreateTableMySQL(chen::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //BLOG_DATACHUNK_UPLOAD_INFO_H
