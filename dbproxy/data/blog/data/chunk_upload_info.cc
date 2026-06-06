#include "chunk_upload_info.h"
#include "chen/log/log.h"
#include <map>

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

int ChunkUploadInfoDao::QueryByOwnerIdPages(std::vector<ChunkUploadInfo::ptr>& results, int64_t& total,  const int64_t& owner_id, int32_t offset, int32_t limit, chen::IDB::ptr conn) {
    std::string countSql = "select count(*) from chunk_upload where owner_id = ?";
    auto countStmt = conn->prepare(countSql);
    if (!countStmt) {
        ERROR(logger) << "stmt=" << countSql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    countStmt->bindInt64(1, owner_id);
    auto countRt = countStmt->query();
    if (!countRt) {
        return countStmt->getErrno();
    }
    if (countRt->next()) {
        total = countRt->getInt64(0);
    }
    if (total == 0) {
        return 0;
    }
    std::string sql = "select id, upload_id, filename, total_chunks, uploaded_chunks, size, owner_id, status, is_deleted, create_time, update_time from chunk_upload where owner_id = ? order by id desc limit ? offset ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, owner_id);
    stmt->bindInt32(2, limit);
    stmt->bindInt32(3, offset);
    auto rt = stmt->query();
    if (!rt) {
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
    std::map<std::string, std::string> existing_cols;  // name -> type
    while (data->next()) {
        existing_cols[data->getString(1)] = data->getString(2);
    }

    bool need_recreate = false;
    {
        auto it = existing_cols.find("id");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: chunk_upload.id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("upload_id");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: chunk_upload.upload_id " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("filename");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: chunk_upload.filename " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("total_chunks");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: chunk_upload.total_chunks " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("uploaded_chunks");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: chunk_upload.uploaded_chunks " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("size");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: chunk_upload.size " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("owner_id");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: chunk_upload.owner_id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("status");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: chunk_upload.status " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("is_deleted");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: chunk_upload.is_deleted " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("create_time");
        if (it != existing_cols.end() && it->second != "TIMESTAMP") {
            INFO(logger) << "Column type changed: chunk_upload.create_time " << it->second << " -> TIMESTAMP";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("update_time");
        if (it != existing_cols.end() && it->second != "TIMESTAMP") {
            INFO(logger) << "Column type changed: chunk_upload.update_time " << it->second << " -> TIMESTAMP";
            need_recreate = true;
        }
    }
    if (!need_recreate) {
        for (auto& [name, _] : existing_cols) {
            (void)_;  // suppress unused warning
            bool found = false;
            if (name == "id") found = true;
            if (name == "upload_id") found = true;
            if (name == "filename") found = true;
            if (name == "total_chunks") found = true;
            if (name == "uploaded_chunks") found = true;
            if (name == "size") found = true;
            if (name == "owner_id") found = true;
            if (name == "status") found = true;
            if (name == "is_deleted") found = true;
            if (name == "create_time") found = true;
            if (name == "update_time") found = true;
            if (!found) {
                need_recreate = true;
                WARN(logger) << "Column chunk_upload." << name << " removed, table recreate required";
                break;
            }
        }
    }

    if (need_recreate) {
        INFO(logger) << "Recreating table chunk_upload";

        std::vector<std::string> common_cols;
        if (existing_cols.find("id") != existing_cols.end()) {
            common_cols.push_back("id");
        }
        if (existing_cols.find("upload_id") != existing_cols.end()) {
            common_cols.push_back("upload_id");
        }
        if (existing_cols.find("filename") != existing_cols.end()) {
            common_cols.push_back("filename");
        }
        if (existing_cols.find("total_chunks") != existing_cols.end()) {
            common_cols.push_back("total_chunks");
        }
        if (existing_cols.find("uploaded_chunks") != existing_cols.end()) {
            common_cols.push_back("uploaded_chunks");
        }
        if (existing_cols.find("size") != existing_cols.end()) {
            common_cols.push_back("size");
        }
        if (existing_cols.find("owner_id") != existing_cols.end()) {
            common_cols.push_back("owner_id");
        }
        if (existing_cols.find("status") != existing_cols.end()) {
            common_cols.push_back("status");
        }
        if (existing_cols.find("is_deleted") != existing_cols.end()) {
            common_cols.push_back("is_deleted");
        }
        if (existing_cols.find("create_time") != existing_cols.end()) {
            common_cols.push_back("create_time");
        }
        if (existing_cols.find("update_time") != existing_cols.end()) {
            common_cols.push_back("update_time");
        }

        if (conn->execute("ALTER TABLE chunk_upload RENAME TO chunk_upload_tmp")) {
            ERROR(logger) << "RENAME TABLE chunk_upload failed";
            return conn->getErrno();
        }
        CreateTableSQLite3(conn);
        if (!common_cols.empty()) {
            std::string cols;
            for (size_t i = 0; i < common_cols.size(); ++i) {
                if (i) cols += ",";
                cols += common_cols[i];
            }
            std::string sql = "INSERT INTO chunk_upload (" + cols + ") SELECT " + cols + " FROM chunk_upload_tmp";
            if (int rt = conn->execute(sql)) {
                ERROR(logger) << "copy data from chunk_upload_tmp to chunk_upload failed, errno=" << rt;
                // don't return; try to continue
            }
        }
        conn->execute("DROP TABLE chunk_upload_tmp");
        return 0;
    }

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

    return 0;
}

int ChunkUploadInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM chunk_upload");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM chunk_upload errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::map<std::string, std::string> existing_cols;  // name -> type
    while (data->next()) {
        existing_cols[data->getString(0)] = data->getString(1);
    }

    {
        auto it = existing_cols.find("id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column chunk_upload.id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE chunk_upload MODIFY COLUMN `id` bigint NOT NULL DEFAULT 0 COMMENT '主键ID'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN chunk_upload.id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("upload_id");
        if (it != existing_cols.end() && it->second != "varchar(64)") {
            INFO(logger) << "Modifying column chunk_upload.upload_id " << it->second << " -> varchar(64)";
            int rt = conn->execute("ALTER TABLE chunk_upload MODIFY COLUMN `upload_id` varchar(64) NOT NULL DEFAULT '' COMMENT '上传任务唯一ID'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN chunk_upload.upload_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("filename");
        if (it != existing_cols.end() && it->second != "varchar(256)") {
            INFO(logger) << "Modifying column chunk_upload.filename " << it->second << " -> varchar(256)";
            int rt = conn->execute("ALTER TABLE chunk_upload MODIFY COLUMN `filename` varchar(256) NOT NULL DEFAULT '' COMMENT '原始文件名'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN chunk_upload.filename failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("total_chunks");
        if (it != existing_cols.end() && it->second != "int") {
            INFO(logger) << "Modifying column chunk_upload.total_chunks " << it->second << " -> int";
            int rt = conn->execute("ALTER TABLE chunk_upload MODIFY COLUMN `total_chunks` int NOT NULL DEFAULT 0 COMMENT '总分片数'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN chunk_upload.total_chunks failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("uploaded_chunks");
        if (it != existing_cols.end() && it->second != "int") {
            INFO(logger) << "Modifying column chunk_upload.uploaded_chunks " << it->second << " -> int";
            int rt = conn->execute("ALTER TABLE chunk_upload MODIFY COLUMN `uploaded_chunks` int NOT NULL DEFAULT 0 COMMENT '已上传分片数'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN chunk_upload.uploaded_chunks failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("size");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column chunk_upload.size " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE chunk_upload MODIFY COLUMN `size` bigint NOT NULL DEFAULT 0 COMMENT '文件总大小（字节）'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN chunk_upload.size failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("owner_id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column chunk_upload.owner_id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE chunk_upload MODIFY COLUMN `owner_id` bigint NOT NULL DEFAULT 0 COMMENT '上传者用户ID'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN chunk_upload.owner_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("status");
        if (it != existing_cols.end() && it->second != "int") {
            INFO(logger) << "Modifying column chunk_upload.status " << it->second << " -> int";
            int rt = conn->execute("ALTER TABLE chunk_upload MODIFY COLUMN `status` int NOT NULL DEFAULT 1 COMMENT '状态: 1上传中 2已完成 3失败'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN chunk_upload.status failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("is_deleted");
        if (it != existing_cols.end() && it->second != "int") {
            INFO(logger) << "Modifying column chunk_upload.is_deleted " << it->second << " -> int";
            int rt = conn->execute("ALTER TABLE chunk_upload MODIFY COLUMN `is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN chunk_upload.is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("create_time");
        if (it != existing_cols.end() && it->second != "timestamp") {
            INFO(logger) << "Modifying column chunk_upload.create_time " << it->second << " -> timestamp";
            int rt = conn->execute("ALTER TABLE chunk_upload MODIFY COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN chunk_upload.create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("update_time");
        if (it != existing_cols.end() && it->second != "timestamp") {
            INFO(logger) << "Modifying column chunk_upload.update_time " << it->second << " -> timestamp";
            int rt = conn->execute("ALTER TABLE chunk_upload MODIFY COLUMN `update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '更新时间'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN chunk_upload.update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    for (auto& [name, _] : existing_cols) {
        (void)_;
        bool found = false;
        if (name == "id") found = true;
        if (name == "upload_id") found = true;
        if (name == "filename") found = true;
        if (name == "total_chunks") found = true;
        if (name == "uploaded_chunks") found = true;
        if (name == "size") found = true;
        if (name == "owner_id") found = true;
        if (name == "status") found = true;
        if (name == "is_deleted") found = true;
        if (name == "create_time") found = true;
        if (name == "update_time") found = true;
        if (!found) {
            WARN(logger) << "Dropping column chunk_upload." << name << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE chunk_upload DROP COLUMN `" + name + "`");
            if (rt) {
                ERROR(logger) << "DROP COLUMN chunk_upload." << name << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

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

    return 0;
}


} //namespace data
} //namespace blog
