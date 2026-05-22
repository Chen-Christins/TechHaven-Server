#include "task_info.h"
#include "chen/log/log.h"

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

TaskInfo::TaskInfo()
    :m_priority(1)
    ,m_status(0)
    ,m_estimatedHours()
    ,m_isDeleted(0)
    ,m_id()
    ,m_orgId()
    ,m_creatorId()
    ,m_assigneeId()
    ,m_requirementId()
    ,m_bugId()
    ,m_title()
    ,m_description()
    ,m_deadline()
    ,m_createTime(time(0))
    ,m_updateTime() {
}

std::string TaskInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["org_id"] = std::to_string(m_orgId);
    v["title"] = m_title;
    v["description"] = m_description;
    v["priority"] = m_priority;
    v["status"] = m_status;
    v["creator_id"] = std::to_string(m_creatorId);
    v["assignee_id"] = std::to_string(m_assigneeId);
    v["requirement_id"] = std::to_string(m_requirementId);
    v["bug_id"] = std::to_string(m_bugId);
    v["deadline"] = chen::Time2Str(m_deadline);
    v["estimated_hours"] = m_estimatedHours;
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void TaskInfo::setId(const int64_t& v) {
    m_id = v;
}

void TaskInfo::setOrgId(const int64_t& v) {
    m_orgId = v;
}

void TaskInfo::setTitle(const std::string& v) {
    m_title = v;
}

void TaskInfo::setDescription(const std::string& v) {
    m_description = v;
}

void TaskInfo::setPriority(const int32_t& v) {
    m_priority = v;
}

void TaskInfo::setStatus(const int32_t& v) {
    m_status = v;
}

void TaskInfo::setCreatorId(const int64_t& v) {
    m_creatorId = v;
}

void TaskInfo::setAssigneeId(const int64_t& v) {
    m_assigneeId = v;
}

void TaskInfo::setRequirementId(const int64_t& v) {
    m_requirementId = v;
}

void TaskInfo::setBugId(const int64_t& v) {
    m_bugId = v;
}

void TaskInfo::setDeadline(const int64_t& v) {
    m_deadline = v;
}

void TaskInfo::setEstimatedHours(const int32_t& v) {
    m_estimatedHours = v;
}

void TaskInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void TaskInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void TaskInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int TaskInfoDao::Update(TaskInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update task set org_id = ?, title = ?, description = ?, priority = ?, status = ?, creator_id = ?, assignee_id = ?, requirement_id = ?, bug_id = ?, deadline = ?, estimated_hours = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_orgId);
    stmt->bindString(2, info->m_title);
    stmt->bindString(3, info->m_description);
    stmt->bindInt32(4, info->m_priority);
    stmt->bindInt32(5, info->m_status);
    stmt->bindInt64(6, info->m_creatorId);
    stmt->bindInt64(7, info->m_assigneeId);
    stmt->bindInt64(8, info->m_requirementId);
    stmt->bindInt64(9, info->m_bugId);
    stmt->bindTime(10, info->m_deadline);
    stmt->bindInt32(11, info->m_estimatedHours);
    stmt->bindInt32(12, info->m_isDeleted);
    stmt->bindTime(13, info->m_createTime);
    stmt->bindTime(14, info->m_updateTime);
    stmt->bindInt64(15, info->m_id);
    return stmt->execute();
}

