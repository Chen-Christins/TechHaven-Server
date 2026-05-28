#include "organization_user_rel_info.h"
#include "chen/log/log.h"
#include <map>

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

OrganizationUserRelInfo::OrganizationUserRelInfo()
    :m_role(1)
    ,m_status(1)
    ,m_isDeleted(0)
    ,m_id()
    ,m_orgId()
    ,m_userId()
    ,m_createTime(time(0))
    ,m_updateTime() {
}

std::string OrganizationUserRelInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["org_id"] = std::to_string(m_orgId);
    v["user_id"] = std::to_string(m_userId);
    v["role"] = m_role;
    v["status"] = m_status;
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void OrganizationUserRelInfo::setId(const int64_t& v) {
    m_id = v;
}

void OrganizationUserRelInfo::setOrgId(const int64_t& v) {
    m_orgId = v;
}

void OrganizationUserRelInfo::setUserId(const int64_t& v) {
    m_userId = v;
}

void OrganizationUserRelInfo::setRole(const int32_t& v) {
    m_role = v;
}

void OrganizationUserRelInfo::setStatus(const int32_t& v) {
    m_status = v;
}

void OrganizationUserRelInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void OrganizationUserRelInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void OrganizationUserRelInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int OrganizationUserRelInfoDao::Update(OrganizationUserRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update organization_user_rel set org_id = ?, user_id = ?, role = ?, status = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_orgId);
    stmt->bindInt64(2, info->m_userId);
    stmt->bindInt32(3, info->m_role);
    stmt->bindInt32(4, info->m_status);
    stmt->bindInt32(5, info->m_isDeleted);
    stmt->bindTime(6, info->m_createTime);
    stmt->bindTime(7, info->m_updateTime);
    stmt->bindInt64(8, info->m_id);
    return stmt->execute();
}

