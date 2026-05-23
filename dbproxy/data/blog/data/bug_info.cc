#include "bug_info.h"
#include "chen/log/log.h"
#include <set>

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

BugInfo::BugInfo()
    :m_severity(1)
    ,m_priority(1)
    ,m_status(0)
    ,m_isDeleted(0)
    ,m_id()
    ,m_orgId()
    ,m_creatorId()
    ,m_assigneeId()
    ,m_requirementId()
    ,m_title()
    ,m_description()
    ,m_module()
    ,m_stepsToReproduce()
    ,m_environment()
    ,m_createTime(time(0))
    ,m_updateTime() {
}

std::string BugInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["org_id"] = std::to_string(m_orgId);
    v["title"] = m_title;
    v["description"] = m_description;
    v["severity"] = m_severity;
    v["priority"] = m_priority;
    v["status"] = m_status;
    v["creator_id"] = std::to_string(m_creatorId);
    v["assignee_id"] = std::to_string(m_assigneeId);
    v["requirement_id"] = std::to_string(m_requirementId);
    v["module"] = m_module;
    v["steps_to_reproduce"] = m_stepsToReproduce;
    v["environment"] = m_environment;
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void BugInfo::setId(const int64_t& v) {
    m_id = v;
}

void BugInfo::setOrgId(const int64_t& v) {
    m_orgId = v;
}

void BugInfo::setTitle(const std::string& v) {
    m_title = v;
}

void BugInfo::setDescription(const std::string& v) {
    m_description = v;
}

void BugInfo::setSeverity(const int32_t& v) {
    m_severity = v;
}

void BugInfo::setPriority(const int32_t& v) {
    m_priority = v;
}

void BugInfo::setStatus(const int32_t& v) {
    m_status = v;
}

void BugInfo::setCreatorId(const int64_t& v) {
    m_creatorId = v;
}

void BugInfo::setAssigneeId(const int64_t& v) {
    m_assigneeId = v;
}

void BugInfo::setRequirementId(const int64_t& v) {
    m_requirementId = v;
}

void BugInfo::setModule(const std::string& v) {
    m_module = v;
}

void BugInfo::setStepsToReproduce(const std::string& v) {
    m_stepsToReproduce = v;
}

void BugInfo::setEnvironment(const std::string& v) {
    m_environment = v;
}

void BugInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void BugInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void BugInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int BugInfoDao::Update(BugInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update bug set org_id = ?, title = ?, description = ?, severity = ?, priority = ?, status = ?, creator_id = ?, assignee_id = ?, requirement_id = ?, module = ?, steps_to_reproduce = ?, environment = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_orgId);
    stmt->bindString(2, info->m_title);
    stmt->bindString(3, info->m_description);
    stmt->bindInt32(4, info->m_severity);
    stmt->bindInt32(5, info->m_priority);
    stmt->bindInt32(6, info->m_status);
    stmt->bindInt64(7, info->m_creatorId);
    stmt->bindInt64(8, info->m_assigneeId);
    stmt->bindInt64(9, info->m_requirementId);
    stmt->bindString(10, info->m_module);
    stmt->bindString(11, info->m_stepsToReproduce);
    stmt->bindString(12, info->m_environment);
    stmt->bindInt32(13, info->m_isDeleted);
    stmt->bindTime(14, info->m_createTime);
    stmt->bindTime(15, info->m_updateTime);
    stmt->bindInt64(16, info->m_id);
    return stmt->execute();
}

