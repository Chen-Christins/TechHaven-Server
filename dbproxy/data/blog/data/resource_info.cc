#include "resource_info.h"
#include "chen/log/log.h"
#include <set>

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

ResourceInfo::ResourceInfo()
    :m_type()
    ,m_status(1)
    ,m_isDeleted(0)
    ,m_id()
    ,m_size()
    ,m_ownerId()
    ,m_bizId()
    ,m_name()
    ,m_path()
    ,m_hash()
    ,m_bizType()
    ,m_createTime(time(0))
    ,m_updateTime() {
}

std::string ResourceInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["name"] = m_name;
    v["path"] = m_path;
    v["type"] = m_type;
    v["size"] = std::to_string(m_size);
    v["hash"] = m_hash;
    v["owner_id"] = std::to_string(m_ownerId);
    v["biz_type"] = m_bizType;
    v["biz_id"] = std::to_string(m_bizId);
    v["status"] = m_status;
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void ResourceInfo::setId(const int64_t& v) {
    m_id = v;
}

void ResourceInfo::setName(const std::string& v) {
    m_name = v;
}

void ResourceInfo::setPath(const std::string& v) {
    m_path = v;
}

void ResourceInfo::setType(const int32_t& v) {
    m_type = v;
}

void ResourceInfo::setSize(const int64_t& v) {
    m_size = v;
}

void ResourceInfo::setHash(const std::string& v) {
    m_hash = v;
}

void ResourceInfo::setOwnerId(const int64_t& v) {
    m_ownerId = v;
}

void ResourceInfo::setBizType(const std::string& v) {
    m_bizType = v;
}

void ResourceInfo::setBizId(const int64_t& v) {
    m_bizId = v;
}

void ResourceInfo::setStatus(const int32_t& v) {
    m_status = v;
}

void ResourceInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void ResourceInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void ResourceInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int ResourceInfoDao::Update(ResourceInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update resource set name = ?, path = ?, type = ?, size = ?, hash = ?, owner_id = ?, biz_type = ?, biz_id = ?, status = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_name);
    stmt->bindString(2, info->m_path);
    stmt->bindInt32(3, info->m_type);
    stmt->bindInt64(4, info->m_size);
    stmt->bindString(5, info->m_hash);
    stmt->bindInt64(6, info->m_ownerId);
    stmt->bindString(7, info->m_bizType);
    stmt->bindInt64(8, info->m_bizId);
    stmt->bindInt32(9, info->m_status);
    stmt->bindInt32(10, info->m_isDeleted);
    stmt->bindTime(11, info->m_createTime);
    stmt->bindTime(12, info->m_updateTime);
    stmt->bindInt64(13, info->m_id);
    return stmt->execute();
}

