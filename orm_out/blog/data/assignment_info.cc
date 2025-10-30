#include "assignment_info.h"
#include "chen/log/log.h"

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

AssignmentInfo::AssignmentInfo()
    :m_isDeleted()
    ,m_id()
    ,m_subjectId()
    ,m_name()
    ,m_color()
    ,m_content()
    ,m_deadline()
    ,m_createTime()
    ,m_updateTime() {
}

std::string AssignmentInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["subject_id"] = std::to_string(m_subjectId);
    v["name"] = m_name;
    v["color"] = m_color;
    v["content"] = m_content;
    v["deadline"] = chen::Time2Str(m_deadline);
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void AssignmentInfo::setId(const int64_t& v) {
    m_id = v;
}

void AssignmentInfo::setSubjectId(const int64_t& v) {
    m_subjectId = v;
}

void AssignmentInfo::setName(const std::string& v) {
    m_name = v;
}

void AssignmentInfo::setColor(const std::string& v) {
    m_color = v;
}

void AssignmentInfo::setContent(const std::string& v) {
    m_content = v;
}

void AssignmentInfo::setDeadline(const int64_t& v) {
    m_deadline = v;
}

void AssignmentInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void AssignmentInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void AssignmentInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int AssignmentInfoDao::Update(AssignmentInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update assignment set subject_id = ?, name = ?, color = ?, content = ?, deadline = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_subjectId);
    stmt->bindString(2, info->m_name);
    stmt->bindString(3, info->m_color);
    stmt->bindString(4, info->m_content);
    stmt->bindTime(5, info->m_deadline);
    stmt->bindInt32(6, info->m_isDeleted);
    stmt->bindTime(7, info->m_createTime);
    stmt->bindTime(8, info->m_updateTime);
    stmt->bindInt64(9, info->m_id);
    return stmt->execute();
}

int AssignmentInfoDao::Insert(AssignmentInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into assignment (subject_id, name, color, content, deadline, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_subjectId);
    stmt->bindString(2, info->m_name);
    stmt->bindString(3, info->m_color);
    stmt->bindString(4, info->m_content);
    stmt->bindTime(5, info->m_deadline);
    stmt->bindInt32(6, info->m_isDeleted);
    stmt->bindTime(7, info->m_createTime);
    stmt->bindTime(8, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int AssignmentInfoDao::InsertOrUpdate(AssignmentInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into assignment (id, subject_id, name, color, content, deadline, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_subjectId);
    stmt->bindString(3, info->m_name);
    stmt->bindString(4, info->m_color);
    stmt->bindString(5, info->m_content);
    stmt->bindTime(6, info->m_deadline);
    stmt->bindInt32(7, info->m_isDeleted);
    stmt->bindTime(8, info->m_createTime);
    stmt->bindTime(9, info->m_updateTime);
    return stmt->execute();
}

int AssignmentInfoDao::Delete(AssignmentInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from assignment where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int AssignmentInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from assignment where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int AssignmentInfoDao::DeleteBySubjectId( const int64_t& subject_id, chen::IDB::ptr conn) {
    std::string sql = "delete from assignment where subject_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, subject_id);
    return stmt->execute();
}

int AssignmentInfoDao::DeleteBySubjectIdName( const int64_t& subject_id,  const std::string& name, chen::IDB::ptr conn) {
    std::string sql = "delete from assignment where subject_id = ? and name = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, subject_id);
    stmt->bindString(1, name);
    return stmt->execute();
}

int AssignmentInfoDao::QueryAll(std::vector<AssignmentInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, subject_id, name, color, content, deadline, is_deleted, create_time, update_time from assignment";
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
        AssignmentInfo::ptr v(new AssignmentInfo);
        v->m_id = rt->getInt64(0);
        v->m_subjectId = rt->getInt64(1);
        v->m_name = rt->getString(2);
        v->m_color = rt->getString(3);
        v->m_content = rt->getString(4);
        v->m_deadline = rt->getTime(5);
        v->m_isDeleted = rt->getInt32(6);
        v->m_createTime = rt->getTime(7);
        v->m_updateTime = rt->getTime(8);
        results.push_back(v);
    }
    return 0;
}

AssignmentInfo::ptr AssignmentInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, subject_id, name, color, content, deadline, is_deleted, create_time, update_time from assignment where id = ?";
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
    AssignmentInfo::ptr v(new AssignmentInfo);
    v->m_id = rt->getInt64(0);
    v->m_subjectId = rt->getInt64(1);
    v->m_name = rt->getString(2);
    v->m_color = rt->getString(3);
    v->m_content = rt->getString(4);
    v->m_deadline = rt->getTime(5);
    v->m_isDeleted = rt->getInt32(6);
    v->m_createTime = rt->getTime(7);
    v->m_updateTime = rt->getTime(8);
    return v;
}

int AssignmentInfoDao::QueryBySubjectId(std::vector<AssignmentInfo::ptr>& results,  const int64_t& subject_id, chen::IDB::ptr conn) {
    std::string sql = "select id, subject_id, name, color, content, deadline, is_deleted, create_time, update_time from assignment where subject_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, subject_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        AssignmentInfo::ptr v(new AssignmentInfo);
        v->m_id = rt->getInt64(0);
        v->m_subjectId = rt->getInt64(1);
        v->m_name = rt->getString(2);
        v->m_color = rt->getString(3);
        v->m_content = rt->getString(4);
        v->m_deadline = rt->getTime(5);
        v->m_isDeleted = rt->getInt32(6);
        v->m_createTime = rt->getTime(7);
        v->m_updateTime = rt->getTime(8);
        results.push_back(v);
    };
    return 0;
}