int TaskInfoDao::Insert(TaskInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into task (org_id, title, description, priority, status, creator_id, assignee_id, requirement_id, bug_id, deadline, estimated_hours, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_orgId);
    stmt->bindString(2, info->m_title);
    stmt->bindString(3, info->m_description);
    stmt->bindInt32(4, info->m_priority);
    stmt->bindInt32(5, info->m_status);
    stmt->bindInt64(6, info->m_creatorId);
    stmt->bindInt64(7, info->m_assigneeId);
    stmt->bindInt64(8, info->m_requirementId);
    stmt->bindInt64(9, info->m_bugId);
    stmt->bindTime(10, info->m_deadline);
    stmt->bindInt32(11, info->m_estimatedHours);
    stmt->bindInt32(12, info->m_isDeleted);
    stmt->bindTime(13, info->m_createTime);
    stmt->bindTime(14, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int TaskInfoDao::InsertOrUpdate(TaskInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into task (id, org_id, title, description, priority, status, creator_id, assignee_id, requirement_id, bug_id, deadline, estimated_hours, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_orgId);
    stmt->bindString(3, info->m_title);
    stmt->bindString(4, info->m_description);
    stmt->bindInt32(5, info->m_priority);
    stmt->bindInt32(6, info->m_status);
    stmt->bindInt64(7, info->m_creatorId);
    stmt->bindInt64(8, info->m_assigneeId);
    stmt->bindInt64(9, info->m_requirementId);
    stmt->bindInt64(10, info->m_bugId);
    stmt->bindTime(11, info->m_deadline);
    stmt->bindInt32(12, info->m_estimatedHours);
    stmt->bindInt32(13, info->m_isDeleted);
    stmt->bindTime(14, info->m_createTime);
    stmt->bindTime(15, info->m_updateTime);
    return stmt->execute();
}

int TaskInfoDao::Delete(TaskInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from task where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int TaskInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from task where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int TaskInfoDao::DeleteByOrgId( const int64_t& org_id, chen::IDB::ptr conn) {
    std::string sql = "delete from task where org_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, org_id);
    return stmt->execute();
}

int TaskInfoDao::DeleteByCreatorId( const int64_t& creator_id, chen::IDB::ptr conn) {
    std::string sql = "delete from task where creator_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, creator_id);
    return stmt->execute();
}

int TaskInfoDao::DeleteByAssigneeId( const int64_t& assignee_id, chen::IDB::ptr conn) {
    std::string sql = "delete from task where assignee_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, assignee_id);
    return stmt->execute();
}

int TaskInfoDao::QueryAll(std::vector<TaskInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, title, description, priority, status, creator_id, assignee_id, requirement_id, bug_id, deadline, estimated_hours, is_deleted, create_time, update_time from task";
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
        TaskInfo::ptr v(new TaskInfo);
        v->m_id = rt->getInt64(0);
        v->m_orgId = rt->getInt64(1);
        v->m_title = rt->getString(2);
        v->m_description = rt->getString(3);
        v->m_priority = rt->getInt32(4);
        v->m_status = rt->getInt32(5);
        v->m_creatorId = rt->getInt64(6);
        v->m_assigneeId = rt->getInt64(7);
        v->m_requirementId = rt->getInt64(8);
        v->m_bugId = rt->getInt64(9);
        v->m_deadline = rt->getTime(10);
        v->m_estimatedHours = rt->getInt32(11);
        v->m_isDeleted = rt->getInt32(12);
        v->m_createTime = rt->getTime(13);
        v->m_updateTime = rt->getTime(14);
        results.push_back(v);
    }
    return 0;
}

TaskInfo::ptr TaskInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, title, description, priority, status, creator_id, assignee_id, requirement_id, bug_id, deadline, estimated_hours, is_deleted, create_time, update_time from task where id = ?";
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
    TaskInfo::ptr v(new TaskInfo);
    v->m_id = rt->getInt64(0);
    v->m_orgId = rt->getInt64(1);
    v->m_title = rt->getString(2);
    v->m_description = rt->getString(3);
    v->m_priority = rt->getInt32(4);
    v->m_status = rt->getInt32(5);
    v->m_creatorId = rt->getInt64(6);
    v->m_assigneeId = rt->getInt64(7);
    v->m_requirementId = rt->getInt64(8);
    v->m_bugId = rt->getInt64(9);
    v->m_deadline = rt->getTime(10);
    v->m_estimatedHours = rt->getInt32(11);
    v->m_isDeleted = rt->getInt32(12);
    v->m_createTime = rt->getTime(13);
    v->m_updateTime = rt->getTime(14);
    return v;
}

int TaskInfoDao::QueryByOrgId(std::vector<TaskInfo::ptr>& results,  const int64_t& org_id, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, title, description, priority, status, creator_id, assignee_id, requirement_id, bug_id, deadline, estimated_hours, is_deleted, create_time, update_time from task where org_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, org_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        TaskInfo::ptr v(new TaskInfo);
        v->m_id = rt->getInt64(0);
        v->m_orgId = rt->getInt64(1);
        v->m_title = rt->getString(2);
        v->m_description = rt->getString(3);
        v->m_priority = rt->getInt32(4);
        v->m_status = rt->getInt32(5);
        v->m_creatorId = rt->getInt64(6);
        v->m_assigneeId = rt->getInt64(7);
        v->m_requirementId = rt->getInt64(8);
        v->m_bugId = rt->getInt64(9);
        v->m_deadline = rt->getTime(10);
        v->m_estimatedHours = rt->getInt32(11);
        v->m_isDeleted = rt->getInt32(12);
        v->m_createTime = rt->getTime(13);
        v->m_updateTime = rt->getTime(14);
        results.push_back(v);
    };
    return 0;
}

