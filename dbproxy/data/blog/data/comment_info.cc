#include "comment_info.h"
#include "chen/log/log.h"

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

CommentInfo::CommentInfo()
    :m_status(1)
    ,m_isReported(0)
    ,m_reportCount(0)
    ,m_isDeleted(0)
    ,m_id()
    ,m_articleId()
    ,m_userId()
    ,m_parentId()
    ,m_ip()
    ,m_userAgent()
    ,m_content()
    ,m_createTime()
    ,m_updateTime() {
}

std::string CommentInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["article_id"] = std::to_string(m_articleId);
    v["user_id"] = std::to_string(m_userId);
    v["parent_id"] = std::to_string(m_parentId);
    v["content"] = m_content;
    v["ip"] = m_ip;
    v["user_agent"] = m_userAgent;
    v["status"] = m_status;
    v["is_reported"] = m_isReported;
    v["report_count"] = m_reportCount;
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void CommentInfo::setId(const int64_t& v) {
    m_id = v;
}

void CommentInfo::setArticleId(const int64_t& v) {
    m_articleId = v;
}

void CommentInfo::setUserId(const int64_t& v) {
    m_userId = v;
}

void CommentInfo::setParentId(const int64_t& v) {
    m_parentId = v;
}

void CommentInfo::setContent(const std::string& v) {
    m_content = v;
}

void CommentInfo::setIp(const std::string& v) {
    m_ip = v;
}

void CommentInfo::setUserAgent(const std::string& v) {
    m_userAgent = v;
}

void CommentInfo::setStatus(const int32_t& v) {
    m_status = v;
}

void CommentInfo::setIsReported(const int32_t& v) {
    m_isReported = v;
}

void CommentInfo::setReportCount(const int32_t& v) {
    m_reportCount = v;
}

void CommentInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void CommentInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void CommentInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int CommentInfoDao::Update(CommentInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update comment set article_id = ?, user_id = ?, parent_id = ?, content = ?, ip = ?, user_agent = ?, status = ?, is_reported = ?, report_count = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_articleId);
    stmt->bindInt64(2, info->m_userId);
    stmt->bindInt64(3, info->m_parentId);
    stmt->bindString(4, info->m_content);
    stmt->bindString(5, info->m_ip);
    stmt->bindString(6, info->m_userAgent);
    stmt->bindInt32(7, info->m_status);
    stmt->bindInt32(8, info->m_isReported);
    stmt->bindInt32(9, info->m_reportCount);
    stmt->bindInt32(10, info->m_isDeleted);
    stmt->bindTime(11, info->m_createTime);
    stmt->bindTime(12, info->m_updateTime);
    stmt->bindInt64(13, info->m_id);
    return stmt->execute();
}