int ResourceInfoDao::Insert(ResourceInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into resource (name, path, type, size, hash, owner_id, biz_type, biz_id, status, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_name);
    stmt->bindString(2, info->m_path);
    stmt->bindInt32(3, info->m_type);
    stmt->bindInt64(4, info->m_size);
    stmt->bindString(5, info->m_hash);
    stmt->bindInt64(6, info->m_ownerId);
    stmt->bindString(7, info->m_bizType);
    stmt->bindInt64(8, info->m_bizId);
    stmt->bindInt32(9, info->m_status);
    stmt->bindInt32(10, info->m_isDeleted);
    stmt->bindTime(11, info->m_createTime);
    stmt->bindTime(12, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int ResourceInfoDao::InsertOrUpdate(ResourceInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into resource (id, name, path, type, size, hash, owner_id, biz_type, biz_id, status, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindString(2, info->m_name);
    stmt->bindString(3, info->m_path);
    stmt->bindInt32(4, info->m_type);
    stmt->bindInt64(5, info->m_size);
    stmt->bindString(6, info->m_hash);
    stmt->bindInt64(7, info->m_ownerId);
    stmt->bindString(8, info->m_bizType);
    stmt->bindInt64(9, info->m_bizId);
    stmt->bindInt32(10, info->m_status);
    stmt->bindInt32(11, info->m_isDeleted);
    stmt->bindTime(12, info->m_createTime);
    stmt->bindTime(13, info->m_updateTime);
    return stmt->execute();
}

int ResourceInfoDao::Delete(ResourceInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from resource where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int ResourceInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from resource where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int ResourceInfoDao::DeleteByOwnerId( const int64_t& owner_id, chen::IDB::ptr conn) {
    std::string sql = "delete from resource where owner_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, owner_id);
    return stmt->execute();
}

int ResourceInfoDao::DeleteByBizTypeBizId( const std::string& biz_type,  const int64_t& biz_id, chen::IDB::ptr conn) {
    std::string sql = "delete from resource where biz_type = ? and biz_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, biz_type);
    stmt->bindInt64(1, biz_id);
    return stmt->execute();
}

int ResourceInfoDao::DeleteByHash( const std::string& hash, chen::IDB::ptr conn) {
    std::string sql = "delete from resource where hash = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, hash);
    return stmt->execute();
}

int ResourceInfoDao::QueryAll(std::vector<ResourceInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, name, path, type, size, hash, owner_id, biz_type, biz_id, status, is_deleted, create_time, update_time from resource";
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
        ResourceInfo::ptr v(new ResourceInfo);
        v->m_id = rt->getInt64(0);
        v->m_name = rt->getString(1);
        v->m_path = rt->getString(2);
        v->m_type = rt->getInt32(3);
        v->m_size = rt->getInt64(4);
        v->m_hash = rt->getString(5);
        v->m_ownerId = rt->getInt64(6);
        v->m_bizType = rt->getString(7);
        v->m_bizId = rt->getInt64(8);
        v->m_status = rt->getInt32(9);
        v->m_isDeleted = rt->getInt32(10);
        v->m_createTime = rt->getTime(11);
        v->m_updateTime = rt->getTime(12);
        results.push_back(v);
    }
    return 0;
}

ResourceInfo::ptr ResourceInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, name, path, type, size, hash, owner_id, biz_type, biz_id, status, is_deleted, create_time, update_time from resource where id = ?";
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
    ResourceInfo::ptr v(new ResourceInfo);
    v->m_id = rt->getInt64(0);
    v->m_name = rt->getString(1);
    v->m_path = rt->getString(2);
    v->m_type = rt->getInt32(3);
    v->m_size = rt->getInt64(4);
    v->m_hash = rt->getString(5);
    v->m_ownerId = rt->getInt64(6);
    v->m_bizType = rt->getString(7);
    v->m_bizId = rt->getInt64(8);
    v->m_status = rt->getInt32(9);
    v->m_isDeleted = rt->getInt32(10);
    v->m_createTime = rt->getTime(11);
    v->m_updateTime = rt->getTime(12);
    return v;
}

int ResourceInfoDao::QueryByOwnerId(std::vector<ResourceInfo::ptr>& results,  const int64_t& owner_id, chen::IDB::ptr conn) {
    std::string sql = "select id, name, path, type, size, hash, owner_id, biz_type, biz_id, status, is_deleted, create_time, update_time from resource where owner_id = ?";
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
        ResourceInfo::ptr v(new ResourceInfo);
        v->m_id = rt->getInt64(0);
        v->m_name = rt->getString(1);
        v->m_path = rt->getString(2);
        v->m_type = rt->getInt32(3);
        v->m_size = rt->getInt64(4);
        v->m_hash = rt->getString(5);
        v->m_ownerId = rt->getInt64(6);
        v->m_bizType = rt->getString(7);
        v->m_bizId = rt->getInt64(8);
        v->m_status = rt->getInt32(9);
        v->m_isDeleted = rt->getInt32(10);
        v->m_createTime = rt->getTime(11);
        v->m_updateTime = rt->getTime(12);
        results.push_back(v);
    };
    return 0;
}

int ResourceInfoDao::QueryByBizTypeBizId(std::vector<ResourceInfo::ptr>& results,  const std::string& biz_type,  const int64_t& biz_id, chen::IDB::ptr conn) {
    std::string sql = "select id, name, path, type, size, hash, owner_id, biz_type, biz_id, status, is_deleted, create_time, update_time from resource where biz_type = ? and biz_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, biz_type);
    stmt->bindInt64(2, biz_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        ResourceInfo::ptr v(new ResourceInfo);
        v->m_id = rt->getInt64(0);
        v->m_name = rt->getString(1);
        v->m_path = rt->getString(2);
        v->m_type = rt->getInt32(3);
        v->m_size = rt->getInt64(4);
        v->m_hash = rt->getString(5);
        v->m_ownerId = rt->getInt64(6);
        v->m_bizType = rt->getString(7);
        v->m_bizId = rt->getInt64(8);
        v->m_status = rt->getInt32(9);
        v->m_isDeleted = rt->getInt32(10);
        v->m_createTime = rt->getTime(11);
        v->m_updateTime = rt->getTime(12);
        results.push_back(v);
    };
    return 0;
}

int ResourceInfoDao::QueryByHash(std::vector<ResourceInfo::ptr>& results,  const std::string& hash, chen::IDB::ptr conn) {
    std::string sql = "select id, name, path, type, size, hash, owner_id, biz_type, biz_id, status, is_deleted, create_time, update_time from resource where hash = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, hash);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        ResourceInfo::ptr v(new ResourceInfo);
        v->m_id = rt->getInt64(0);
        v->m_name = rt->getString(1);
        v->m_path = rt->getString(2);
        v->m_type = rt->getInt32(3);
        v->m_size = rt->getInt64(4);
        v->m_hash = rt->getString(5);
        v->m_ownerId = rt->getInt64(6);
        v->m_bizType = rt->getString(7);
        v->m_bizId = rt->getInt64(8);
        v->m_status = rt->getInt32(9);
        v->m_isDeleted = rt->getInt32(10);
        v->m_createTime = rt->getTime(11);
        v->m_updateTime = rt->getTime(12);
        results.push_back(v);
    };
    return 0;
}

int ResourceInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS resource("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL DEFAULT '',"
            "path TEXT NOT NULL DEFAULT '',"
            "type INTEGER NOT NULL DEFAULT 0,"
            "size INTEGER NOT NULL DEFAULT 0,"
            "hash TEXT NOT NULL DEFAULT '',"
            "owner_id INTEGER NOT NULL DEFAULT 0,"
            "biz_type TEXT NOT NULL DEFAULT '',"
            "biz_id INTEGER NOT NULL DEFAULT 0,"
            "status INTEGER NOT NULL DEFAULT 1,"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE INDEX IF NOT EXISTS resource_owner_id ON resource(owner_id);"
            "CREATE INDEX IF NOT EXISTS resource_biz_type_biz_id ON resource(biz_type,biz_id);"
            "CREATE INDEX IF NOT EXISTS resource_hash ON resource(hash);"
            );
}

int ResourceInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS resource("
            "`id` bigint AUTO_INCREMENT COMMENT '主键ID',"
            "`name` varchar(256) NOT NULL DEFAULT '' COMMENT '原始文件名',"
            "`path` varchar(512) NOT NULL DEFAULT '' COMMENT '存储路径（相对或绝对）',"
            "`type` int NOT NULL DEFAULT 0 COMMENT '文件类型 1: image、2: video、3: document、4: compressed、5: audio、6: other',"
            "`size` bigint NOT NULL DEFAULT 0 COMMENT '文件大小（字节）',"
            "`hash` varchar(64) NOT NULL DEFAULT '' COMMENT '文件哈希（去重/校验）',"
            "`owner_id` bigint NOT NULL DEFAULT 0 COMMENT '上传者用户ID',"
            "`biz_type` varchar(32) NOT NULL DEFAULT '' COMMENT '业务类型 如assignment、article等',"
            "`biz_id` bigint NOT NULL DEFAULT 0 COMMENT '业务ID 如作业ID、文章ID等',"
            "`status` int NOT NULL DEFAULT 1 COMMENT '状态: 1正常 2删除',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '上传时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "KEY `resource_owner_id` (`owner_id`),"
            "KEY `resource_biz_type_biz_id` (`biz_type`,`biz_id`),"
            "KEY `resource_hash` (`hash`)) COMMENT='文件资源元数据表'");
}

