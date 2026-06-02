#include "organization_apply_info.h"
#include "chen/log/log.h"
#include <map>

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

OrganizationApplyInfo::OrganizationApplyInfo()
    :m_status(0)
    ,m_isDeleted(0)
    ,m_id()
    ,m_userId()
    ,m_createdAt()
    ,m_reviewedAt()
    ,m_orgName()
    ,m_orgType()
    ,m_orgDescription()
    ,m_reviewReason() {
}

std::string OrganizationApplyInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["user_id"] = std::to_string(m_userId);
    v["org_name"] = m_orgName;
    v["org_type"] = m_orgType;
    v["org_description"] = m_orgDescription;
    v["status"] = m_status;
    v["review_reason"] = m_reviewReason;
    v["created_at"] = std::to_string(m_createdAt);
    v["reviewed_at"] = std::to_string(m_reviewedAt);
    v["is_deleted"] = m_isDeleted;
    return chen::JsonUtil::ToString(v);
}

void OrganizationApplyInfo::setId(const int64_t& v) {
    m_id = v;
}

void OrganizationApplyInfo::setUserId(const int64_t& v) {
    m_userId = v;
}

void OrganizationApplyInfo::setOrgName(const std::string& v) {
    m_orgName = v;
}

void OrganizationApplyInfo::setOrgType(const std::string& v) {
    m_orgType = v;
}

void OrganizationApplyInfo::setOrgDescription(const std::string& v) {
    m_orgDescription = v;
}

void OrganizationApplyInfo::setStatus(const int32_t& v) {
    m_status = v;
}

void OrganizationApplyInfo::setReviewReason(const std::string& v) {
    m_reviewReason = v;
}

void OrganizationApplyInfo::setCreatedAt(const int64_t& v) {
    m_createdAt = v;
}

void OrganizationApplyInfo::setReviewedAt(const int64_t& v) {
    m_reviewedAt = v;
}

void OrganizationApplyInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}


int OrganizationApplyInfoDao::Update(OrganizationApplyInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update organization_apply set user_id = ?, org_name = ?, org_type = ?, org_description = ?, status = ?, review_reason = ?, created_at = ?, reviewed_at = ?, is_deleted = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_userId);
    stmt->bindString(2, info->m_orgName);
    stmt->bindString(3, info->m_orgType);
    stmt->bindString(4, info->m_orgDescription);
    stmt->bindInt32(5, info->m_status);
    stmt->bindString(6, info->m_reviewReason);
    stmt->bindInt64(7, info->m_createdAt);
    stmt->bindInt64(8, info->m_reviewedAt);
    stmt->bindInt32(9, info->m_isDeleted);
    stmt->bindInt64(10, info->m_id);
    return stmt->execute();
}

int OrganizationApplyInfoDao::Insert(OrganizationApplyInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into organization_apply (user_id, org_name, org_type, org_description, status, review_reason, created_at, reviewed_at, is_deleted) values (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_userId);
    stmt->bindString(2, info->m_orgName);
    stmt->bindString(3, info->m_orgType);
    stmt->bindString(4, info->m_orgDescription);
    stmt->bindInt32(5, info->m_status);
    stmt->bindString(6, info->m_reviewReason);
    stmt->bindInt64(7, info->m_createdAt);
    stmt->bindInt64(8, info->m_reviewedAt);
    stmt->bindInt32(9, info->m_isDeleted);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int OrganizationApplyInfoDao::InsertOrUpdate(OrganizationApplyInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into organization_apply (id, user_id, org_name, org_type, org_description, status, review_reason, created_at, reviewed_at, is_deleted) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_userId);
    stmt->bindString(3, info->m_orgName);
    stmt->bindString(4, info->m_orgType);
    stmt->bindString(5, info->m_orgDescription);
    stmt->bindInt32(6, info->m_status);
    stmt->bindString(7, info->m_reviewReason);
    stmt->bindInt64(8, info->m_createdAt);
    stmt->bindInt64(9, info->m_reviewedAt);
    stmt->bindInt32(10, info->m_isDeleted);
    return stmt->execute();
}

