#include "organization_user_rel_info.h"
#include "chen/log/log.h"

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
            "`role` int NOT NULL DEFAULT 1 COMMENT '角色: 1成员 2管理员 3拥有者',"
            "`status` int NOT NULL DEFAULT 1 COMMENT '状态: 0申请中 1已加入 2已拒绝 3已退出',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '加入时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "UNIQUE KEY `organization_user_rel_org_id_user_id` (`org_id`,`user_id`),"
            "KEY `organization_user_rel_user_id` (`user_id`)) COMMENT='组织用户关联表'");
}
} //namespace data
} //namespace blog
