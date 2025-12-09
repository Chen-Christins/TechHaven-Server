#include "assignment_info.h"
#include "chen/log/log.h"

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

AssignmentInfo::AssignmentInfo()
    :m_status()
    ,m_maxSize()
    ,m_isDeleted()
    ,m_id()
    ,m_name()
    ,m_subjectName()
    ,m_description()
    ,m_fileType()
    ,m_deadline()
    ,m_createTime()
    ,m_updateTime() {
}

std::string AssignmentInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["name"] = m_name;
    v["subject_name"] = m_subjectName;
    v["status"] = m_status;
    v["description"] = m_description;
    v["max_size"] = m_maxSize;
    v["file_type"] = m_fileType;
    v["deadline"] = chen::Time2Str(m_deadline);
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void AssignmentInfo::setId(const int64_t& v) {
    m_id = v;
}

void AssignmentInfo::setName(const std::string& v) {
    m_name = v;
}

void AssignmentInfo::setSubjectName(const std::string& v) {
    m_subjectName = v;
}

void AssignmentInfo::setStatus(const int32_t& v) {
    m_status = v;
}

void AssignmentInfo::setDescription(const std::string& v) {
    m_description = v;
}

void AssignmentInfo::setMaxSize(const int32_t& v) {
    m_maxSize = v;
}

void AssignmentInfo::setFileType(const std::string& v) {
    m_fileType = v;
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
    std::string sql = "update assignment set name = ?, subject_name = ?, status = ?, description = ?, max_size = ?, file_type = ?, deadline = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_name);
    stmt->bindString(2, info->m_subjectName);
    stmt->bindInt32(3, info->m_status);
    stmt->bindString(4, info->m_description);
    stmt->bindInt32(5, info->m_maxSize);
    stmt->bindString(6, info->m_fileType);
    stmt->bindTime(7, info->m_deadline);
    stmt->bindInt32(8, info->m_isDeleted);
    stmt->bindTime(9, info->m_createTime);
    stmt->bindTime(10, info->m_updateTime);
    stmt->bindInt64(11, info->m_id);
    return stmt->execute();
}

int AssignmentInfoDao::Insert(AssignmentInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into assignment (name, subject_name, status, description, max_size, file_type, deadline, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_name);
    stmt->bindString(2, info->m_subjectName);
    stmt->bindInt32(3, info->m_status);
    stmt->bindString(4, info->m_description);
    stmt->bindInt32(5, info->m_maxSize);
    stmt->bindString(6, info->m_fileType);
    stmt->bindTime(7, info->m_deadline);
    stmt->bindInt32(8, info->m_isDeleted);
    stmt->bindTime(9, info->m_createTime);
    stmt->bindTime(10, info->m_updateTime);
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
    std::string sql = "replace into assignment (id, name, subject_name, status, description, max_size, file_type, deadline, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindString(2, info->m_name);
    stmt->bindString(3, info->m_subjectName);
    stmt->bindInt32(4, info->m_status);
    stmt->bindString(5, info->m_description);
    stmt->bindInt32(6, info->m_maxSize);
    stmt->bindString(7, info->m_fileType);
    stmt->bindTime(8, info->m_deadline);
    stmt->bindInt32(9, info->m_isDeleted);
    stmt->bindTime(10, info->m_createTime);
    stmt->bindTime(11, info->m_updateTime);
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

int AssignmentInfoDao::DeleteBySubjectName( const std::string& subject_name, chen::IDB::ptr conn) {
    std::string sql = "delete from assignment where subject_name = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, subject_name);
    return stmt->execute();
}

int AssignmentInfoDao::DeleteBySubjectNameName( const std::string& subject_name,  const std::string& name, chen::IDB::ptr conn) {
    std::string sql = "delete from assignment where subject_name = ? and name = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, subject_name);
    stmt->bindString(1, name);
    return stmt->execute();
}

int AssignmentInfoDao::QueryAll(std::vector<AssignmentInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, name, subject_name, status, description, max_size, file_type, deadline, is_deleted, create_time, update_time from assignment";
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
        v->m_name = rt->getString(1);
        v->m_subjectName = rt->getString(2);
        v->m_status = rt->getInt32(3);
        v->m_description = rt->getString(4);
        v->m_maxSize = rt->getInt32(5);
        v->m_fileType = rt->getString(6);
        v->m_deadline = rt->getTime(7);
        v->m_isDeleted = rt->getInt32(8);
        v->m_createTime = rt->getTime(9);
        v->m_updateTime = rt->getTime(10);
        results.push_back(v);
    }
    return 0;
}

