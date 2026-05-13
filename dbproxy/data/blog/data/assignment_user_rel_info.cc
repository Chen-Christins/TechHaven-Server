#include "assignment_user_rel_info.h"
#include "chen/log/log.h"

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

AssignmentUserRelInfo::AssignmentUserRelInfo()
    :m_status(1)
    ,m_score()
    ,m_isDeleted(0)
    ,m_id()
    ,m_assignmentId()
    ,m_userId()
    ,m_submitTime()
    ,m_createTime(time(0))
    ,m_updateTime() {
}

std::string AssignmentUserRelInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["assignment_id"] = std::to_string(m_assignmentId);
    v["user_id"] = std::to_string(m_userId);
    v["status"] = m_status;
    v["score"] = m_score;
    v["submit_time"] = chen::Time2Str(m_submitTime);
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void AssignmentUserRelInfo::setId(const int64_t& v) {
    m_id = v;
}

void AssignmentUserRelInfo::setAssignmentId(const int64_t& v) {
    m_assignmentId = v;
}

void AssignmentUserRelInfo::setUserId(const int64_t& v) {
    m_userId = v;
}

void AssignmentUserRelInfo::setStatus(const int32_t& v) {
    m_status = v;
}

void AssignmentUserRelInfo::setScore(const int32_t& v) {
    m_score = v;
}

void AssignmentUserRelInfo::setSubmitTime(const int64_t& v) {
    m_submitTime = v;
}

void AssignmentUserRelInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void AssignmentUserRelInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void AssignmentUserRelInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int AssignmentUserRelInfoDao::Update(AssignmentUserRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update assignment_user_rel set assignment_id = ?, user_id = ?, status = ?, score = ?, submit_time = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_assignmentId);
    stmt->bindInt64(2, info->m_userId);
    stmt->bindInt32(3, info->m_status);
    stmt->bindInt32(4, info->m_score);
    stmt->bindTime(5, info->m_submitTime);
    stmt->bindInt32(6, info->m_isDeleted);
    stmt->bindTime(7, info->m_createTime);
    stmt->bindTime(8, info->m_updateTime);
    stmt->bindInt64(9, info->m_id);
    return stmt->execute();
}

