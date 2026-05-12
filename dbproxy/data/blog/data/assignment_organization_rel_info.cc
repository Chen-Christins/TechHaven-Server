#include "assignment_organization_rel_info.h"
#include "chen/log/log.h"

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

AssignmentOrganizationRelInfo::AssignmentOrganizationRelInfo()
    :m_status(1)
    ,m_isDeleted(0)
    ,m_id()
    ,m_assignmentId()
    ,m_organizationId()
    ,m_assignedBy()
    ,m_createTime(time(0))
    ,m_updateTime() {
}

std::string AssignmentOrganizationRelInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["assignment_id"] = std::to_string(m_assignmentId);
    v["organization_id"] = std::to_string(m_organizationId);
    v["assigned_by"] = m_assignedBy;
    v["status"] = m_status;
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void AssignmentOrganizationRelInfo::setId(const int64_t& v) {
    m_id = v;
}

void AssignmentOrganizationRelInfo::setAssignmentId(const int64_t& v) {
    m_assignmentId = v;
}

void AssignmentOrganizationRelInfo::setOrganizationId(const int64_t& v) {
    m_organizationId = v;
}

void AssignmentOrganizationRelInfo::setAssignedBy(const std::string& v) {
    m_assignedBy = v;
}

void AssignmentOrganizationRelInfo::setStatus(const int32_t& v) {
    m_status = v;
}

void AssignmentOrganizationRelInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void AssignmentOrganizationRelInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void AssignmentOrganizationRelInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int AssignmentOrganizationRelInfoDao::Update(AssignmentOrganizationRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update assignment_organization_rel set assignment_id = ?, organization_id = ?, assigned_by = ?, status = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_assignmentId);
    stmt->bindInt64(2, info->m_organizationId);
    stmt->bindString(3, info->m_assignedBy);
    stmt->bindInt32(4, info->m_status);
    stmt->bindInt32(5, info->m_isDeleted);
    stmt->bindTime(6, info->m_createTime);
    stmt->bindTime(7, info->m_updateTime);
    stmt->bindInt64(8, info->m_id);
    return stmt->execute();
}

