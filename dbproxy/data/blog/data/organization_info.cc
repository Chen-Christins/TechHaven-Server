#include "organization_info.h"
#include "chen/log/log.h"
#include <map>

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

int OrganizationInfoDao::MigrateTableSQLite3(chen::IDB::ptr conn) {
    auto data = conn->query("PRAGMA table_info(organization)");
    if (!data) {
        ERROR(logger) << "PRAGMA table_info(organization) errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
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
            INFO(logger) << "Column type changed: organization.id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("name");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: organization.name " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("type");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: organization.type " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("description");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: organization.description " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("owner_id");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: organization.owner_id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("status");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: organization.status " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("is_deleted");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: organization.is_deleted " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("create_time");
        if (it != existing_cols.end() && it->second != "TIMESTAMP") {
            INFO(logger) << "Column type changed: organization.create_time " << it->second << " -> TIMESTAMP";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("update_time");
        if (it != existing_cols.end() && it->second != "TIMESTAMP") {
            INFO(logger) << "Column type changed: organization.update_time " << it->second << " -> TIMESTAMP";
            need_recreate = true;
        }
    }
    if (!need_recreate) {
        for (auto& [name, _] : existing_cols) {
            (void)_;  // suppress unused warning
            bool found = false;
            if (name == "id") found = true;
            if (name == "name") found = true;
            if (name == "type") found = true;
            if (name == "description") found = true;
            if (name == "owner_id") found = true;
            if (name == "status") found = true;
            if (name == "is_deleted") found = true;
            if (name == "create_time") found = true;
            if (name == "update_time") found = true;
            if (!found) {
                need_recreate = true;
                WARN(logger) << "Column organization." << name << " removed, table recreate required";
                break;
            }
        }
    }

    if (need_recreate) {
        INFO(logger) << "Recreating table organization";

        std::vector<std::string> common_cols;
        if (existing_cols.find("id") != existing_cols.end()) {
            common_cols.push_back("id");
        }
        if (existing_cols.find("name") != existing_cols.end()) {
            common_cols.push_back("name");
        }
        if (existing_cols.find("type") != existing_cols.end()) {
            common_cols.push_back("type");
        }
        if (existing_cols.find("description") != existing_cols.end()) {
            common_cols.push_back("description");
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

        if (conn->execute("ALTER TABLE organization RENAME TO organization_tmp")) {
            ERROR(logger) << "RENAME TABLE organization failed";
            return conn->getErrno();
        }
        CreateTableSQLite3(conn);
        if (!common_cols.empty()) {
            std::string cols;
            for (size_t i = 0; i < common_cols.size(); ++i) {
                if (i) cols += ",";
                cols += common_cols[i];
            }
            std::string sql = "INSERT INTO organization (" + cols + ") SELECT " + cols + " FROM organization_tmp";
            if (int rt = conn->execute(sql)) {
                ERROR(logger) << "copy data from organization_tmp to organization failed, errno=" << rt;
                // don't return; try to continue
            }
        }
        conn->execute("DROP TABLE organization_tmp");
        return 0;
    }

    if (existing_cols.find("name") == existing_cols.end()) {
        INFO(logger) << "Adding column organization.name";
        int rt = conn->execute("ALTER TABLE organization ADD COLUMN name TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization ADD COLUMN name failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("type") == existing_cols.end()) {
        INFO(logger) << "Adding column organization.type";
        int rt = conn->execute("ALTER TABLE organization ADD COLUMN type TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization ADD COLUMN type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("description") == existing_cols.end()) {
        INFO(logger) << "Adding column organization.description";
        int rt = conn->execute("ALTER TABLE organization ADD COLUMN description TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization ADD COLUMN description failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("owner_id") == existing_cols.end()) {
        INFO(logger) << "Adding column organization.owner_id";
        int rt = conn->execute("ALTER TABLE organization ADD COLUMN owner_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization ADD COLUMN owner_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("status") == existing_cols.end()) {
        INFO(logger) << "Adding column organization.status";
        int rt = conn->execute("ALTER TABLE organization ADD COLUMN status INTEGER NOT NULL DEFAULT 1");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization ADD COLUMN status failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column organization.is_deleted";
        int rt = conn->execute("ALTER TABLE organization ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column organization.create_time";
        int rt = conn->execute("ALTER TABLE organization ADD COLUMN create_time TIMESTAMP NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column organization.update_time";
        int rt = conn->execute("ALTER TABLE organization ADD COLUMN update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    return 0;
}

int OrganizationInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM organization");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM organization errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::map<std::string, std::string> existing_cols;  // name -> type
    while (data->next()) {
        existing_cols[data->getString(0)] = data->getString(1);
    }

    {
        auto it = existing_cols.find("id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column organization.id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE organization MODIFY COLUMN `id` bigint NOT NULL DEFAULT 0 COMMENT '组织ID'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization.id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("name");
        if (it != existing_cols.end() && it->second != "varchar(100)") {
            INFO(logger) << "Modifying column organization.name " << it->second << " -> varchar(100)";
            int rt = conn->execute("ALTER TABLE organization MODIFY COLUMN `name` varchar(100) NOT NULL DEFAULT '' COMMENT '组织名称'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization.name failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("type");
        if (it != existing_cols.end() && it->second != "varchar(50)") {
            INFO(logger) << "Modifying column organization.type " << it->second << " -> varchar(50)";
            int rt = conn->execute("ALTER TABLE organization MODIFY COLUMN `type` varchar(50) NOT NULL DEFAULT '' COMMENT '组织类型'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization.type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("description");
        if (it != existing_cols.end() && it->second != "varchar(500)") {
            INFO(logger) << "Modifying column organization.description " << it->second << " -> varchar(500)";
            int rt = conn->execute("ALTER TABLE organization MODIFY COLUMN `description` varchar(500) NOT NULL DEFAULT '' COMMENT '组织描述'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization.description failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("owner_id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column organization.owner_id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE organization MODIFY COLUMN `owner_id` bigint NOT NULL DEFAULT 0 COMMENT '拥有者ID(用户ID)'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization.owner_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("status");
        if (it != existing_cols.end() && it->second != "int") {
            INFO(logger) << "Modifying column organization.status " << it->second << " -> int";
            int rt = conn->execute("ALTER TABLE organization MODIFY COLUMN `status` int NOT NULL DEFAULT 1 COMMENT '状态: 0停用 1启用'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization.status failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("is_deleted");
        if (it != existing_cols.end() && it->second != "int") {
            INFO(logger) << "Modifying column organization.is_deleted " << it->second << " -> int";
            int rt = conn->execute("ALTER TABLE organization MODIFY COLUMN `is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization.is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("create_time");
        if (it != existing_cols.end() && it->second != "timestamp") {
            INFO(logger) << "Modifying column organization.create_time " << it->second << " -> timestamp";
            int rt = conn->execute("ALTER TABLE organization MODIFY COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization.create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("update_time");
        if (it != existing_cols.end() && it->second != "timestamp") {
            INFO(logger) << "Modifying column organization.update_time " << it->second << " -> timestamp";
            int rt = conn->execute("ALTER TABLE organization MODIFY COLUMN `update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '更新时间'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization.update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    for (auto& [name, _] : existing_cols) {
        (void)_;
        bool found = false;
        if (name == "id") found = true;
        if (name == "name") found = true;
        if (name == "type") found = true;
        if (name == "description") found = true;
        if (name == "owner_id") found = true;
        if (name == "status") found = true;
        if (name == "is_deleted") found = true;
        if (name == "create_time") found = true;
        if (name == "update_time") found = true;
        if (!found) {
            WARN(logger) << "Dropping column organization." << name << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE organization DROP COLUMN `" + name + "`");
            if (rt) {
                ERROR(logger) << "DROP COLUMN organization." << name << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    if (existing_cols.find("name") == existing_cols.end()) {
        INFO(logger) << "Adding column organization.name";
        int rt = conn->execute("ALTER TABLE organization ADD COLUMN `name` varchar(100) NOT NULL DEFAULT '' COMMENT '组织名称'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization ADD COLUMN name failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("type") == existing_cols.end()) {
        INFO(logger) << "Adding column organization.type";
        int rt = conn->execute("ALTER TABLE organization ADD COLUMN `type` varchar(50) NOT NULL DEFAULT '' COMMENT '组织类型'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization ADD COLUMN type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("description") == existing_cols.end()) {
        INFO(logger) << "Adding column organization.description";
        int rt = conn->execute("ALTER TABLE organization ADD COLUMN `description` varchar(500) NOT NULL DEFAULT '' COMMENT '组织描述'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization ADD COLUMN description failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("owner_id") == existing_cols.end()) {
        INFO(logger) << "Adding column organization.owner_id";
        int rt = conn->execute("ALTER TABLE organization ADD COLUMN `owner_id` bigint NOT NULL DEFAULT 0 COMMENT '拥有者ID(用户ID)'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization ADD COLUMN owner_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("status") == existing_cols.end()) {
        INFO(logger) << "Adding column organization.status";
        int rt = conn->execute("ALTER TABLE organization ADD COLUMN `status` int NOT NULL DEFAULT 1 COMMENT '状态: 0停用 1启用'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization ADD COLUMN status failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column organization.is_deleted";
        int rt = conn->execute("ALTER TABLE organization ADD COLUMN `is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column organization.create_time";
        int rt = conn->execute("ALTER TABLE organization ADD COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column organization.update_time";
        int rt = conn->execute("ALTER TABLE organization ADD COLUMN `update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '更新时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    return 0;
}


} //namespace data
} //namespace blog