int ResourceInfoDao::MigrateTableSQLite3(chen::IDB::ptr conn) {
    auto data = conn->query("PRAGMA table_info(resource)");
    if (!data) {
        ERROR(logger) << "PRAGMA table_info(resource) errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(1));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("name");
    expected_cols.insert("path");
    expected_cols.insert("type");
    expected_cols.insert("size");
    expected_cols.insert("hash");
    expected_cols.insert("owner_id");
    expected_cols.insert("biz_type");
    expected_cols.insert("biz_id");
    expected_cols.insert("status");
    expected_cols.insert("is_deleted");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("name") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.name";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN name TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN name failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("path") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.path";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN path TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN path failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("type") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.type";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN type INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("size") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.size";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN size INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN size failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("hash") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.hash";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN hash TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN hash failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("owner_id") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.owner_id";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN owner_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN owner_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("biz_type") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.biz_type";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN biz_type TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN biz_type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("biz_id") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.biz_id";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN biz_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN biz_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("status") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.status";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN status INTEGER NOT NULL DEFAULT 1");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN status failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.is_deleted";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.create_time";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN create_time TIMESTAMP NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.update_time";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column resource." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE resource DROP COLUMN " + col);
            if (rt) {
                ERROR(logger) << "ALTER TABLE resource DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}

int ResourceInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM resource");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM resource errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(0));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("name");
    expected_cols.insert("path");
    expected_cols.insert("type");
    expected_cols.insert("size");
    expected_cols.insert("hash");
    expected_cols.insert("owner_id");
    expected_cols.insert("biz_type");
    expected_cols.insert("biz_id");
    expected_cols.insert("status");
    expected_cols.insert("is_deleted");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("name") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.name";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN `name` varchar(256) NOT NULL DEFAULT '' COMMENT '原始文件名'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN name failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("path") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.path";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN `path` varchar(512) NOT NULL DEFAULT '' COMMENT '存储路径（相对或绝对）'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN path failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("type") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.type";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN `type` int NOT NULL DEFAULT 0 COMMENT '文件类型 1: image、2: video、3: document、4: compressed、5: audio、6: other'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("size") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.size";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN `size` bigint NOT NULL DEFAULT 0 COMMENT '文件大小（字节）'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN size failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("hash") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.hash";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN `hash` varchar(64) NOT NULL DEFAULT '' COMMENT '文件哈希（去重/校验）'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN hash failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("owner_id") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.owner_id";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN `owner_id` bigint NOT NULL DEFAULT 0 COMMENT '上传者用户ID'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN owner_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("biz_type") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.biz_type";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN `biz_type` varchar(32) NOT NULL DEFAULT '' COMMENT '业务类型 如assignment、article等'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN biz_type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("biz_id") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.biz_id";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN `biz_id` bigint NOT NULL DEFAULT 0 COMMENT '业务ID 如作业ID、文章ID等'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN biz_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("status") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.status";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN `status` int NOT NULL DEFAULT 1 COMMENT '状态: 1正常 2删除'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN status failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.is_deleted";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN `is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.create_time";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '上传时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column resource.update_time";
        int rt = conn->execute("ALTER TABLE resource ADD COLUMN `update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '更新时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE resource ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column resource." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE resource DROP COLUMN `" + col + "`");
            if (rt) {
                ERROR(logger) << "ALTER TABLE resource DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}


} //namespace data
} //namespace blog