int BugInfoDao::Insert(BugInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into bug (org_id, title, description, severity, priority, status, creator_id, assignee_id, requirement_id, module, steps_to_reproduce, environment, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_orgId);
    stmt->bindString(2, info->m_title);
    stmt->bindString(3, info->m_description);
    stmt->bindInt32(4, info->m_severity);
    stmt->bindInt32(5, info->m_priority);
    stmt->bindInt32(6, info->m_status);
    stmt->bindInt64(7, info->m_creatorId);
    stmt->bindInt64(8, info->m_assigneeId);
    stmt->bindInt64(9, info->m_requirementId);
    stmt->bindString(10, info->m_module);
    stmt->bindString(11, info->m_stepsToReproduce);
    stmt->bindString(12, info->m_environment);
    stmt->bindInt32(13, info->m_isDeleted);
    stmt->bindTime(14, info->m_createTime);
    stmt->bindTime(15, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int BugInfoDao::InsertOrUpdate(BugInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into bug (id, org_id, title, description, severity, priority, status, creator_id, assignee_id, requirement_id, module, steps_to_reproduce, environment, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
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
    stmt->bindInt32(5, info->m_severity);
    stmt->bindInt32(6, info->m_priority);
    stmt->bindInt32(7, info->m_status);
    stmt->bindInt64(8, info->m_creatorId);
    stmt->bindInt64(9, info->m_assigneeId);
    stmt->bindInt64(10, info->m_requirementId);
    stmt->bindString(11, info->m_module);
    stmt->bindString(12, info->m_stepsToReproduce);
    stmt->bindString(13, info->m_environment);
    stmt->bindInt32(14, info->m_isDeleted);
    stmt->bindTime(15, info->m_createTime);
    stmt->bindTime(16, info->m_updateTime);
    return stmt->execute();
}

int BugInfoDao::Delete(BugInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from bug where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int BugInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from bug where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int BugInfoDao::DeleteByOrgId( const int64_t& org_id, chen::IDB::ptr conn) {
    std::string sql = "delete from bug where org_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, org_id);
    return stmt->execute();
}

int BugInfoDao::DeleteByCreatorId( const int64_t& creator_id, chen::IDB::ptr conn) {
    std::string sql = "delete from bug where creator_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, creator_id);
    return stmt->execute();
}

int BugInfoDao::DeleteByAssigneeId( const int64_t& assignee_id, chen::IDB::ptr conn) {
    std::string sql = "delete from bug where assignee_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, assignee_id);
    return stmt->execute();
}

int BugInfoDao::QueryAll(std::vector<BugInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, title, description, severity, priority, status, creator_id, assignee_id, requirement_id, module, steps_to_reproduce, environment, is_deleted, create_time, update_time from bug";
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
        BugInfo::ptr v(new BugInfo);
        v->m_id = rt->getInt64(0);
        v->m_orgId = rt->getInt64(1);
        v->m_title = rt->getString(2);
        v->m_description = rt->getString(3);
        v->m_severity = rt->getInt32(4);
        v->m_priority = rt->getInt32(5);
        v->m_status = rt->getInt32(6);
        v->m_creatorId = rt->getInt64(7);
        v->m_assigneeId = rt->getInt64(8);
        v->m_requirementId = rt->getInt64(9);
        v->m_module = rt->getString(10);
        v->m_stepsToReproduce = rt->getString(11);
        v->m_environment = rt->getString(12);
        v->m_isDeleted = rt->getInt32(13);
        v->m_createTime = rt->getTime(14);
        v->m_updateTime = rt->getTime(15);
        results.push_back(v);
    }
    return 0;
}

BugInfo::ptr BugInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, title, description, severity, priority, status, creator_id, assignee_id, requirement_id, module, steps_to_reproduce, environment, is_deleted, create_time, update_time from bug where id = ?";
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
    BugInfo::ptr v(new BugInfo);
    v->m_id = rt->getInt64(0);
    v->m_orgId = rt->getInt64(1);
    v->m_title = rt->getString(2);
    v->m_description = rt->getString(3);
    v->m_severity = rt->getInt32(4);
    v->m_priority = rt->getInt32(5);
    v->m_status = rt->getInt32(6);
    v->m_creatorId = rt->getInt64(7);
    v->m_assigneeId = rt->getInt64(8);
    v->m_requirementId = rt->getInt64(9);
    v->m_module = rt->getString(10);
    v->m_stepsToReproduce = rt->getString(11);
    v->m_environment = rt->getString(12);
    v->m_isDeleted = rt->getInt32(13);
    v->m_createTime = rt->getTime(14);
    v->m_updateTime = rt->getTime(15);
    return v;
}

int BugInfoDao::QueryByOrgId(std::vector<BugInfo::ptr>& results,  const int64_t& org_id, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, title, description, severity, priority, status, creator_id, assignee_id, requirement_id, module, steps_to_reproduce, environment, is_deleted, create_time, update_time from bug where org_id = ?";
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
        BugInfo::ptr v(new BugInfo);
        v->m_id = rt->getInt64(0);
        v->m_orgId = rt->getInt64(1);
        v->m_title = rt->getString(2);
        v->m_description = rt->getString(3);
        v->m_severity = rt->getInt32(4);
        v->m_priority = rt->getInt32(5);
        v->m_status = rt->getInt32(6);
        v->m_creatorId = rt->getInt64(7);
        v->m_assigneeId = rt->getInt64(8);
        v->m_requirementId = rt->getInt64(9);
        v->m_module = rt->getString(10);
        v->m_stepsToReproduce = rt->getString(11);
        v->m_environment = rt->getString(12);
        v->m_isDeleted = rt->getInt32(13);
        v->m_createTime = rt->getTime(14);
        v->m_updateTime = rt->getTime(15);
        results.push_back(v);
    };
    return 0;
}

int BugInfoDao::QueryByCreatorId(std::vector<BugInfo::ptr>& results,  const int64_t& creator_id, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, title, description, severity, priority, status, creator_id, assignee_id, requirement_id, module, steps_to_reproduce, environment, is_deleted, create_time, update_time from bug where creator_id = ?";
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
        BugInfo::ptr v(new BugInfo);
        v->m_id = rt->getInt64(0);
        v->m_orgId = rt->getInt64(1);
        v->m_title = rt->getString(2);
        v->m_description = rt->getString(3);
        v->m_severity = rt->getInt32(4);
        v->m_priority = rt->getInt32(5);
        v->m_status = rt->getInt32(6);
        v->m_creatorId = rt->getInt64(7);
        v->m_assigneeId = rt->getInt64(8);
        v->m_requirementId = rt->getInt64(9);
        v->m_module = rt->getString(10);
        v->m_stepsToReproduce = rt->getString(11);
        v->m_environment = rt->getString(12);
        v->m_isDeleted = rt->getInt32(13);
        v->m_createTime = rt->getTime(14);
        v->m_updateTime = rt->getTime(15);
        results.push_back(v);
    };
    return 0;
}