int CommentInfoDao::Insert(CommentInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into comment (article_id, user_id, parent_id, content, ip, user_agent, status, is_reported, report_count, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_articleId);
    stmt->bindInt64(2, info->m_userId);
    stmt->bindInt64(3, info->m_parentId);
    stmt->bindString(4, info->m_content);
    stmt->bindString(5, info->m_ip);
    stmt->bindString(6, info->m_userAgent);
    stmt->bindInt32(7, info->m_status);
    stmt->bindInt32(8, info->m_isReported);
    stmt->bindInt32(9, info->m_reportCount);
    stmt->bindInt32(10, info->m_isDeleted);
    stmt->bindTime(11, info->m_createTime);
    stmt->bindTime(12, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int CommentInfoDao::InsertOrUpdate(CommentInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into comment (id, article_id, user_id, parent_id, content, ip, user_agent, status, is_reported, report_count, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_articleId);
    stmt->bindInt64(3, info->m_userId);
    stmt->bindInt64(4, info->m_parentId);
    stmt->bindString(5, info->m_content);
    stmt->bindString(6, info->m_ip);
    stmt->bindString(7, info->m_userAgent);
    stmt->bindInt32(8, info->m_status);
    stmt->bindInt32(9, info->m_isReported);
    stmt->bindInt32(10, info->m_reportCount);
    stmt->bindInt32(11, info->m_isDeleted);
    stmt->bindTime(12, info->m_createTime);
    stmt->bindTime(13, info->m_updateTime);
    return stmt->execute();
}

int CommentInfoDao::Delete(CommentInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from comment where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int CommentInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from comment where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int CommentInfoDao::DeleteByArticleId( const int64_t& article_id, chen::IDB::ptr conn) {
    std::string sql = "delete from comment where article_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, article_id);
    return stmt->execute();
}

int CommentInfoDao::DeleteByUserId( const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "delete from comment where user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    return stmt->execute();
}

int CommentInfoDao::DeleteByParentId( const int64_t& parent_id, chen::IDB::ptr conn) {
    std::string sql = "delete from comment where parent_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, parent_id);
    return stmt->execute();
}

int CommentInfoDao::DeleteByStatus( const int32_t& status, chen::IDB::ptr conn) {
    std::string sql = "delete from comment where status = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt32(1, status);
    return stmt->execute();
}

int CommentInfoDao::QueryAll(std::vector<CommentInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, article_id, user_id, parent_id, content, ip, user_agent, status, is_reported, report_count, is_deleted, create_time, update_time from comment";
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
        CommentInfo::ptr v(new CommentInfo);
        v->m_id = rt->getInt64(0);
        v->m_articleId = rt->getInt64(1);
        v->m_userId = rt->getInt64(2);
        v->m_parentId = rt->getInt64(3);
        v->m_content = rt->getString(4);
        v->m_ip = rt->getString(5);
        v->m_userAgent = rt->getString(6);
        v->m_status = rt->getInt32(7);
        v->m_isReported = rt->getInt32(8);
        v->m_reportCount = rt->getInt32(9);
        v->m_isDeleted = rt->getInt32(10);
        v->m_createTime = rt->getTime(11);
        v->m_updateTime = rt->getTime(12);
        results.push_back(v);
    }
    return 0;
}

CommentInfo::ptr CommentInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, article_id, user_id, parent_id, content, ip, user_agent, status, is_reported, report_count, is_deleted, create_time, update_time from comment where id = ?";
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
    CommentInfo::ptr v(new CommentInfo);
    v->m_id = rt->getInt64(0);
    v->m_articleId = rt->getInt64(1);
    v->m_userId = rt->getInt64(2);
    v->m_parentId = rt->getInt64(3);
    v->m_content = rt->getString(4);
    v->m_ip = rt->getString(5);
    v->m_userAgent = rt->getString(6);
    v->m_status = rt->getInt32(7);
    v->m_isReported = rt->getInt32(8);
    v->m_reportCount = rt->getInt32(9);
    v->m_isDeleted = rt->getInt32(10);
    v->m_createTime = rt->getTime(11);
    v->m_updateTime = rt->getTime(12);
    return v;
}

int CommentInfoDao::QueryByArticleId(std::vector<CommentInfo::ptr>& results,  const int64_t& article_id, chen::IDB::ptr conn) {
    std::string sql = "select id, article_id, user_id, parent_id, content, ip, user_agent, status, is_reported, report_count, is_deleted, create_time, update_time from comment where article_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, article_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        CommentInfo::ptr v(new CommentInfo);
        v->m_id = rt->getInt64(0);
        v->m_articleId = rt->getInt64(1);
        v->m_userId = rt->getInt64(2);
        v->m_parentId = rt->getInt64(3);
        v->m_content = rt->getString(4);
        v->m_ip = rt->getString(5);
        v->m_userAgent = rt->getString(6);
        v->m_status = rt->getInt32(7);
        v->m_isReported = rt->getInt32(8);
        v->m_reportCount = rt->getInt32(9);
        v->m_isDeleted = rt->getInt32(10);
        v->m_createTime = rt->getTime(11);
        v->m_updateTime = rt->getTime(12);
        results.push_back(v);
    };
    return 0;
}

int CommentInfoDao::QueryByUserId(std::vector<CommentInfo::ptr>& results,  const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "select id, article_id, user_id, parent_id, content, ip, user_agent, status, is_reported, report_count, is_deleted, create_time, update_time from comment where user_id = ?";
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
        CommentInfo::ptr v(new CommentInfo);
        v->m_id = rt->getInt64(0);
        v->m_articleId = rt->getInt64(1);
        v->m_userId = rt->getInt64(2);
        v->m_parentId = rt->getInt64(3);
        v->m_content = rt->getString(4);
        v->m_ip = rt->getString(5);
        v->m_userAgent = rt->getString(6);
        v->m_status = rt->getInt32(7);
        v->m_isReported = rt->getInt32(8);
        v->m_reportCount = rt->getInt32(9);
        v->m_isDeleted = rt->getInt32(10);
        v->m_createTime = rt->getTime(11);
        v->m_updateTime = rt->getTime(12);
        results.push_back(v);
    };
    return 0;
}

