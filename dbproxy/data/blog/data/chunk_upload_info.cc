#include "chunk_upload_info.h"
#include "chen/log/log.h"

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

ChunkUploadInfo::ChunkUploadInfo()
    :m_totalChunks()
    ,m_uploadedChunks()
    ,m_status(1)
    ,m_isDeleted(0)
    ,m_id()
    ,m_size()
    ,m_ownerId()
    ,m_uploadId()
    ,m_filename()
    ,m_createTime(time(0))
    ,m_updateTime() {
}

std::string ChunkUploadInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["upload_id"] = m_uploadId;
    v["filename"] = m_filename;
    v["total_chunks"] = m_totalChunks;
    v["uploaded_chunks"] = m_uploadedChunks;
    v["size"] = std::to_string(m_size);
    v["owner_id"] = std::to_string(m_ownerId);
    v["status"] = m_status;
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void ChunkUploadInfo::setId(const int64_t& v) {
    m_id = v;
}

void ChunkUploadInfo::setUploadId(const std::string& v) {
    m_uploadId = v;
}

void ChunkUploadInfo::setFilename(const std::string& v) {
    m_filename = v;
}

void ChunkUploadInfo::setTotalChunks(const int32_t& v) {
    m_totalChunks = v;
}

void ChunkUploadInfo::setUploadedChunks(const int32_t& v) {
    m_uploadedChunks = v;
}

void ChunkUploadInfo::setSize(const int64_t& v) {
    m_size = v;
}

void ChunkUploadInfo::setOwnerId(const int64_t& v) {
    m_ownerId = v;
}

void ChunkUploadInfo::setStatus(const int32_t& v) {
    m_status = v;
}

void ChunkUploadInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void ChunkUploadInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void ChunkUploadInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int ChunkUploadInfoDao::Update(ChunkUploadInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update chunk_upload set upload_id = ?, filename = ?, total_chunks = ?, uploaded_chunks = ?, size = ?, owner_id = ?, status = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_uploadId);
    stmt->bindString(2, info->m_filename);
    stmt->bindInt32(3, info->m_totalChunks);
    stmt->bindInt32(4, info->m_uploadedChunks);
    stmt->bindInt64(5, info->m_size);
    stmt->bindInt64(6, info->m_ownerId);
    stmt->bindInt32(7, info->m_status);
    stmt->bindInt32(8, info->m_isDeleted);
    stmt->bindTime(9, info->m_createTime);
    stmt->bindTime(10, info->m_updateTime);
    stmt->bindInt64(11, info->m_id);
    return stmt->execute();
}