int OrganizationUserRelInfoDao::Insert(OrganizationUserRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into organization_user_rel (org_id, user_id, role, status, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_orgId);
    stmt->bindInt64(2, info->m_userId);
    stmt->bindInt32(3, info->m_role);
    stmt->bindInt32(4, info->m_status);
    stmt->bindInt32(5, info->m_isDeleted);
    stmt->bindTime(6, info->m_createTime);
    stmt->bindTime(7, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int OrganizationUserRelInfoDao::InsertOrUpdate(OrganizationUserRelInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into organization_user_rel (id, org_id, user_id, role, status, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_orgId);
    stmt->bindInt64(3, info->m_userId);
    stmt->bindInt32(4, info->m_role);
    stmt->bindInt32(5, info->m_status);
    stmt->bindInt32(6, info->m_isDeleted);
    stmt->bindTime(7, info->m_createTime);
    stmt->bindTime(8, info->m_updateTime);
    return stmt->execute();
}

int OrganizationUserRelInfoDao::Delete(OrganizationUserRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from organization_user_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int OrganizationUserRelInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from organization_user_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int OrganizationUserRelInfoDao::DeleteByOrgIdUserId( const int64_t& org_id,  const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "delete from organization_user_rel where org_id = ? and user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, org_id);
    stmt->bindInt64(1, user_id);
    return stmt->execute();
}

int OrganizationUserRelInfoDao::DeleteByUserId( const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "delete from organization_user_rel where user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    return stmt->execute();
}

int OrganizationUserRelInfoDao::QueryAll(std::vector<OrganizationUserRelInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, user_id, role, status, is_deleted, create_time, update_time from organization_user_rel";
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
        OrganizationUserRelInfo::ptr v(new OrganizationUserRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_orgId = rt->getInt64(1);
        v->m_userId = rt->getInt64(2);
        v->m_role = rt->getInt32(3);
        v->m_status = rt->getInt32(4);
        v->m_isDeleted = rt->getInt32(5);
        v->m_createTime = rt->getTime(6);
        v->m_updateTime = rt->getTime(7);
        results.push_back(v);
    }
    return 0;
}

OrganizationUserRelInfo::ptr OrganizationUserRelInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, user_id, role, status, is_deleted, create_time, update_time from organization_user_rel where id = ?";
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
    OrganizationUserRelInfo::ptr v(new OrganizationUserRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_orgId = rt->getInt64(1);
    v->m_userId = rt->getInt64(2);
    v->m_role = rt->getInt32(3);
    v->m_status = rt->getInt32(4);
    v->m_isDeleted = rt->getInt32(5);
    v->m_createTime = rt->getTime(6);
    v->m_updateTime = rt->getTime(7);
    return v;
}

OrganizationUserRelInfo::ptr OrganizationUserRelInfoDao::QueryByOrgIdUserId( const int64_t& org_id,  const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, user_id, role, status, is_deleted, create_time, update_time from organization_user_rel where org_id = ? and user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindInt64(1, org_id);
    stmt->bindInt64(2, user_id);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    OrganizationUserRelInfo::ptr v(new OrganizationUserRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_orgId = rt->getInt64(1);
    v->m_userId = rt->getInt64(2);
    v->m_role = rt->getInt32(3);
    v->m_status = rt->getInt32(4);
    v->m_isDeleted = rt->getInt32(5);
    v->m_createTime = rt->getTime(6);
    v->m_updateTime = rt->getTime(7);
    return v;
}

int OrganizationUserRelInfoDao::QueryByUserId(std::vector<OrganizationUserRelInfo::ptr>& results,  const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, user_id, role, status, is_deleted, create_time, update_time from organization_user_rel where user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        OrganizationUserRelInfo::ptr v(new OrganizationUserRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_orgId = rt->getInt64(1);
        v->m_userId = rt->getInt64(2);
        v->m_role = rt->getInt32(3);
        v->m_status = rt->getInt32(4);
        v->m_isDeleted = rt->getInt32(5);
        v->m_createTime = rt->getTime(6);
        v->m_updateTime = rt->getTime(7);
        results.push_back(v);
    };
    return 0;
}

int OrganizationUserRelInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS organization_user_rel("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "org_id INTEGER NOT NULL DEFAULT 0,"
            "user_id INTEGER NOT NULL DEFAULT 0,"
            "role INTEGER NOT NULL DEFAULT 1,"
            "status INTEGER NOT NULL DEFAULT 1,"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE UNIQUE INDEX IF NOT EXISTS organization_user_rel_org_id_user_id ON organization_user_rel(org_id,user_id);"
            "CREATE INDEX IF NOT EXISTS organization_user_rel_user_id ON organization_user_rel(user_id);"
            );
}

int OrganizationUserRelInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS organization_user_rel("
            "`id` bigint AUTO_INCREMENT COMMENT '主键ID',"
            "`org_id` bigint NOT NULL DEFAULT 0 COMMENT '组织ID',"
            "`user_id` bigint NOT NULL DEFAULT 0 COMMENT '用户ID',"
            "`role` int NOT NULL DEFAULT 1 COMMENT '角色: 1普通成员 2报告者 3开发者 4研发主管 5组织管理员',"
            "`status` int NOT NULL DEFAULT 1 COMMENT '状态: 0申请中 1已加入 2已拒绝 3已退出',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '加入时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "UNIQUE KEY `organization_user_rel_org_id_user_id` (`org_id`,`user_id`),"
            "KEY `organization_user_rel_user_id` (`user_id`)) COMMENT='组织用户关联表'");
}

int OrganizationUserRelInfoDao::MigrateTableSQLite3(chen::IDB::ptr conn) {
    auto data = conn->query("PRAGMA table_info(organization_user_rel)");
    if (!data) {
        ERROR(logger) << "PRAGMA table_info(organization_user_rel) errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
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
            INFO(logger) << "Column type changed: organization_user_rel.id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("org_id");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: organization_user_rel.org_id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("user_id");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: organization_user_rel.user_id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("role");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: organization_user_rel.role " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("status");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: organization_user_rel.status " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("is_deleted");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: organization_user_rel.is_deleted " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("create_time");
        if (it != existing_cols.end() && it->second != "TIMESTAMP") {
            INFO(logger) << "Column type changed: organization_user_rel.create_time " << it->second << " -> TIMESTAMP";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("update_time");
        if (it != existing_cols.end() && it->second != "TIMESTAMP") {
            INFO(logger) << "Column type changed: organization_user_rel.update_time " << it->second << " -> TIMESTAMP";
            need_recreate = true;
        }
    }
    if (!need_recreate) {
        for (auto& [name, _] : existing_cols) {
            (void)_;  // suppress unused warning
            bool found = false;
            if (name == "id") found = true;
            if (name == "org_id") found = true;
            if (name == "user_id") found = true;
            if (name == "role") found = true;
            if (name == "status") found = true;
            if (name == "is_deleted") found = true;
            if (name == "create_time") found = true;
            if (name == "update_time") found = true;
            if (!found) {
                need_recreate = true;
                WARN(logger) << "Column organization_user_rel." << name << " removed, table recreate required";
                break;
            }
        }
    }

    if (need_recreate) {
        INFO(logger) << "Recreating table organization_user_rel";

        std::vector<std::string> common_cols;
        if (existing_cols.find("id") != existing_cols.end()) {
            common_cols.push_back("id");
        }
        if (existing_cols.find("org_id") != existing_cols.end()) {
            common_cols.push_back("org_id");
        }
        if (existing_cols.find("user_id") != existing_cols.end()) {
            common_cols.push_back("user_id");
        }
        if (existing_cols.find("role") != existing_cols.end()) {
            common_cols.push_back("role");
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

        if (conn->execute("ALTER TABLE organization_user_rel RENAME TO organization_user_rel_tmp")) {
            ERROR(logger) << "RENAME TABLE organization_user_rel failed";
            return conn->getErrno();
        }
        CreateTableSQLite3(conn);
        if (!common_cols.empty()) {
            std::string cols;
            for (size_t i = 0; i < common_cols.size(); ++i) {
                if (i) cols += ",";
                cols += common_cols[i];
            }
            std::string sql = "INSERT INTO organization_user_rel (" + cols + ") SELECT " + cols + " FROM organization_user_rel_tmp";
            if (int rt = conn->execute(sql)) {
                ERROR(logger) << "copy data from organization_user_rel_tmp to organization_user_rel failed, errno=" << rt;
                // don't return; try to continue
            }
        }
        conn->execute("DROP TABLE organization_user_rel_tmp");
        return 0;
    }

    if (existing_cols.find("org_id") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_user_rel.org_id";
        int rt = conn->execute("ALTER TABLE organization_user_rel ADD COLUMN org_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_user_rel ADD COLUMN org_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("user_id") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_user_rel.user_id";
        int rt = conn->execute("ALTER TABLE organization_user_rel ADD COLUMN user_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_user_rel ADD COLUMN user_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("role") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_user_rel.role";
        int rt = conn->execute("ALTER TABLE organization_user_rel ADD COLUMN role INTEGER NOT NULL DEFAULT 1");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_user_rel ADD COLUMN role failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("status") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_user_rel.status";
        int rt = conn->execute("ALTER TABLE organization_user_rel ADD COLUMN status INTEGER NOT NULL DEFAULT 1");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_user_rel ADD COLUMN status failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_user_rel.is_deleted";
        int rt = conn->execute("ALTER TABLE organization_user_rel ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_user_rel ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_user_rel.create_time";
        int rt = conn->execute("ALTER TABLE organization_user_rel ADD COLUMN create_time TIMESTAMP NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_user_rel ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_user_rel.update_time";
        int rt = conn->execute("ALTER TABLE organization_user_rel ADD COLUMN update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_user_rel ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    return 0;
}

int OrganizationUserRelInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM organization_user_rel");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM organization_user_rel errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::map<std::string, std::string> existing_cols;  // name -> type
    while (data->next()) {
        existing_cols[data->getString(0)] = data->getString(1);
    }

    {
        auto it = existing_cols.find("id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column organization_user_rel.id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE organization_user_rel MODIFY COLUMN `id` bigint NOT NULL DEFAULT 0 COMMENT '主键ID'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_user_rel.id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("org_id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column organization_user_rel.org_id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE organization_user_rel MODIFY COLUMN `org_id` bigint NOT NULL DEFAULT 0 COMMENT '组织ID'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_user_rel.org_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("user_id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column organization_user_rel.user_id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE organization_user_rel MODIFY COLUMN `user_id` bigint NOT NULL DEFAULT 0 COMMENT '用户ID'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_user_rel.user_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("role");
        if (it != existing_cols.end() && it->second != "int") {
            INFO(logger) << "Modifying column organization_user_rel.role " << it->second << " -> int";
            int rt = conn->execute("ALTER TABLE organization_user_rel MODIFY COLUMN `role` int NOT NULL DEFAULT 1 COMMENT '角色: 1普通成员 2报告者 3开发者 4研发主管 5组织管理员'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_user_rel.role failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("status");
        if (it != existing_cols.end() && it->second != "int") {
            INFO(logger) << "Modifying column organization_user_rel.status " << it->second << " -> int";
            int rt = conn->execute("ALTER TABLE organization_user_rel MODIFY COLUMN `status` int NOT NULL DEFAULT 1 COMMENT '状态: 0申请中 1已加入 2已拒绝 3已退出'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_user_rel.status failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("is_deleted");
        if (it != existing_cols.end() && it->second != "int") {
            INFO(logger) << "Modifying column organization_user_rel.is_deleted " << it->second << " -> int";
            int rt = conn->execute("ALTER TABLE organization_user_rel MODIFY COLUMN `is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_user_rel.is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("create_time");
        if (it != existing_cols.end() && it->second != "timestamp") {
            INFO(logger) << "Modifying column organization_user_rel.create_time " << it->second << " -> timestamp";
            int rt = conn->execute("ALTER TABLE organization_user_rel MODIFY COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '加入时间'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_user_rel.create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("update_time");
        if (it != existing_cols.end() && it->second != "timestamp") {
            INFO(logger) << "Modifying column organization_user_rel.update_time " << it->second << " -> timestamp";
            int rt = conn->execute("ALTER TABLE organization_user_rel MODIFY COLUMN `update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '更新时间'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_user_rel.update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    for (auto& [name, _] : existing_cols) {
        (void)_;
        bool found = false;
        if (name == "id") found = true;
        if (name == "org_id") found = true;
        if (name == "user_id") found = true;
        if (name == "role") found = true;
        if (name == "status") found = true;
        if (name == "is_deleted") found = true;
        if (name == "create_time") found = true;
        if (name == "update_time") found = true;
        if (!found) {
            WARN(logger) << "Dropping column organization_user_rel." << name << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE organization_user_rel DROP COLUMN `" + name + "`");
            if (rt) {
                ERROR(logger) << "DROP COLUMN organization_user_rel." << name << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    if (existing_cols.find("org_id") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_user_rel.org_id";
        int rt = conn->execute("ALTER TABLE organization_user_rel ADD COLUMN `org_id` bigint NOT NULL DEFAULT 0 COMMENT '组织ID'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_user_rel ADD COLUMN org_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("user_id") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_user_rel.user_id";
        int rt = conn->execute("ALTER TABLE organization_user_rel ADD COLUMN `user_id` bigint NOT NULL DEFAULT 0 COMMENT '用户ID'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_user_rel ADD COLUMN user_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("role") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_user_rel.role";
        int rt = conn->execute("ALTER TABLE organization_user_rel ADD COLUMN `role` int NOT NULL DEFAULT 1 COMMENT '角色: 1普通成员 2报告者 3开发者 4研发主管 5组织管理员'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_user_rel ADD COLUMN role failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("status") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_user_rel.status";
        int rt = conn->execute("ALTER TABLE organization_user_rel ADD COLUMN `status` int NOT NULL DEFAULT 1 COMMENT '状态: 0申请中 1已加入 2已拒绝 3已退出'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_user_rel ADD COLUMN status failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_user_rel.is_deleted";
        int rt = conn->execute("ALTER TABLE organization_user_rel ADD COLUMN `is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_user_rel ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_user_rel.create_time";
        int rt = conn->execute("ALTER TABLE organization_user_rel ADD COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '加入时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_user_rel ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_user_rel.update_time";
        int rt = conn->execute("ALTER TABLE organization_user_rel ADD COLUMN `update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '更新时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_user_rel ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    return 0;
}


} //namespace data
} //namespace blog