int OrganizationApplyInfoDao::Delete(OrganizationApplyInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from organization_apply where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int OrganizationApplyInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from organization_apply where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int OrganizationApplyInfoDao::DeleteByUserId( const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "delete from organization_apply where user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    return stmt->execute();
}

int OrganizationApplyInfoDao::DeleteByStatus( const int32_t& status, chen::IDB::ptr conn) {
    std::string sql = "delete from organization_apply where status = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt32(1, status);
    return stmt->execute();
}

int OrganizationApplyInfoDao::QueryAll(std::vector<OrganizationApplyInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, org_name, org_type, org_description, status, review_reason, created_at, reviewed_at, is_deleted from organization_apply";
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
        OrganizationApplyInfo::ptr v(new OrganizationApplyInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_orgName = rt->getString(2);
        v->m_orgType = rt->getString(3);
        v->m_orgDescription = rt->getString(4);
        v->m_status = rt->getInt32(5);
        v->m_reviewReason = rt->getString(6);
        v->m_createdAt = rt->getInt64(7);
        v->m_reviewedAt = rt->getInt64(8);
        v->m_isDeleted = rt->getInt32(9);
        results.push_back(v);
    }
    return 0;
}

OrganizationApplyInfo::ptr OrganizationApplyInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, org_name, org_type, org_description, status, review_reason, created_at, reviewed_at, is_deleted from organization_apply where id = ?";
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
    OrganizationApplyInfo::ptr v(new OrganizationApplyInfo);
    v->m_id = rt->getInt64(0);
    v->m_userId = rt->getInt64(1);
    v->m_orgName = rt->getString(2);
    v->m_orgType = rt->getString(3);
    v->m_orgDescription = rt->getString(4);
    v->m_status = rt->getInt32(5);
    v->m_reviewReason = rt->getString(6);
    v->m_createdAt = rt->getInt64(7);
    v->m_reviewedAt = rt->getInt64(8);
    v->m_isDeleted = rt->getInt32(9);
    return v;
}

int OrganizationApplyInfoDao::QueryByUserId(std::vector<OrganizationApplyInfo::ptr>& results,  const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, org_name, org_type, org_description, status, review_reason, created_at, reviewed_at, is_deleted from organization_apply where user_id = ?";
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
        OrganizationApplyInfo::ptr v(new OrganizationApplyInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_orgName = rt->getString(2);
        v->m_orgType = rt->getString(3);
        v->m_orgDescription = rt->getString(4);
        v->m_status = rt->getInt32(5);
        v->m_reviewReason = rt->getString(6);
        v->m_createdAt = rt->getInt64(7);
        v->m_reviewedAt = rt->getInt64(8);
        v->m_isDeleted = rt->getInt32(9);
        results.push_back(v);
    };
    return 0;
}

int OrganizationApplyInfoDao::QueryByStatus(std::vector<OrganizationApplyInfo::ptr>& results,  const int32_t& status, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, org_name, org_type, org_description, status, review_reason, created_at, reviewed_at, is_deleted from organization_apply where status = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt32(1, status);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        OrganizationApplyInfo::ptr v(new OrganizationApplyInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_orgName = rt->getString(2);
        v->m_orgType = rt->getString(3);
        v->m_orgDescription = rt->getString(4);
        v->m_status = rt->getInt32(5);
        v->m_reviewReason = rt->getString(6);
        v->m_createdAt = rt->getInt64(7);
        v->m_reviewedAt = rt->getInt64(8);
        v->m_isDeleted = rt->getInt32(9);
        results.push_back(v);
    };
    return 0;
}

int OrganizationApplyInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS organization_apply("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "user_id INTEGER NOT NULL DEFAULT 0,"
            "org_name TEXT NOT NULL DEFAULT '',"
            "org_type TEXT NOT NULL DEFAULT '',"
            "org_description TEXT NOT NULL DEFAULT '',"
            "status INTEGER NOT NULL DEFAULT 0,"
            "review_reason TEXT NOT NULL DEFAULT '',"
            "created_at INTEGER NOT NULL DEFAULT 0,"
            "reviewed_at INTEGER NOT NULL DEFAULT 0,"
            "is_deleted INTEGER NOT NULL DEFAULT 0);"
            "CREATE INDEX IF NOT EXISTS organization_apply_user_id ON organization_apply(user_id);"
            "CREATE INDEX IF NOT EXISTS organization_apply_status ON organization_apply(status);"
            );
}

int OrganizationApplyInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS organization_apply("
            "`id` bigint AUTO_INCREMENT COMMENT '申请ID',"
            "`user_id` bigint NOT NULL DEFAULT 0 COMMENT '申请人用户ID',"
            "`org_name` varchar(100) NOT NULL DEFAULT '' COMMENT '组织名称',"
            "`org_type` varchar(50) NOT NULL DEFAULT '' COMMENT '组织类型',"
            "`org_description` varchar(500) NOT NULL DEFAULT '' COMMENT '组织描述',"
            "`status` int NOT NULL DEFAULT 0 COMMENT '状态: 0待审核 1已通过 2已拒绝',"
            "`review_reason` varchar(500) NOT NULL DEFAULT '' COMMENT '审核备注',"
            "`created_at` bigint NOT NULL DEFAULT 0 COMMENT '申请时间(unix timestamp)',"
            "`reviewed_at` bigint NOT NULL DEFAULT 0 COMMENT '审核时间(unix timestamp)',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "PRIMARY KEY(`id`),"
            "KEY `organization_apply_user_id` (`user_id`),"
            "KEY `organization_apply_status` (`status`)) COMMENT='组织创建申请表'");
}