int ChunkUploadInfoDao::Insert(ChunkUploadInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into chunk_upload (upload_id, filename, total_chunks, uploaded_chunks, size, owner_id, status, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_uploadId);
    stmt->bindString(2, info->m_filename);
    stmt->bindInt32(3, info->m_totalChunks);
    stmt->bindInt32(4, info->m_uploadedChunks);
    stmt->bindInt64(5, info->m_size);
    stmt->bindInt64(6, info->m_ownerId);
    stmt->bindInt32(7, info->m_status);
    stmt->bindInt32(8, info->m_isDeleted);
    stmt->bindTime(9, info->m_createTime);
    stmt->bindTime(10, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int ChunkUploadInfoDao::InsertOrUpdate(ChunkUploadInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into chunk_upload (id, upload_id, filename, total_chunks, uploaded_chunks, size, owner_id, status, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindString(2, info->m_uploadId);
    stmt->bindString(3, info->m_filename);
    stmt->bindInt32(4, info->m_totalChunks);
    stmt->bindInt32(5, info->m_uploadedChunks);
    stmt->bindInt64(6, info->m_size);
    stmt->bindInt64(7, info->m_ownerId);
    stmt->bindInt32(8, info->m_status);
    stmt->bindInt32(9, info->m_isDeleted);
    stmt->bindTime(10, info->m_createTime);
    stmt->bindTime(11, info->m_updateTime);
    return stmt->execute();
}

int ChunkUploadInfoDao::Delete(ChunkUploadInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from chunk_upload where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int ChunkUploadInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from chunk_upload where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int ChunkUploadInfoDao::DeleteByUploadId( const std::string& upload_id, chen::IDB::ptr conn) {
    std::string sql = "delete from chunk_upload where upload_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, upload_id);
    return stmt->execute();
}

int ChunkUploadInfoDao::DeleteByOwnerId( const int64_t& owner_id, chen::IDB::ptr conn) {
    std::string sql = "delete from chunk_upload where owner_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, owner_id);
    return stmt->execute();
}

int ChunkUploadInfoDao::QueryAll(std::vector<ChunkUploadInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, upload_id, filename, total_chunks, uploaded_chunks, size, owner_id, status, is_deleted, create_time, update_time from chunk_upload";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    auto rt = stmt->query();
    if(!rt) {
        return stmt->getErrno();
    }
    while (rt->next()) {
        ChunkUploadInfo::ptr v(new ChunkUploadInfo);
        v->m_id = rt->getInt64(0);
        v->m_uploadId = rt->getString(1);
        v->m_filename = rt->getString(2);
        v->m_totalChunks = rt->getInt32(3);
        v->m_uploadedChunks = rt->getInt32(4);
        v->m_size = rt->getInt64(5);
        v->m_ownerId = rt->getInt64(6);
        v->m_status = rt->getInt32(7);
        v->m_isDeleted = rt->getInt32(8);
        v->m_createTime = rt->getTime(9);
        v->m_updateTime = rt->getTime(10);
        results.push_back(v);
    }
    return 0;
}

ChunkUploadInfo::ptr ChunkUploadInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, upload_id, filename, total_chunks, uploaded_chunks, size, owner_id, status, is_deleted, create_time, update_time from chunk_upload where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindInt64(1, id);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    ChunkUploadInfo::ptr v(new ChunkUploadInfo);
    v->m_id = rt->getInt64(0);
    v->m_uploadId = rt->getString(1);
    v->m_filename = rt->getString(2);
    v->m_totalChunks = rt->getInt32(3);
    v->m_uploadedChunks = rt->getInt32(4);
    v->m_size = rt->getInt64(5);
    v->m_ownerId = rt->getInt64(6);
    v->m_status = rt->getInt32(7);
    v->m_isDeleted = rt->getInt32(8);
    v->m_createTime = rt->getTime(9);
    v->m_updateTime = rt->getTime(10);
    return v;
}

ChunkUploadInfo::ptr ChunkUploadInfoDao::QueryByUploadId( const std::string& upload_id, chen::IDB::ptr conn) {
    std::string sql = "select id, upload_id, filename, total_chunks, uploaded_chunks, size, owner_id, status, is_deleted, create_time, update_time from chunk_upload where upload_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindString(1, upload_id);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    ChunkUploadInfo::ptr v(new ChunkUploadInfo);
    v->m_id = rt->getInt64(0);
    v->m_uploadId = rt->getString(1);
    v->m_filename = rt->getString(2);
    v->m_totalChunks = rt->getInt32(3);
    v->m_uploadedChunks = rt->getInt32(4);
    v->m_size = rt->getInt64(5);
    v->m_ownerId = rt->getInt64(6);
    v->m_status = rt->getInt32(7);
    v->m_isDeleted = rt->getInt32(8);
    v->m_createTime = rt->getTime(9);
    v->m_updateTime = rt->getTime(10);
    return v;
}

int ChunkUploadInfoDao::QueryByOwnerId(std::vector<ChunkUploadInfo::ptr>& results,  const int64_t& owner_id, chen::IDB::ptr conn) {
    std::string sql = "select id, upload_id, filename, total_chunks, uploaded_chunks, size, owner_id, status, is_deleted, create_time, update_time from chunk_upload where owner_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, owner_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        ChunkUploadInfo::ptr v(new ChunkUploadInfo);
        v->m_id = rt->getInt64(0);
        v->m_uploadId = rt->getString(1);
        v->m_filename = rt->getString(2);
        v->m_totalChunks = rt->getInt32(3);
        v->m_uploadedChunks = rt->getInt32(4);
        v->m_size = rt->getInt64(5);
        v->m_ownerId = rt->getInt64(6);
        v->m_status = rt->getInt32(7);
        v->m_isDeleted = rt->getInt32(8);
        v->m_createTime = rt->getTime(9);
        v->m_updateTime = rt->getTime(10);
        results.push_back(v);
    };
    return 0;
}

int ChunkUploadInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE chunk_upload("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "upload_id TEXT NOT NULL DEFAULT '',"
            "filename TEXT NOT NULL DEFAULT '',"
            "total_chunks INTEGER NOT NULL DEFAULT 0,"
            "uploaded_chunks INTEGER NOT NULL DEFAULT 0,"
            "size INTEGER NOT NULL DEFAULT 0,"
            "owner_id INTEGER NOT NULL DEFAULT 0,"
            "status INTEGER NOT NULL DEFAULT 1,"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE UNIQUE INDEX chunk_upload_upload_id ON chunk_upload(upload_id);"
            "CREATE INDEX chunk_upload_owner_id ON chunk_upload(owner_id);"
            );
}

int ChunkUploadInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE chunk_upload("
            "`id` bigint AUTO_INCREMENT COMMENT '主键ID',"
            "`upload_id` varchar(64) NOT NULL DEFAULT '' COMMENT '上传任务唯一ID',"
            "`filename` varchar(256) NOT NULL DEFAULT '' COMMENT '原始文件名',"
            "`total_chunks` int NOT NULL DEFAULT 0 COMMENT '总分片数',"
            "`uploaded_chunks` int NOT NULL DEFAULT 0 COMMENT '已上传分片数',"
            "`size` bigint NOT NULL DEFAULT 0 COMMENT '文件总大小（字节）',"
            "`owner_id` bigint NOT NULL DEFAULT 0 COMMENT '上传者用户ID',"
            "`status` int NOT NULL DEFAULT 1 COMMENT '状态: 1上传中 2已完成 3失败',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "UNIQUE KEY `chunk_upload_upload_id` (`upload_id`),"
            "KEY `chunk_upload_owner_id` (`owner_id`)) COMMENT='分片上传任务表'");
}
} //namespace data
} //namespace blog
