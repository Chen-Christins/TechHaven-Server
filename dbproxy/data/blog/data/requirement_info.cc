#include "requirement_info.h"
#include "chen/log/log.h"

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

RequirementInfo::RequirementInfo()
    :m_priority(1)
    ,m_status(0)
    ,m_isDeleted(0)
    ,m_id()
    ,m_orgId()
    ,m_creatorId()
    ,m_assigneeId()
    ,m_title()
    ,m_description()
    ,m_iteration()
    ,m_category()
    ,m_source()
    ,m_deadline()
    ,m_createTime(time(0))
    ,m_updateTime() {
}

std::string RequirementInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["org_id"] = std::to_string(m_orgId);
    v["title"] = m_title;
    v["description"] = m_description;
    v["priority"] = m_priority;
    v["status"] = m_status;
    v["creator_id"] = std::to_string(m_creatorId);
    v["assignee_id"] = std::to_string(m_assigneeId);
    v["iteration"] = m_iteration;
    v["category"] = m_category;
    v["source"] = m_source;
    v["deadline"] = chen::Time2Str(m_deadline);
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void RequirementInfo::setId(const int64_t& v) {
    m_id = v;
}

void RequirementInfo::setOrgId(const int64_t& v) {
    m_orgId = v;
}

void RequirementInfo::setTitle(const std::string& v) {
    m_title = v;
}

void RequirementInfo::setDescription(const std::string& v) {
    m_description = v;
}

void RequirementInfo::setPriority(const int32_t& v) {
    m_priority = v;
}

void RequirementInfo::setStatus(const int32_t& v) {
    m_status = v;
}

void RequirementInfo::setCreatorId(const int64_t& v) {
    m_creatorId = v;
}

void RequirementInfo::setAssigneeId(const int64_t& v) {
    m_assigneeId = v;
}

void RequirementInfo::setIteration(const std::string& v) {
    m_iteration = v;
}

void RequirementInfo::setCategory(const std::string& v) {
    m_category = v;
}

void RequirementInfo::setSource(const std::string& v) {
    m_source = v;
}

void RequirementInfo::setDeadline(const int64_t& v) {
    m_deadline = v;
}

void RequirementInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void RequirementInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void RequirementInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int RequirementInfoDao::Update(RequirementInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update requirement set org_id = ?, title = ?, description = ?, priority = ?, status = ?, creator_id = ?, assignee_id = ?, iteration = ?, category = ?, source = ?, deadline = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
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
    stmt->bindString(8, info->m_iteration);
    stmt->bindString(9, info->m_category);
    stmt->bindString(10, info->m_source);
    stmt->bindTime(11, info->m_deadline);
    stmt->bindInt32(12, info->m_isDeleted);
    stmt->bindTime(13, info->m_createTime);
    stmt->bindTime(14, info->m_updateTime);
    stmt->bindInt64(15, info->m_id);
    return stmt->execute();
}

