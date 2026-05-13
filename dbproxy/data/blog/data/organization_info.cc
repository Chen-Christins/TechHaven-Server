#include "organization_info.h"
#include "chen/log/log.h"

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

OrganizationInfo::OrganizationInfo()
    :m_status(1)
    ,m_isDeleted(0)
    ,m_id()
    ,m_ownerId()
    ,m_name()
    ,m_type()
    ,m_description()
    ,m_createTime(time(0))
    ,m_updateTime() {
}

std::string OrganizationInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["name"] = m_name;
    v["type"] = m_type;
    v["description"] = m_description;
    v["owner_id"] = std::to_string(m_ownerId);
    v["status"] = m_status;
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void OrganizationInfo::setId(const int64_t& v) {
    m_id = v;
}

void OrganizationInfo::setName(const std::string& v) {
    m_name = v;
}

void OrganizationInfo::setType(const std::string& v) {
    m_type = v;
}

void OrganizationInfo::setDescription(const std::string& v) {
    m_description = v;
}

void OrganizationInfo::setOwnerId(const int64_t& v) {
    m_ownerId = v;
}

void OrganizationInfo::setStatus(const int32_t& v) {
    m_status = v;
}

void OrganizationInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void OrganizationInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void OrganizationInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int OrganizationInfoDao::Update(OrganizationInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update organization set name = ?, type = ?, description = ?, owner_id = ?, status = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_name);
    stmt->bindString(2, info->m_type);
    stmt->bindString(3, info->m_description);
    stmt->bindInt64(4, info->m_ownerId);
    stmt->bindInt32(5, info->m_status);
    stmt->bindInt32(6, info->m_isDeleted);
    stmt->bindTime(7, info->m_createTime);
    stmt->bindTime(8, info->m_updateTime);
    stmt->bindInt64(9, info->m_id);
    return stmt->execute();
}

int OrganizationInfoDao::Insert(OrganizationInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into organization (name, type, description, owner_id, status, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_name);
    stmt->bindString(2, info->m_type);
    stmt->bindString(3, info->m_description);
    stmt->bindInt64(4, info->m_ownerId);
    stmt->bindInt32(5, info->m_status);
    stmt->bindInt32(6, info->m_isDeleted);
    stmt->bindTime(7, info->m_createTime);
    stmt->bindTime(8, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int OrganizationInfoDao::InsertOrUpdate(OrganizationInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into organization (id, name, type, description, owner_id, status, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindString(2, info->m_name);
    stmt->bindString(3, info->m_type);
    stmt->bindString(4, info->m_description);
    stmt->bindInt64(5, info->m_ownerId);
    stmt->bindInt32(6, info->m_status);
    stmt->bindInt32(7, info->m_isDeleted);
    stmt->bindTime(8, info->m_createTime);
    stmt->bindTime(9, info->m_updateTime);
    return stmt->execute();
}

int OrganizationInfoDao::Delete(OrganizationInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from organization where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int OrganizationInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from organization where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int OrganizationInfoDao::DeleteByOwnerId( const int64_t& owner_id, chen::IDB::ptr conn) {
    std::string sql = "delete from organization where owner_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, owner_id);
    return stmt->execute();
}

int OrganizationInfoDao::DeleteByName( const std::string& name, chen::IDB::ptr conn) {
    std::string sql = "delete from organization where name = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, name);
    return stmt->execute();
}

int OrganizationInfoDao::QueryAll(std::vector<OrganizationInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, name, type, description, owner_id, status, is_deleted, create_time, update_time from organization";
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
        OrganizationInfo::ptr v(new OrganizationInfo);
        v->m_id = rt->getInt64(0);
        v->m_name = rt->getString(1);
        v->m_type = rt->getString(2);
        v->m_description = rt->getString(3);
        v->m_ownerId = rt->getInt64(4);
        v->m_status = rt->getInt32(5);
        v->m_isDeleted = rt->getInt32(6);
        v->m_createTime = rt->getTime(7);
        v->m_updateTime = rt->getTime(8);
        results.push_back(v);
    }
    return 0;
}

OrganizationInfo::ptr OrganizationInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, name, type, description, owner_id, status, is_deleted, create_time, update_time from organization where id = ?";
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
    OrganizationInfo::ptr v(new OrganizationInfo);
    v->m_id = rt->getInt64(0);
    v->m_name = rt->getString(1);
    v->m_type = rt->getString(2);
    v->m_description = rt->getString(3);
    v->m_ownerId = rt->getInt64(4);
    v->m_status = rt->getInt32(5);
    v->m_isDeleted = rt->getInt32(6);
    v->m_createTime = rt->getTime(7);
    v->m_updateTime = rt->getTime(8);
    return v;
}

int OrganizationInfoDao::QueryByOwnerId(std::vector<OrganizationInfo::ptr>& results,  const int64_t& owner_id, chen::IDB::ptr conn) {
    std::string sql = "select id, name, type, description, owner_id, status, is_deleted, create_time, update_time from organization where owner_id = ?";
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
        OrganizationInfo::ptr v(new OrganizationInfo);
        v->m_id = rt->getInt64(0);
        v->m_name = rt->getString(1);
        v->m_type = rt->getString(2);
        v->m_description = rt->getString(3);
        v->m_ownerId = rt->getInt64(4);
        v->m_status = rt->getInt32(5);
        v->m_isDeleted = rt->getInt32(6);
        v->m_createTime = rt->getTime(7);
        v->m_updateTime = rt->getTime(8);
        results.push_back(v);
    };
    return 0;
}

OrganizationInfo::ptr OrganizationInfoDao::QueryByName( const std::string& name, chen::IDB::ptr conn) {
    std::string sql = "select id, name, type, description, owner_id, status, is_deleted, create_time, update_time from organization where name = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindString(1, name);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    OrganizationInfo::ptr v(new OrganizationInfo);
    v->m_id = rt->getInt64(0);
    v->m_name = rt->getString(1);
    v->m_type = rt->getString(2);
    v->m_description = rt->getString(3);
    v->m_ownerId = rt->getInt64(4);
    v->m_status = rt->getInt32(5);
    v->m_isDeleted = rt->getInt32(6);
    v->m_createTime = rt->getTime(7);
    v->m_updateTime = rt->getTime(8);
    return v;
}

int OrganizationInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS organization("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL DEFAULT '',"
            "type TEXT NOT NULL DEFAULT '',"
            "description TEXT NOT NULL DEFAULT '',"
            "owner_id INTEGER NOT NULL DEFAULT 0,"
            "status INTEGER NOT NULL DEFAULT 1,"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE INDEX IF NOT EXISTS organization_owner_id ON organization(owner_id);"
            "CREATE UNIQUE INDEX IF NOT EXISTS organization_name ON organization(name);"
            );
}

int OrganizationInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS organization("
            "`id` bigint AUTO_INCREMENT COMMENT '组织ID',"
            "`name` varchar(100) NOT NULL DEFAULT '' COMMENT '组织名称',"
            "`type` varchar(50) NOT NULL DEFAULT '' COMMENT '组织类型',"
            "`description` varchar(500) NOT NULL DEFAULT '' COMMENT '组织描述',"
            "`owner_id` bigint NOT NULL DEFAULT 0 COMMENT '拥有者ID(用户ID)',"
            "`status` int NOT NULL DEFAULT 1 COMMENT '状态: 0停用 1启用',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "KEY `organization_owner_id` (`owner_id`),"
            "UNIQUE KEY `organization_name` (`name`)) COMMENT='组织表'");
}
} //namespace data
} //namespace blog
