#include "chunk_upload_info.h"
#include "chen/log/log.h"
#include <set>

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
    return conn->execute("CREATE TABLE IF NOT EXISTS chunk_upload("
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
            "CREATE UNIQUE INDEX IF NOT EXISTS chunk_upload_upload_id ON chunk_upload(upload_id);"
            "CREATE INDEX IF NOT EXISTS chunk_upload_owner_id ON chunk_upload(owner_id);"
            );
}

int ChunkUploadInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS chunk_upload("
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

int ChunkUploadInfoDao::MigrateTableSQLite3(chen::IDB::ptr conn) {
    auto data = conn->query("PRAGMA table_info(chunk_upload)");
    if (!data) {
        ERROR(logger) << "PRAGMA table_info(chunk_upload) errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(1));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("upload_id");
    expected_cols.insert("filename");
    expected_cols.insert("total_chunks");
    expected_cols.insert("uploaded_chunks");
    expected_cols.insert("size");
    expected_cols.insert("owner_id");
    expected_cols.insert("status");
    expected_cols.insert("is_deleted");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("upload_id") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.upload_id";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN upload_id TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN upload_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("filename") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.filename";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN filename TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN filename failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("total_chunks") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.total_chunks";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN total_chunks INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN total_chunks failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("uploaded_chunks") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.uploaded_chunks";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN uploaded_chunks INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN uploaded_chunks failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("size") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.size";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN size INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN size failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("owner_id") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.owner_id";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN owner_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN owner_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("status") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.status";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN status INTEGER NOT NULL DEFAULT 1");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN status failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.is_deleted";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.create_time";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN create_time TIMESTAMP NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.update_time";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column chunk_upload." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE chunk_upload DROP COLUMN " + col);
            if (rt) {
                ERROR(logger) << "ALTER TABLE chunk_upload DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}

int ChunkUploadInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM chunk_upload");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM chunk_upload errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(0));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("upload_id");
    expected_cols.insert("filename");
    expected_cols.insert("total_chunks");
    expected_cols.insert("uploaded_chunks");
    expected_cols.insert("size");
    expected_cols.insert("owner_id");
    expected_cols.insert("status");
    expected_cols.insert("is_deleted");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("upload_id") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.upload_id";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN `upload_id` varchar(64) NOT NULL DEFAULT '' COMMENT '上传任务唯一ID'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN upload_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("filename") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.filename";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN `filename` varchar(256) NOT NULL DEFAULT '' COMMENT '原始文件名'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN filename failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("total_chunks") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.total_chunks";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN `total_chunks` int NOT NULL DEFAULT 0 COMMENT '总分片数'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN total_chunks failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("uploaded_chunks") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.uploaded_chunks";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN `uploaded_chunks` int NOT NULL DEFAULT 0 COMMENT '已上传分片数'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN uploaded_chunks failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("size") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.size";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN `size` bigint NOT NULL DEFAULT 0 COMMENT '文件总大小（字节）'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN size failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("owner_id") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.owner_id";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN `owner_id` bigint NOT NULL DEFAULT 0 COMMENT '上传者用户ID'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN owner_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("status") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.status";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN `status` int NOT NULL DEFAULT 1 COMMENT '状态: 1上传中 2已完成 3失败'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN status failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.is_deleted";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN `is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.create_time";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column chunk_upload.update_time";
        int rt = conn->execute("ALTER TABLE chunk_upload ADD COLUMN `update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '更新时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE chunk_upload ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column chunk_upload." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE chunk_upload DROP COLUMN `" + col + "`");
            if (rt) {
                ERROR(logger) << "ALTER TABLE chunk_upload DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}


} //namespace data
} //namespace blog