int AssignmentOrganizationRelInfoDao::Insert(AssignmentOrganizationRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into assignment_organization_rel (assignment_id, organization_id, assigned_by, status, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_assignmentId);
    stmt->bindInt64(2, info->m_organizationId);
    stmt->bindString(3, info->m_assignedBy);
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

int AssignmentOrganizationRelInfoDao::InsertOrUpdate(AssignmentOrganizationRelInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into assignment_organization_rel (id, assignment_id, organization_id, assigned_by, status, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_assignmentId);
    stmt->bindInt64(3, info->m_organizationId);
    stmt->bindString(4, info->m_assignedBy);
    stmt->bindInt32(5, info->m_status);
    stmt->bindInt32(6, info->m_isDeleted);
    stmt->bindTime(7, info->m_createTime);
    stmt->bindTime(8, info->m_updateTime);
    return stmt->execute();
}

int AssignmentOrganizationRelInfoDao::Delete(AssignmentOrganizationRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from assignment_organization_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int AssignmentOrganizationRelInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from assignment_organization_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int AssignmentOrganizationRelInfoDao::DeleteByAssignmentIdOrganizationId( const int64_t& assignment_id,  const int64_t& organization_id, chen::IDB::ptr conn) {
    std::string sql = "delete from assignment_organization_rel where assignment_id = ? and organization_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, assignment_id);
    stmt->bindInt64(1, organization_id);
    return stmt->execute();
}

int AssignmentOrganizationRelInfoDao::DeleteByAssignmentId( const int64_t& assignment_id, chen::IDB::ptr conn) {
    std::string sql = "delete from assignment_organization_rel where assignment_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, assignment_id);
    return stmt->execute();
}

int AssignmentOrganizationRelInfoDao::DeleteByOrganizationId( const int64_t& organization_id, chen::IDB::ptr conn) {
    std::string sql = "delete from assignment_organization_rel where organization_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, organization_id);
    return stmt->execute();
}

int AssignmentOrganizationRelInfoDao::QueryAll(std::vector<AssignmentOrganizationRelInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, assignment_id, organization_id, assigned_by, status, is_deleted, create_time, update_time from assignment_organization_rel";
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
        AssignmentOrganizationRelInfo::ptr v(new AssignmentOrganizationRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_assignmentId = rt->getInt64(1);
        v->m_organizationId = rt->getInt64(2);
        v->m_assignedBy = rt->getString(3);
        v->m_status = rt->getInt32(4);
        v->m_isDeleted = rt->getInt32(5);
        v->m_createTime = rt->getTime(6);
        v->m_updateTime = rt->getTime(7);
        results.push_back(v);
    }
    return 0;
}

AssignmentOrganizationRelInfo::ptr AssignmentOrganizationRelInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, assignment_id, organization_id, assigned_by, status, is_deleted, create_time, update_time from assignment_organization_rel where id = ?";
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
    AssignmentOrganizationRelInfo::ptr v(new AssignmentOrganizationRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_assignmentId = rt->getInt64(1);
    v->m_organizationId = rt->getInt64(2);
    v->m_assignedBy = rt->getString(3);
    v->m_status = rt->getInt32(4);
    v->m_isDeleted = rt->getInt32(5);
    v->m_createTime = rt->getTime(6);
    v->m_updateTime = rt->getTime(7);
    return v;
}

AssignmentOrganizationRelInfo::ptr AssignmentOrganizationRelInfoDao::QueryByAssignmentIdOrganizationId( const int64_t& assignment_id,  const int64_t& organization_id, chen::IDB::ptr conn) {
    std::string sql = "select id, assignment_id, organization_id, assigned_by, status, is_deleted, create_time, update_time from assignment_organization_rel where assignment_id = ? and organization_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindInt64(1, assignment_id);
    stmt->bindInt64(2, organization_id);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    AssignmentOrganizationRelInfo::ptr v(new AssignmentOrganizationRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_assignmentId = rt->getInt64(1);
    v->m_organizationId = rt->getInt64(2);
    v->m_assignedBy = rt->getString(3);
    v->m_status = rt->getInt32(4);
    v->m_isDeleted = rt->getInt32(5);
    v->m_createTime = rt->getTime(6);
    v->m_updateTime = rt->getTime(7);
    return v;
}

int AssignmentOrganizationRelInfoDao::QueryByAssignmentId(std::vector<AssignmentOrganizationRelInfo::ptr>& results,  const int64_t& assignment_id, chen::IDB::ptr conn) {
    std::string sql = "select id, assignment_id, organization_id, assigned_by, status, is_deleted, create_time, update_time from assignment_organization_rel where assignment_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, assignment_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        AssignmentOrganizationRelInfo::ptr v(new AssignmentOrganizationRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_assignmentId = rt->getInt64(1);
        v->m_organizationId = rt->getInt64(2);
        v->m_assignedBy = rt->getString(3);
        v->m_status = rt->getInt32(4);
        v->m_isDeleted = rt->getInt32(5);
        v->m_createTime = rt->getTime(6);
        v->m_updateTime = rt->getTime(7);
        results.push_back(v);
    };
    return 0;
}

int AssignmentOrganizationRelInfoDao::QueryByOrganizationId(std::vector<AssignmentOrganizationRelInfo::ptr>& results,  const int64_t& organization_id, chen::IDB::ptr conn) {
    std::string sql = "select id, assignment_id, organization_id, assigned_by, status, is_deleted, create_time, update_time from assignment_organization_rel where organization_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, organization_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        AssignmentOrganizationRelInfo::ptr v(new AssignmentOrganizationRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_assignmentId = rt->getInt64(1);
        v->m_organizationId = rt->getInt64(2);
        v->m_assignedBy = rt->getString(3);
        v->m_status = rt->getInt32(4);
        v->m_isDeleted = rt->getInt32(5);
        v->m_createTime = rt->getTime(6);
        v->m_updateTime = rt->getTime(7);
        results.push_back(v);
    };
    return 0;
}

int AssignmentOrganizationRelInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS assignment_organization_rel("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "assignment_id INTEGER NOT NULL DEFAULT 0,"
            "organization_id INTEGER NOT NULL DEFAULT 0,"
            "assigned_by TEXT NOT NULL DEFAULT '',"
            "status INTEGER NOT NULL DEFAULT 1,"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE UNIQUE INDEX IF NOT EXISTS assignment_organization_rel_assignment_id_organization_id ON assignment_organization_rel(assignment_id,organization_id);"
            "CREATE INDEX IF NOT EXISTS assignment_organization_rel_assignment_id ON assignment_organization_rel(assignment_id);"
            "CREATE INDEX IF NOT EXISTS assignment_organization_rel_organization_id ON assignment_organization_rel(organization_id);"
            );
}

int AssignmentOrganizationRelInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS assignment_organization_rel("
            "`id` bigint AUTO_INCREMENT COMMENT '主键ID',"
            "`assignment_id` bigint NOT NULL DEFAULT 0 COMMENT '作业ID',"
            "`organization_id` bigint NOT NULL DEFAULT 0 COMMENT '组织ID',"
            "`assigned_by` varchar(128) NOT NULL DEFAULT '' COMMENT '负责人',"
            "`status` int NOT NULL DEFAULT 1 COMMENT '状态: 1分配 2取消',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "UNIQUE KEY `assignment_organization_rel_assignment_id_organization_id` (`assignment_id`,`organization_id`),"
            "KEY `assignment_organization_rel_assignment_id` (`assignment_id`),"
            "KEY `assignment_organization_rel_organization_id` (`organization_id`)) COMMENT='作业-组织关联表'");
}
} //namespace data
} //namespace blog