int BugInfoDao::QueryByAssigneeId(std::vector<BugInfo::ptr>& results,  const int64_t& assignee_id, chen::IDB::ptr conn) {
    std::string sql = "select id, org_id, title, description, severity, priority, status, creator_id, assignee_id, requirement_id, module, steps_to_reproduce, environment, is_deleted, create_time, update_time from bug where assignee_id = ?";
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
        BugInfo::ptr v(new BugInfo);
        v->m_id = rt->getInt64(0);
        v->m_orgId = rt->getInt64(1);
        v->m_title = rt->getString(2);
        v->m_description = rt->getString(3);
        v->m_severity = rt->getInt32(4);
        v->m_priority = rt->getInt32(5);
        v->m_status = rt->getInt32(6);
        v->m_creatorId = rt->getInt64(7);
        v->m_assigneeId = rt->getInt64(8);
        v->m_requirementId = rt->getInt64(9);
        v->m_module = rt->getString(10);
        v->m_stepsToReproduce = rt->getString(11);
        v->m_environment = rt->getString(12);
        v->m_isDeleted = rt->getInt32(13);
        v->m_createTime = rt->getTime(14);
        v->m_updateTime = rt->getTime(15);
        results.push_back(v);
    };
    return 0;
}

int BugInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS bug("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "org_id INTEGER NOT NULL DEFAULT 0,"
            "title TEXT NOT NULL DEFAULT '',"
            "description TEXT NOT NULL DEFAULT '',"
            "severity INTEGER NOT NULL DEFAULT 1,"
            "priority INTEGER NOT NULL DEFAULT 1,"
            "status INTEGER NOT NULL DEFAULT 0,"
            "creator_id INTEGER NOT NULL DEFAULT 0,"
            "assignee_id INTEGER NOT NULL DEFAULT 0,"
            "requirement_id INTEGER NOT NULL DEFAULT 0,"
            "module TEXT NOT NULL DEFAULT '',"
            "steps_to_reproduce TEXT NOT NULL DEFAULT '',"
            "environment TEXT NOT NULL DEFAULT '',"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE INDEX IF NOT EXISTS bug_org_id ON bug(org_id);"
            "CREATE INDEX IF NOT EXISTS bug_creator_id ON bug(creator_id);"
            "CREATE INDEX IF NOT EXISTS bug_assignee_id ON bug(assignee_id);"
            );
}

int BugInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS bug("
            "`id` bigint AUTO_INCREMENT COMMENT '缺陷ID',"
            "`org_id` bigint NOT NULL DEFAULT 0 COMMENT '组织ID',"
            "`title` varchar(256) NOT NULL DEFAULT '' COMMENT '缺陷标题',"
            "`description` varchar(2048) NOT NULL DEFAULT '' COMMENT '缺陷描述',"
            "`severity` int NOT NULL DEFAULT 1 COMMENT '严重程度: 1轻微 2一般 3严重 4致命',"
            "`priority` int NOT NULL DEFAULT 1 COMMENT '优先级: 1低 2中 3高 4紧急',"
            "`status` int NOT NULL DEFAULT 0 COMMENT '状态: 0待处理 1进行中 2已修复 3已关闭 4重新打开',"
            "`creator_id` bigint NOT NULL DEFAULT 0 COMMENT '创建者ID',"
            "`assignee_id` bigint NOT NULL DEFAULT 0 COMMENT '负责人ID',"
            "`requirement_id` bigint NOT NULL DEFAULT 0 COMMENT '关联需求ID',"
            "`module` varchar(128) NOT NULL DEFAULT '' COMMENT '所属模块',"
            "`steps_to_reproduce` varchar(2048) NOT NULL DEFAULT '' COMMENT '复现步骤',"
            "`environment` varchar(256) NOT NULL DEFAULT '' COMMENT '环境信息',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "KEY `bug_org_id` (`org_id`),"
            "KEY `bug_creator_id` (`creator_id`),"
            "KEY `bug_assignee_id` (`assignee_id`)) COMMENT='缺陷表'");
}