int OrganizationApplyInfoDao::MigrateTableSQLite3(chen::IDB::ptr conn) {
    auto data = conn->query("PRAGMA table_info(organization_apply)");
    if (!data) {
        ERROR(logger) << "PRAGMA table_info(organization_apply) errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
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
            INFO(logger) << "Column type changed: organization_apply.id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("user_id");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: organization_apply.user_id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("org_name");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: organization_apply.org_name " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("org_type");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: organization_apply.org_type " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("org_description");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: organization_apply.org_description " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("status");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: organization_apply.status " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("review_reason");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: organization_apply.review_reason " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("created_at");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: organization_apply.created_at " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("reviewed_at");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: organization_apply.reviewed_at " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("is_deleted");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: organization_apply.is_deleted " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    if (!need_recreate) {
        for (auto& [name, _] : existing_cols) {
            (void)_;  // suppress unused warning
            bool found = false;
            if (name == "id") found = true;
            if (name == "user_id") found = true;
            if (name == "org_name") found = true;
            if (name == "org_type") found = true;
            if (name == "org_description") found = true;
            if (name == "status") found = true;
            if (name == "review_reason") found = true;
            if (name == "created_at") found = true;
            if (name == "reviewed_at") found = true;
            if (name == "is_deleted") found = true;
            if (!found) {
                need_recreate = true;
                WARN(logger) << "Column organization_apply." << name << " removed, table recreate required";
                break;
            }
        }
    }

    if (need_recreate) {
        INFO(logger) << "Recreating table organization_apply";

        std::vector<std::string> common_cols;
        if (existing_cols.find("id") != existing_cols.end()) {
            common_cols.push_back("id");
        }
        if (existing_cols.find("user_id") != existing_cols.end()) {
            common_cols.push_back("user_id");
        }
        if (existing_cols.find("org_name") != existing_cols.end()) {
            common_cols.push_back("org_name");
        }
        if (existing_cols.find("org_type") != existing_cols.end()) {
            common_cols.push_back("org_type");
        }
        if (existing_cols.find("org_description") != existing_cols.end()) {
            common_cols.push_back("org_description");
        }
        if (existing_cols.find("status") != existing_cols.end()) {
            common_cols.push_back("status");
        }
        if (existing_cols.find("review_reason") != existing_cols.end()) {
            common_cols.push_back("review_reason");
        }
        if (existing_cols.find("created_at") != existing_cols.end()) {
            common_cols.push_back("created_at");
        }
        if (existing_cols.find("reviewed_at") != existing_cols.end()) {
            common_cols.push_back("reviewed_at");
        }
        if (existing_cols.find("is_deleted") != existing_cols.end()) {
            common_cols.push_back("is_deleted");
        }

        if (conn->execute("ALTER TABLE organization_apply RENAME TO organization_apply_tmp")) {
            ERROR(logger) << "RENAME TABLE organization_apply failed";
            return conn->getErrno();
        }
        CreateTableSQLite3(conn);
        if (!common_cols.empty()) {
            std::string cols;
            for (size_t i = 0; i < common_cols.size(); ++i) {
                if (i) cols += ",";
                cols += common_cols[i];
            }
            std::string sql = "INSERT INTO organization_apply (" + cols + ") SELECT " + cols + " FROM organization_apply_tmp";
            if (int rt = conn->execute(sql)) {
                ERROR(logger) << "copy data from organization_apply_tmp to organization_apply failed, errno=" << rt;
                // don't return; try to continue
            }
        }
        conn->execute("DROP TABLE organization_apply_tmp");
        return 0;
    }

    if (existing_cols.find("user_id") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.user_id";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN user_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN user_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("org_name") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.org_name";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN org_name TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN org_name failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("org_type") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.org_type";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN org_type TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN org_type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("org_description") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.org_description";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN org_description TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN org_description failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("status") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.status";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN status INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN status failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("review_reason") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.review_reason";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN review_reason TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN review_reason failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("created_at") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.created_at";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN created_at INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN created_at failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("reviewed_at") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.reviewed_at";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN reviewed_at INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN reviewed_at failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.is_deleted";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    return 0;
}

int OrganizationApplyInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM organization_apply");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM organization_apply errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::map<std::string, std::string> existing_cols;  // name -> type
    while (data->next()) {
        existing_cols[data->getString(0)] = data->getString(1);
    }

    {
        auto it = existing_cols.find("id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column organization_apply.id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE organization_apply MODIFY COLUMN `id` bigint NOT NULL DEFAULT 0 COMMENT '申请ID'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_apply.id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("user_id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column organization_apply.user_id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE organization_apply MODIFY COLUMN `user_id` bigint NOT NULL DEFAULT 0 COMMENT '申请人用户ID'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_apply.user_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("org_name");
        if (it != existing_cols.end() && it->second != "varchar(100)") {
            INFO(logger) << "Modifying column organization_apply.org_name " << it->second << " -> varchar(100)";
            int rt = conn->execute("ALTER TABLE organization_apply MODIFY COLUMN `org_name` varchar(100) NOT NULL DEFAULT '' COMMENT '组织名称'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_apply.org_name failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("org_type");
        if (it != existing_cols.end() && it->second != "varchar(50)") {
            INFO(logger) << "Modifying column organization_apply.org_type " << it->second << " -> varchar(50)";
            int rt = conn->execute("ALTER TABLE organization_apply MODIFY COLUMN `org_type` varchar(50) NOT NULL DEFAULT '' COMMENT '组织类型'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_apply.org_type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("org_description");
        if (it != existing_cols.end() && it->second != "varchar(500)") {
            INFO(logger) << "Modifying column organization_apply.org_description " << it->second << " -> varchar(500)";
            int rt = conn->execute("ALTER TABLE organization_apply MODIFY COLUMN `org_description` varchar(500) NOT NULL DEFAULT '' COMMENT '组织描述'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_apply.org_description failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("status");
        if (it != existing_cols.end() && it->second != "int") {
            INFO(logger) << "Modifying column organization_apply.status " << it->second << " -> int";
            int rt = conn->execute("ALTER TABLE organization_apply MODIFY COLUMN `status` int NOT NULL DEFAULT 0 COMMENT '状态: 0待审核 1已通过 2已拒绝'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_apply.status failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("review_reason");
        if (it != existing_cols.end() && it->second != "varchar(500)") {
            INFO(logger) << "Modifying column organization_apply.review_reason " << it->second << " -> varchar(500)";
            int rt = conn->execute("ALTER TABLE organization_apply MODIFY COLUMN `review_reason` varchar(500) NOT NULL DEFAULT '' COMMENT '审核备注'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_apply.review_reason failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("created_at");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column organization_apply.created_at " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE organization_apply MODIFY COLUMN `created_at` bigint NOT NULL DEFAULT 0 COMMENT '申请时间(unix timestamp)'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_apply.created_at failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("reviewed_at");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column organization_apply.reviewed_at " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE organization_apply MODIFY COLUMN `reviewed_at` bigint NOT NULL DEFAULT 0 COMMENT '审核时间(unix timestamp)'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_apply.reviewed_at failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("is_deleted");
        if (it != existing_cols.end() && it->second != "int") {
            INFO(logger) << "Modifying column organization_apply.is_deleted " << it->second << " -> int";
            int rt = conn->execute("ALTER TABLE organization_apply MODIFY COLUMN `is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN organization_apply.is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    for (auto& [name, _] : existing_cols) {
        (void)_;
        bool found = false;
        if (name == "id") found = true;
        if (name == "user_id") found = true;
        if (name == "org_name") found = true;
        if (name == "org_type") found = true;
        if (name == "org_description") found = true;
        if (name == "status") found = true;
        if (name == "review_reason") found = true;
        if (name == "created_at") found = true;
        if (name == "reviewed_at") found = true;
        if (name == "is_deleted") found = true;
        if (!found) {
            WARN(logger) << "Dropping column organization_apply." << name << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE organization_apply DROP COLUMN `" + name + "`");
            if (rt) {
                ERROR(logger) << "DROP COLUMN organization_apply." << name << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    if (existing_cols.find("user_id") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.user_id";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN `user_id` bigint NOT NULL DEFAULT 0 COMMENT '申请人用户ID'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN user_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("org_name") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.org_name";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN `org_name` varchar(100) NOT NULL DEFAULT '' COMMENT '组织名称'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN org_name failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("org_type") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.org_type";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN `org_type` varchar(50) NOT NULL DEFAULT '' COMMENT '组织类型'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN org_type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("org_description") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.org_description";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN `org_description` varchar(500) NOT NULL DEFAULT '' COMMENT '组织描述'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN org_description failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("status") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.status";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN `status` int NOT NULL DEFAULT 0 COMMENT '状态: 0待审核 1已通过 2已拒绝'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN status failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("review_reason") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.review_reason";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN `review_reason` varchar(500) NOT NULL DEFAULT '' COMMENT '审核备注'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN review_reason failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("created_at") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.created_at";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN `created_at` bigint NOT NULL DEFAULT 0 COMMENT '申请时间(unix timestamp)'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN created_at failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("reviewed_at") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.reviewed_at";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN `reviewed_at` bigint NOT NULL DEFAULT 0 COMMENT '审核时间(unix timestamp)'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN reviewed_at failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column organization_apply.is_deleted";
        int rt = conn->execute("ALTER TABLE organization_apply ADD COLUMN `is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE organization_apply ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    return 0;
}


} //namespace data
} //namespace blog