int AssignmentUserRelInfoDao::Insert(AssignmentUserRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into assignment_user_rel (assignment_id, user_id, status, score, submit_time, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_assignmentId);
    stmt->bindInt64(2, info->m_userId);
    stmt->bindInt32(3, info->m_status);
    stmt->bindInt32(4, info->m_score);
    stmt->bindTime(5, info->m_submitTime);
    stmt->bindInt32(6, info->m_isDeleted);
    stmt->bindTime(7, info->m_createTime);
    stmt->bindTime(8, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int AssignmentUserRelInfoDao::InsertOrUpdate(AssignmentUserRelInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into assignment_user_rel (id, assignment_id, user_id, status, score, submit_time, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_assignmentId);
    stmt->bindInt64(3, info->m_userId);
    stmt->bindInt32(4, info->m_status);
    stmt->bindInt32(5, info->m_score);
    stmt->bindTime(6, info->m_submitTime);
    stmt->bindInt32(7, info->m_isDeleted);
    stmt->bindTime(8, info->m_createTime);
    stmt->bindTime(9, info->m_updateTime);
    return stmt->execute();
}

int AssignmentUserRelInfoDao::Delete(AssignmentUserRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from assignment_user_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int AssignmentUserRelInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from assignment_user_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int AssignmentUserRelInfoDao::DeleteByAssignmentIdUserId( const int64_t& assignment_id,  const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "delete from assignment_user_rel where assignment_id = ? and user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, assignment_id);
    stmt->bindInt64(1, user_id);
    return stmt->execute();
}

int AssignmentUserRelInfoDao::DeleteByAssignmentId( const int64_t& assignment_id, chen::IDB::ptr conn) {
    std::string sql = "delete from assignment_user_rel where assignment_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, assignment_id);
    return stmt->execute();
}

int AssignmentUserRelInfoDao::DeleteByUserId( const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "delete from assignment_user_rel where user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    return stmt->execute();
}

int AssignmentUserRelInfoDao::QueryAll(std::vector<AssignmentUserRelInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, assignment_id, user_id, status, score, submit_time, is_deleted, create_time, update_time from assignment_user_rel";
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
        AssignmentUserRelInfo::ptr v(new AssignmentUserRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_assignmentId = rt->getInt64(1);
        v->m_userId = rt->getInt64(2);
        v->m_status = rt->getInt32(3);
        v->m_score = rt->getInt32(4);
        v->m_submitTime = rt->getTime(5);
        v->m_isDeleted = rt->getInt32(6);
        v->m_createTime = rt->getTime(7);
        v->m_updateTime = rt->getTime(8);
        results.push_back(v);
    }
    return 0;
}

AssignmentUserRelInfo::ptr AssignmentUserRelInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, assignment_id, user_id, status, score, submit_time, is_deleted, create_time, update_time from assignment_user_rel where id = ?";
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
    AssignmentUserRelInfo::ptr v(new AssignmentUserRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_assignmentId = rt->getInt64(1);
    v->m_userId = rt->getInt64(2);
    v->m_status = rt->getInt32(3);
    v->m_score = rt->getInt32(4);
    v->m_submitTime = rt->getTime(5);
    v->m_isDeleted = rt->getInt32(6);
    v->m_createTime = rt->getTime(7);
    v->m_updateTime = rt->getTime(8);
    return v;
}

AssignmentUserRelInfo::ptr AssignmentUserRelInfoDao::QueryByAssignmentIdUserId( const int64_t& assignment_id,  const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "select id, assignment_id, user_id, status, score, submit_time, is_deleted, create_time, update_time from assignment_user_rel where assignment_id = ? and user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindInt64(1, assignment_id);
    stmt->bindInt64(2, user_id);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    AssignmentUserRelInfo::ptr v(new AssignmentUserRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_assignmentId = rt->getInt64(1);
    v->m_userId = rt->getInt64(2);
    v->m_status = rt->getInt32(3);
    v->m_score = rt->getInt32(4);
    v->m_submitTime = rt->getTime(5);
    v->m_isDeleted = rt->getInt32(6);
    v->m_createTime = rt->getTime(7);
    v->m_updateTime = rt->getTime(8);
    return v;
}

int AssignmentUserRelInfoDao::QueryByAssignmentId(std::vector<AssignmentUserRelInfo::ptr>& results,  const int64_t& assignment_id, chen::IDB::ptr conn) {
    std::string sql = "select id, assignment_id, user_id, status, score, submit_time, is_deleted, create_time, update_time from assignment_user_rel where assignment_id = ?";
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
        AssignmentUserRelInfo::ptr v(new AssignmentUserRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_assignmentId = rt->getInt64(1);
        v->m_userId = rt->getInt64(2);
        v->m_status = rt->getInt32(3);
        v->m_score = rt->getInt32(4);
        v->m_submitTime = rt->getTime(5);
        v->m_isDeleted = rt->getInt32(6);
        v->m_createTime = rt->getTime(7);
        v->m_updateTime = rt->getTime(8);
        results.push_back(v);
    };
    return 0;
}

int AssignmentUserRelInfoDao::QueryByUserId(std::vector<AssignmentUserRelInfo::ptr>& results,  const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "select id, assignment_id, user_id, status, score, submit_time, is_deleted, create_time, update_time from assignment_user_rel where user_id = ?";
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
        AssignmentUserRelInfo::ptr v(new AssignmentUserRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_assignmentId = rt->getInt64(1);
        v->m_userId = rt->getInt64(2);
        v->m_status = rt->getInt32(3);
        v->m_score = rt->getInt32(4);
        v->m_submitTime = rt->getTime(5);
        v->m_isDeleted = rt->getInt32(6);
        v->m_createTime = rt->getTime(7);
        v->m_updateTime = rt->getTime(8);
        results.push_back(v);
    };
    return 0;
}

int AssignmentUserRelInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS assignment_user_rel("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "assignment_id INTEGER NOT NULL DEFAULT 0,"
            "user_id INTEGER NOT NULL DEFAULT 0,"
            "status INTEGER NOT NULL DEFAULT 1,"
            "score INTEGER NOT NULL DEFAULT 0,"
            "submit_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE UNIQUE INDEX IF NOT EXISTS assignment_user_rel_assignment_id_user_id ON assignment_user_rel(assignment_id,user_id);"
            "CREATE INDEX IF NOT EXISTS assignment_user_rel_assignment_id ON assignment_user_rel(assignment_id);"
            "CREATE INDEX IF NOT EXISTS assignment_user_rel_user_id ON assignment_user_rel(user_id);"
            );
}

int AssignmentUserRelInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS assignment_user_rel("
            "`id` bigint AUTO_INCREMENT COMMENT '主键ID',"
            "`assignment_id` bigint NOT NULL DEFAULT 0 COMMENT '作业ID',"
            "`user_id` bigint NOT NULL DEFAULT 0 COMMENT '学生用户ID',"
            "`status` int NOT NULL DEFAULT 1 COMMENT '状态: 1提交 2批改 3迟交',"
            "`score` int NOT NULL DEFAULT 0 COMMENT '成绩',"
            "`submit_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '提交时间',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "UNIQUE KEY `assignment_user_rel_assignment_id_user_id` (`assignment_id`,`user_id`),"
            "KEY `assignment_user_rel_assignment_id` (`assignment_id`),"
            "KEY `assignment_user_rel_user_id` (`user_id`)) COMMENT='作业-学生关联表'");
}
} //namespace data
} //namespace blog