int BugInfoDao::MigrateTableSQLite3(chen::IDB::ptr conn) {
    auto data = conn->query("PRAGMA table_info(bug)");
    if (!data) {
        ERROR(logger) << "PRAGMA table_info(bug) errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(1));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("org_id");
    expected_cols.insert("title");
    expected_cols.insert("description");
    expected_cols.insert("severity");
    expected_cols.insert("priority");
    expected_cols.insert("status");
    expected_cols.insert("creator_id");
    expected_cols.insert("assignee_id");
    expected_cols.insert("requirement_id");
    expected_cols.insert("module");
    expected_cols.insert("steps_to_reproduce");
    expected_cols.insert("environment");
    expected_cols.insert("is_deleted");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("org_id") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.org_id";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN org_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN org_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("title") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.title";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN title TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN title failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("description") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.description";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN description TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN description failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("severity") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.severity";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN severity INTEGER NOT NULL DEFAULT 1");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN severity failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("priority") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.priority";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN priority INTEGER NOT NULL DEFAULT 1");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN priority failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("status") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.status";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN status INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN status failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("creator_id") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.creator_id";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN creator_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN creator_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("assignee_id") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.assignee_id";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN assignee_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN assignee_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("requirement_id") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.requirement_id";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN requirement_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN requirement_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("module") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.module";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN module TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN module failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("steps_to_reproduce") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.steps_to_reproduce";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN steps_to_reproduce TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN steps_to_reproduce failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("environment") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.environment";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN environment TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN environment failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.is_deleted";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.create_time";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN create_time TIMESTAMP NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.update_time";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column bug." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE bug DROP COLUMN " + col);
            if (rt) {
                ERROR(logger) << "ALTER TABLE bug DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}

int BugInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM bug");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM bug errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(0));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("org_id");
    expected_cols.insert("title");
    expected_cols.insert("description");
    expected_cols.insert("severity");
    expected_cols.insert("priority");
    expected_cols.insert("status");
    expected_cols.insert("creator_id");
    expected_cols.insert("assignee_id");
    expected_cols.insert("requirement_id");
    expected_cols.insert("module");
    expected_cols.insert("steps_to_reproduce");
    expected_cols.insert("environment");
    expected_cols.insert("is_deleted");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("org_id") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.org_id";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN `org_id` bigint NOT NULL DEFAULT 0 COMMENT '组织ID'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN org_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("title") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.title";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN `title` varchar(256) NOT NULL DEFAULT '' COMMENT '缺陷标题'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN title failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("description") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.description";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN `description` varchar(2048) NOT NULL DEFAULT '' COMMENT '缺陷描述'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN description failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("severity") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.severity";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN `severity` int NOT NULL DEFAULT 1 COMMENT '严重程度: 1轻微 2一般 3严重 4致命'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN severity failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("priority") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.priority";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN `priority` int NOT NULL DEFAULT 1 COMMENT '优先级: 1低 2中 3高 4紧急'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN priority failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("status") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.status";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN `status` int NOT NULL DEFAULT 0 COMMENT '状态: 0待处理 1进行中 2已修复 3已关闭 4重新打开'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN status failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("creator_id") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.creator_id";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN `creator_id` bigint NOT NULL DEFAULT 0 COMMENT '创建者ID'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN creator_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("assignee_id") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.assignee_id";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN `assignee_id` bigint NOT NULL DEFAULT 0 COMMENT '负责人ID'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN assignee_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("requirement_id") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.requirement_id";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN `requirement_id` bigint NOT NULL DEFAULT 0 COMMENT '关联需求ID'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN requirement_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("module") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.module";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN `module` varchar(128) NOT NULL DEFAULT '' COMMENT '所属模块'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN module failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("steps_to_reproduce") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.steps_to_reproduce";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN `steps_to_reproduce` varchar(2048) NOT NULL DEFAULT '' COMMENT '复现步骤'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN steps_to_reproduce failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("environment") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.environment";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN `environment` varchar(256) NOT NULL DEFAULT '' COMMENT '环境信息'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN environment failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.is_deleted";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN `is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.create_time";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column bug.update_time";
        int rt = conn->execute("ALTER TABLE bug ADD COLUMN `update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '更新时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE bug ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column bug." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE bug DROP COLUMN `" + col + "`");
            if (rt) {
                ERROR(logger) << "ALTER TABLE bug DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}


} //namespace data
} //namespace blog