int TaskInfoDao::QueryByCreatorId(std::vector<TaskInfo::ptr>& results,  const int64_t& creator_id, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, title, description, priority, status, creator_id, assignee_id, requirement_id, bug_id, deadline, estimated_hours, is_deleted, create_time, update_time from task where creator_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, creator_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        TaskInfo::ptr v(new TaskInfo);
        v->m_id = rt->getInt64(0);
        v->m_orgId = rt->getInt64(1);
        v->m_title = rt->getString(2);
        v->m_description = rt->getString(3);
        v->m_priority = rt->getInt32(4);
        v->m_status = rt->getInt32(5);
        v->m_creatorId = rt->getInt64(6);
        v->m_assigneeId = rt->getInt64(7);
        v->m_requirementId = rt->getInt64(8);
        v->m_bugId = rt->getInt64(9);
        v->m_deadline = rt->getTime(10);
        v->m_estimatedHours = rt->getInt32(11);
        v->m_isDeleted = rt->getInt32(12);
        v->m_createTime = rt->getTime(13);
        v->m_updateTime = rt->getTime(14);
        results.push_back(v);
    };
    return 0;
}

int TaskInfoDao::QueryByAssigneeId(std::vector<TaskInfo::ptr>& results,  const int64_t& assignee_id, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, title, description, priority, status, creator_id, assignee_id, requirement_id, bug_id, deadline, estimated_hours, is_deleted, create_time, update_time from task where assignee_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, assignee_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        TaskInfo::ptr v(new TaskInfo);
        v->m_id = rt->getInt64(0);
        v->m_orgId = rt->getInt64(1);
        v->m_title = rt->getString(2);
        v->m_description = rt->getString(3);
        v->m_priority = rt->getInt32(4);
        v->m_status = rt->getInt32(5);
        v->m_creatorId = rt->getInt64(6);
        v->m_assigneeId = rt->getInt64(7);
        v->m_requirementId = rt->getInt64(8);
        v->m_bugId = rt->getInt64(9);
        v->m_deadline = rt->getTime(10);
        v->m_estimatedHours = rt->getInt32(11);
        v->m_isDeleted = rt->getInt32(12);
        v->m_createTime = rt->getTime(13);
        v->m_updateTime = rt->getTime(14);
        results.push_back(v);
    };
    return 0;
}

int TaskInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS task("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "org_id INTEGER NOT NULL DEFAULT 0,"
            "title TEXT NOT NULL DEFAULT '',"
            "description TEXT NOT NULL DEFAULT '',"
            "priority INTEGER NOT NULL DEFAULT 1,"
            "status INTEGER NOT NULL DEFAULT 0,"
            "creator_id INTEGER NOT NULL DEFAULT 0,"
            "assignee_id INTEGER NOT NULL DEFAULT 0,"
            "requirement_id INTEGER NOT NULL DEFAULT 0,"
            "bug_id INTEGER NOT NULL DEFAULT 0,"
            "deadline TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "estimated_hours INTEGER NOT NULL DEFAULT 0,"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE INDEX IF NOT EXISTS task_org_id ON task(org_id);"
            "CREATE INDEX IF NOT EXISTS task_creator_id ON task(creator_id);"
            "CREATE INDEX IF NOT EXISTS task_assignee_id ON task(assignee_id);"
            );
}

int TaskInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS task("
            "`id` bigint AUTO_INCREMENT COMMENT '任务ID',"
            "`org_id` bigint NOT NULL DEFAULT 0 COMMENT '组织ID',"
            "`title` varchar(256) NOT NULL DEFAULT '' COMMENT '任务标题',"
            "`description` varchar(2048) NOT NULL DEFAULT '' COMMENT '任务描述',"
            "`priority` int NOT NULL DEFAULT 1 COMMENT '优先级: 1低 2中 3高 4紧急',"
            "`status` int NOT NULL DEFAULT 0 COMMENT '状态: 0待办 1进行中 2已完成 3已关闭',"
            "`creator_id` bigint NOT NULL DEFAULT 0 COMMENT '创建者ID',"
            "`assignee_id` bigint NOT NULL DEFAULT 0 COMMENT '负责人ID',"
            "`requirement_id` bigint NOT NULL DEFAULT 0 COMMENT '关联需求ID',"
            "`bug_id` bigint NOT NULL DEFAULT 0 COMMENT '关联缺陷ID',"
            "`deadline` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '截止时间',"
            "`estimated_hours` int NOT NULL DEFAULT 0 COMMENT '预估工时(小时)',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "KEY `task_org_id` (`org_id`),"
            "KEY `task_creator_id` (`creator_id`),"
            "KEY `task_assignee_id` (`assignee_id`)) COMMENT='任务表'");
}
} //namespace data
} //namespace blog