int CommentInfoDao::QueryByParentId(std::vector<CommentInfo::ptr>& results,  const int64_t& parent_id, chen::IDB::ptr conn) {
    std::string sql = "select id, article_id, user_id, parent_id, content, ip, user_agent, status, is_reported, report_count, is_deleted, create_time, update_time from comment where parent_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, parent_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        CommentInfo::ptr v(new CommentInfo);
        v->m_id = rt->getInt64(0);
        v->m_articleId = rt->getInt64(1);
        v->m_userId = rt->getInt64(2);
        v->m_parentId = rt->getInt64(3);
        v->m_content = rt->getString(4);
        v->m_ip = rt->getString(5);
        v->m_userAgent = rt->getString(6);
        v->m_status = rt->getInt32(7);
        v->m_isReported = rt->getInt32(8);
        v->m_reportCount = rt->getInt32(9);
        v->m_isDeleted = rt->getInt32(10);
        v->m_createTime = rt->getTime(11);
        v->m_updateTime = rt->getTime(12);
        results.push_back(v);
    };
    return 0;
}

int CommentInfoDao::QueryByStatus(std::vector<CommentInfo::ptr>& results,  const int32_t& status, chen::IDB::ptr conn) {
    std::string sql = "select id, article_id, user_id, parent_id, content, ip, user_agent, status, is_reported, report_count, is_deleted, create_time, update_time from comment where status = ?";
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
        CommentInfo::ptr v(new CommentInfo);
        v->m_id = rt->getInt64(0);
        v->m_articleId = rt->getInt64(1);
        v->m_userId = rt->getInt64(2);
        v->m_parentId = rt->getInt64(3);
        v->m_content = rt->getString(4);
        v->m_ip = rt->getString(5);
        v->m_userAgent = rt->getString(6);
        v->m_status = rt->getInt32(7);
        v->m_isReported = rt->getInt32(8);
        v->m_reportCount = rt->getInt32(9);
        v->m_isDeleted = rt->getInt32(10);
        v->m_createTime = rt->getTime(11);
        v->m_updateTime = rt->getTime(12);
        results.push_back(v);
    };
    return 0;
}

int CommentInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS comment("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "article_id INTEGER NOT NULL DEFAULT 0,"
            "user_id INTEGER NOT NULL DEFAULT 0,"
            "parent_id INTEGER NOT NULL DEFAULT 0,"
            "content TEXT NOT NULL DEFAULT '',"
            "ip TEXT NOT NULL DEFAULT '',"
            "user_agent TEXT NOT NULL DEFAULT '',"
            "status INTEGER NOT NULL DEFAULT 1,"
            "is_reported INTEGER NOT NULL DEFAULT 0,"
            "report_count INTEGER NOT NULL DEFAULT 0,"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE INDEX IF NOT EXISTS comment_article_id ON comment(article_id);"
            "CREATE INDEX IF NOT EXISTS comment_user_id ON comment(user_id);"
            "CREATE INDEX IF NOT EXISTS comment_parent_id ON comment(parent_id);"
            "CREATE INDEX IF NOT EXISTS comment_status ON comment(status);"
            );
}

int CommentInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS comment("
            "`id` bigint AUTO_INCREMENT COMMENT '评论ID',"
            "`article_id` bigint NOT NULL DEFAULT 0 COMMENT '文章ID',"
            "`user_id` bigint NOT NULL DEFAULT 0 COMMENT '评论用户ID',"
            "`parent_id` bigint NOT NULL DEFAULT 0 COMMENT '父评论ID(0=顶级评论)',"
            "`content` text NOT NULL DEFAULT '' COMMENT '评论内容',"
            "`ip` varchar(64) NOT NULL DEFAULT '' COMMENT '客户端IP',"
            "`user_agent` varchar(512) NOT NULL DEFAULT '' COMMENT '客户端UserAgent',"
            "`status` int NOT NULL DEFAULT 1 COMMENT '状态 1:待审核 2:已通过 3:已拒绝 4:垃圾',"
            "`is_reported` int NOT NULL DEFAULT 0 COMMENT '是否被举报',"
            "`report_count` int NOT NULL DEFAULT 0 COMMENT '举报次数',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "KEY `comment_article_id` (`article_id`),"
            "KEY `comment_user_id` (`user_id`),"
            "KEY `comment_parent_id` (`parent_id`),"
            "KEY `comment_status` (`status`)) COMMENT='文章评论'");
}
} //namespace data
} //namespace blog