int RequirementInfoDao::Insert(RequirementInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into requirement (org_id, title, description, priority, status, creator_id, assignee_id, iteration, category, source, deadline, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
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
    stmt->bindString(8, info->m_iteration);
    stmt->bindString(9, info->m_category);
    stmt->bindString(10, info->m_source);
    stmt->bindTime(11, info->m_deadline);
    stmt->bindInt32(12, info->m_isDeleted);
    stmt->bindTime(13, info->m_createTime);
    stmt->bindTime(14, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int RequirementInfoDao::InsertOrUpdate(RequirementInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into requirement (id, org_id, title, description, priority, status, creator_id, assignee_id, iteration, category, source, deadline, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
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
    stmt->bindString(9, info->m_iteration);
    stmt->bindString(10, info->m_category);
    stmt->bindString(11, info->m_source);
    stmt->bindTime(12, info->m_deadline);
    stmt->bindInt32(13, info->m_isDeleted);
    stmt->bindTime(14, info->m_createTime);
    stmt->bindTime(15, info->m_updateTime);
    return stmt->execute();
}

int RequirementInfoDao::Delete(RequirementInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from requirement where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int RequirementInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from requirement where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int RequirementInfoDao::DeleteByOrgId( const int64_t& org_id, chen::IDB::ptr conn) {
    std::string sql = "delete from requirement where org_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, org_id);
    return stmt->execute();
}

int RequirementInfoDao::DeleteByCreatorId( const int64_t& creator_id, chen::IDB::ptr conn) {
    std::string sql = "delete from requirement where creator_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, creator_id);
    return stmt->execute();
}

int RequirementInfoDao::DeleteByAssigneeId( const int64_t& assignee_id, chen::IDB::ptr conn) {
    std::string sql = "delete from requirement where assignee_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, assignee_id);
    return stmt->execute();
}

int RequirementInfoDao::QueryAll(std::vector<RequirementInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, title, description, priority, status, creator_id, assignee_id, iteration, category, source, deadline, is_deleted, create_time, update_time from requirement";
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
        RequirementInfo::ptr v(new RequirementInfo);
        v->m_id = rt->getInt64(0);
        v->m_orgId = rt->getInt64(1);
        v->m_title = rt->getString(2);
        v->m_description = rt->getString(3);
        v->m_priority = rt->getInt32(4);
        v->m_status = rt->getInt32(5);
        v->m_creatorId = rt->getInt64(6);
        v->m_assigneeId = rt->getInt64(7);
        v->m_iteration = rt->getString(8);
        v->m_category = rt->getString(9);
        v->m_source = rt->getString(10);
        v->m_deadline = rt->getTime(11);
        v->m_isDeleted = rt->getInt32(12);
        v->m_createTime = rt->getTime(13);
        v->m_updateTime = rt->getTime(14);
        results.push_back(v);
    }
    return 0;
}

RequirementInfo::ptr RequirementInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, title, description, priority, status, creator_id, assignee_id, iteration, category, source, deadline, is_deleted, create_time, update_time from requirement where id = ?";
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
    RequirementInfo::ptr v(new RequirementInfo);
    v->m_id = rt->getInt64(0);
    v->m_orgId = rt->getInt64(1);
    v->m_title = rt->getString(2);
    v->m_description = rt->getString(3);
    v->m_priority = rt->getInt32(4);
    v->m_status = rt->getInt32(5);
    v->m_creatorId = rt->getInt64(6);
    v->m_assigneeId = rt->getInt64(7);
    v->m_iteration = rt->getString(8);
    v->m_category = rt->getString(9);
    v->m_source = rt->getString(10);
    v->m_deadline = rt->getTime(11);
    v->m_isDeleted = rt->getInt32(12);
    v->m_createTime = rt->getTime(13);
    v->m_updateTime = rt->getTime(14);
    return v;
}

int RequirementInfoDao::QueryByOrgId(std::vector<RequirementInfo::ptr>& results,  const int64_t& org_id, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, title, description, priority, status, creator_id, assignee_id, iteration, category, source, deadline, is_deleted, create_time, update_time from requirement where org_id = ?";
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
        RequirementInfo::ptr v(new RequirementInfo);
        v->m_id = rt->getInt64(0);
        v->m_orgId = rt->getInt64(1);
        v->m_title = rt->getString(2);
        v->m_description = rt->getString(3);
        v->m_priority = rt->getInt32(4);
        v->m_status = rt->getInt32(5);
        v->m_creatorId = rt->getInt64(6);
        v->m_assigneeId = rt->getInt64(7);
        v->m_iteration = rt->getString(8);
        v->m_category = rt->getString(9);
        v->m_source = rt->getString(10);
        v->m_deadline = rt->getTime(11);
        v->m_isDeleted = rt->getInt32(12);
        v->m_createTime = rt->getTime(13);
        v->m_updateTime = rt->getTime(14);
        results.push_back(v);
    };
    return 0;
}

int RequirementInfoDao::QueryByCreatorId(std::vector<RequirementInfo::ptr>& results,  const int64_t& creator_id, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, title, description, priority, status, creator_id, assignee_id, iteration, category, source, deadline, is_deleted, create_time, update_time from requirement where creator_id = ?";
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
        RequirementInfo::ptr v(new RequirementInfo);
        v->m_id = rt->getInt64(0);
        v->m_orgId = rt->getInt64(1);
        v->m_title = rt->getString(2);
        v->m_description = rt->getString(3);
        v->m_priority = rt->getInt32(4);
        v->m_status = rt->getInt32(5);
        v->m_creatorId = rt->getInt64(6);
        v->m_assigneeId = rt->getInt64(7);
        v->m_iteration = rt->getString(8);
        v->m_category = rt->getString(9);
        v->m_source = rt->getString(10);
        v->m_deadline = rt->getTime(11);
        v->m_isDeleted = rt->getInt32(12);
        v->m_createTime = rt->getTime(13);
        v->m_updateTime = rt->getTime(14);
        results.push_back(v);
    };
    return 0;
}

int RequirementInfoDao::QueryByAssigneeId(std::vector<RequirementInfo::ptr>& results,  const int64_t& assignee_id, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, title, description, priority, status, creator_id, assignee_id, iteration, category, source, deadline, is_deleted, create_time, update_time from requirement where assignee_id = ?";
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
        RequirementInfo::ptr v(new RequirementInfo);
        v->m_id = rt->getInt64(0);
        v->m_orgId = rt->getInt64(1);
        v->m_title = rt->getString(2);
        v->m_description = rt->getString(3);
        v->m_priority = rt->getInt32(4);
        v->m_status = rt->getInt32(5);
        v->m_creatorId = rt->getInt64(6);
        v->m_assigneeId = rt->getInt64(7);
        v->m_iteration = rt->getString(8);
        v->m_category = rt->getString(9);
        v->m_source = rt->getString(10);
        v->m_deadline = rt->getTime(11);
        v->m_isDeleted = rt->getInt32(12);
        v->m_createTime = rt->getTime(13);
        v->m_updateTime = rt->getTime(14);
        results.push_back(v);
    };
    return 0;
}

int RequirementInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS requirement("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "org_id INTEGER NOT NULL DEFAULT 0,"
            "title TEXT NOT NULL DEFAULT '',"
            "description TEXT NOT NULL DEFAULT '',"
            "priority INTEGER NOT NULL DEFAULT 1,"
            "status INTEGER NOT NULL DEFAULT 0,"
            "creator_id INTEGER NOT NULL DEFAULT 0,"
            "assignee_id INTEGER NOT NULL DEFAULT 0,"
            "iteration TEXT NOT NULL DEFAULT '',"
            "category TEXT NOT NULL DEFAULT '',"
            "source TEXT NOT NULL DEFAULT '',"
            "deadline TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE INDEX IF NOT EXISTS requirement_org_id ON requirement(org_id);"
            "CREATE INDEX IF NOT EXISTS requirement_creator_id ON requirement(creator_id);"
            "CREATE INDEX IF NOT EXISTS requirement_assignee_id ON requirement(assignee_id);"
            );
}

int RequirementInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS requirement("
            "`id` bigint AUTO_INCREMENT COMMENT '需求ID',"
            "`org_id` bigint NOT NULL DEFAULT 0 COMMENT '组织ID',"
            "`title` varchar(256) NOT NULL DEFAULT '' COMMENT '需求标题',"
            "`description` varchar(2048) NOT NULL DEFAULT '' COMMENT '需求描述',"
            "`priority` int NOT NULL DEFAULT 1 COMMENT '优先级: 1低 2中 3高 4紧急',"
            "`status` int NOT NULL DEFAULT 0 COMMENT '状态: 0草稿 1待处理 2进行中 3已完成 4已关闭',"
            "`creator_id` bigint NOT NULL DEFAULT 0 COMMENT '创建者ID',"
            "`assignee_id` bigint NOT NULL DEFAULT 0 COMMENT '负责人ID',"
            "`iteration` varchar(128) NOT NULL DEFAULT '' COMMENT '迭代',"
            "`category` varchar(128) NOT NULL DEFAULT '' COMMENT '分类',"
            "`source` varchar(128) NOT NULL DEFAULT '' COMMENT '需求来源',"
            "`deadline` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '截止时间',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "KEY `requirement_org_id` (`org_id`),"
            "KEY `requirement_creator_id` (`creator_id`),"
            "KEY `requirement_assignee_id` (`assignee_id`)) COMMENT='需求表'");
}
} //namespace data
} //namespace blog