AssignmentInfo::ptr AssignmentInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, name, subject_name, status, description, max_size, file_type, deadline, is_deleted, create_time, update_time from assignment where id = ?";
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
    v->m_name = rt->getString(1);
    v->m_subjectName = rt->getString(2);
    v->m_status = rt->getInt32(3);
    v->m_description = rt->getString(4);
    v->m_maxSize = rt->getInt32(5);
    v->m_fileType = rt->getString(6);
    v->m_deadline = rt->getTime(7);
    v->m_isDeleted = rt->getInt32(8);
    v->m_createTime = rt->getTime(9);
    v->m_updateTime = rt->getTime(10);
    return v;
}

int AssignmentInfoDao::QueryBySubjectName(std::vector<AssignmentInfo::ptr>& results,  const std::string& subject_name, chen::IDB::ptr conn) {
    std::string sql = "select id, name, subject_name, status, description, max_size, file_type, deadline, is_deleted, create_time, update_time from assignment where subject_name = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, subject_name);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        AssignmentInfo::ptr v(new AssignmentInfo);
        v->m_id = rt->getInt64(0);
        v->m_name = rt->getString(1);
        v->m_subjectName = rt->getString(2);
        v->m_status = rt->getInt32(3);
        v->m_description = rt->getString(4);
        v->m_maxSize = rt->getInt32(5);
        v->m_fileType = rt->getString(6);
        v->m_deadline = rt->getTime(7);
        v->m_isDeleted = rt->getInt32(8);
        v->m_createTime = rt->getTime(9);
        v->m_updateTime = rt->getTime(10);
        results.push_back(v);
    };
    return 0;
}

AssignmentInfo::ptr AssignmentInfoDao::QueryBySubjectNameName( const std::string& subject_name,  const std::string& name, chen::IDB::ptr conn) {
    std::string sql = "select id, name, subject_name, status, description, max_size, file_type, deadline, is_deleted, create_time, update_time from assignment where subject_name = ? and name = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindString(1, subject_name);
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
    v->m_name = rt->getString(1);
    v->m_subjectName = rt->getString(2);
    v->m_status = rt->getInt32(3);
    v->m_description = rt->getString(4);
    v->m_maxSize = rt->getInt32(5);
    v->m_fileType = rt->getString(6);
    v->m_deadline = rt->getTime(7);
    v->m_isDeleted = rt->getInt32(8);
    v->m_createTime = rt->getTime(9);
    v->m_updateTime = rt->getTime(10);
    return v;
}

int AssignmentInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE assignment("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL DEFAULT '',"
            "subject_name TEXT NOT NULL DEFAULT '',"
            "status INTEGER NOT NULL DEFAULT 0,"
            "description TEXT NOT NULL DEFAULT '',"
            "max_size INTEGER NOT NULL DEFAULT 0,"
            "file_type TEXT NOT NULL DEFAULT '',"
            "deadline TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE INDEX assignment_subject_name ON assignment(subject_name);"
            "CREATE UNIQUE INDEX assignment_subject_name_name ON assignment(subject_name,name);"
            );
}

int AssignmentInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE assignment("
            "`id` bigint AUTO_INCREMENT COMMENT '作业id',"
            "`name` varchar(256) NOT NULL DEFAULT '' COMMENT '作业名称',"
            "`subject_name` varchar(256) NOT NULL DEFAULT '' COMMENT '科目名称',"
            "`status` int NOT NULL DEFAULT 0 COMMENT '作业状态',"
            "`description` varchar(512) NOT NULL DEFAULT '' COMMENT '作业描述',"
            "`max_size` int NOT NULL DEFAULT 0 COMMENT '最大提交大小',"
            "`file_type` varchar(128) NOT NULL DEFAULT '' COMMENT '允许提交的文件类型',"
            "`deadline` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '截止时间',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "KEY `assignment_subject_name` (`subject_name`),"
            "UNIQUE KEY `assignment_subject_name_name` (`subject_name`,`name`)) COMMENT='作业'");
}
} //namespace data
} //namespace blog