AssignmentInfo::ptr AssignmentInfoDao::QueryBySubjectIdName( const int64_t& subject_id,  const std::string& name, chen::IDB::ptr conn) {
    std::string sql = "select id, subject_id, name, color, content, deadline, is_deleted, create_time, update_time from assignment where subject_id = ? and name = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindInt64(1, subject_id);
    stmt->bindString(2, name);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    AssignmentInfo::ptr v(new AssignmentInfo);
    v->m_id = rt->getInt64(0);
    v->m_subjectId = rt->getInt64(1);
    v->m_name = rt->getString(2);
    v->m_color = rt->getString(3);
    v->m_content = rt->getString(4);
    v->m_deadline = rt->getTime(5);
    v->m_isDeleted = rt->getInt32(6);
    v->m_createTime = rt->getTime(7);
    v->m_updateTime = rt->getTime(8);
    return v;
}

int AssignmentInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE assignment("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "subject_id INTEGER NOT NULL DEFAULT 0,"
            "name TEXT NOT NULL DEFAULT '',"
            "color TEXT NOT NULL DEFAULT '',"
            "content TEXT NOT NULL DEFAULT '',"
            "deadline TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE INDEX assignment_subject_id ON assignment(subject_id);"
            "CREATE UNIQUE INDEX assignment_subject_id_name ON assignment(subject_id,name);"
            );
}

int AssignmentInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE assignment("
            "`id` bigint AUTO_INCREMENT COMMENT '作业id',"
            "`subject_id` bigint NOT NULL DEFAULT 0 COMMENT '科目id',"
            "`name` varchar(256) NOT NULL DEFAULT '' COMMENT '作业名称',"
            "`color` varchar(10) NOT NULL DEFAULT '' COMMENT '作业颜色',"
            "`content` text NOT NULL DEFAULT '' COMMENT '作业内容',"
            "`deadline` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '截止时间',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "KEY `assignment_subject_id` (`subject_id`),"
            "UNIQUE KEY `assignment_subject_id_name` (`subject_id`,`name`)) COMMENT='作业'");
}
} //namespace data
} //namespace blog
