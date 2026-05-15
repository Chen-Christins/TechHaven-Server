#include "comment_praise_rel_info.h"
#include "chen/log/log.h"

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

CommentPraiseRelInfo::CommentPraiseRelInfo()
    :m_isDeleted(0)
    ,m_id()
    ,m_userId()
    ,m_commentId()
    ,m_createTime()
    ,m_updateTime() {
}

std::string CommentPraiseRelInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["user_id"] = std::to_string(m_userId);
    v["comment_id"] = std::to_string(m_commentId);
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void CommentPraiseRelInfo::setId(const int64_t& v) {
    m_id = v;
}

void CommentPraiseRelInfo::setUserId(const int64_t& v) {
    m_userId = v;
}

void CommentPraiseRelInfo::setCommentId(const int64_t& v) {
    m_commentId = v;
}

void CommentPraiseRelInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void CommentPraiseRelInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void CommentPraiseRelInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int CommentPraiseRelInfoDao::Update(CommentPraiseRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update comment_praise_rel set user_id = ?, comment_id = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_userId);
    stmt->bindInt64(2, info->m_commentId);
    stmt->bindInt32(3, info->m_isDeleted);
    stmt->bindTime(4, info->m_createTime);
    stmt->bindTime(5, info->m_updateTime);
    stmt->bindInt64(6, info->m_id);
    return stmt->execute();
}

int CommentPraiseRelInfoDao::Insert(CommentPraiseRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into comment_praise_rel (user_id, comment_id, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_userId);
    stmt->bindInt64(2, info->m_commentId);
    stmt->bindInt32(3, info->m_isDeleted);
    stmt->bindTime(4, info->m_createTime);
    stmt->bindTime(5, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int CommentPraiseRelInfoDao::InsertOrUpdate(CommentPraiseRelInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into comment_praise_rel (id, user_id, comment_id, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_userId);
    stmt->bindInt64(3, info->m_commentId);
    stmt->bindInt32(4, info->m_isDeleted);
    stmt->bindTime(5, info->m_createTime);
    stmt->bindTime(6, info->m_updateTime);
    return stmt->execute();
}

int CommentPraiseRelInfoDao::Delete(CommentPraiseRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from comment_praise_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int CommentPraiseRelInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from comment_praise_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int CommentPraiseRelInfoDao::DeleteByUserIdCommentId( const int64_t& user_id,  const int64_t& comment_id, chen::IDB::ptr conn) {
    std::string sql = "delete from comment_praise_rel where user_id = ? and comment_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    stmt->bindInt64(1, comment_id);
    return stmt->execute();
}

int CommentPraiseRelInfoDao::DeleteByUserId( const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "delete from comment_praise_rel where user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    return stmt->execute();
}

int CommentPraiseRelInfoDao::DeleteByCommentId( const int64_t& comment_id, chen::IDB::ptr conn) {
    std::string sql = "delete from comment_praise_rel where comment_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, comment_id);
    return stmt->execute();
}

int CommentPraiseRelInfoDao::QueryAll(std::vector<CommentPraiseRelInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, comment_id, is_deleted, create_time, update_time from comment_praise_rel";
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
        CommentPraiseRelInfo::ptr v(new CommentPraiseRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_commentId = rt->getInt64(2);
        v->m_isDeleted = rt->getInt32(3);
        v->m_createTime = rt->getTime(4);
        v->m_updateTime = rt->getTime(5);
        results.push_back(v);
    }
    return 0;
}

CommentPraiseRelInfo::ptr CommentPraiseRelInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, comment_id, is_deleted, create_time, update_time from comment_praise_rel where id = ?";
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
    CommentPraiseRelInfo::ptr v(new CommentPraiseRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_userId = rt->getInt64(1);
    v->m_commentId = rt->getInt64(2);
    v->m_isDeleted = rt->getInt32(3);
    v->m_createTime = rt->getTime(4);
    v->m_updateTime = rt->getTime(5);
    return v;
}

CommentPraiseRelInfo::ptr CommentPraiseRelInfoDao::QueryByUserIdCommentId( const int64_t& user_id,  const int64_t& comment_id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, comment_id, is_deleted, create_time, update_time from comment_praise_rel where user_id = ? and comment_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindInt64(1, user_id);
    stmt->bindInt64(2, comment_id);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    CommentPraiseRelInfo::ptr v(new CommentPraiseRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_userId = rt->getInt64(1);
    v->m_commentId = rt->getInt64(2);
    v->m_isDeleted = rt->getInt32(3);
    v->m_createTime = rt->getTime(4);
    v->m_updateTime = rt->getTime(5);
    return v;
}

int CommentPraiseRelInfoDao::QueryByUserId(std::vector<CommentPraiseRelInfo::ptr>& results,  const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, comment_id, is_deleted, create_time, update_time from comment_praise_rel where user_id = ?";
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
        CommentPraiseRelInfo::ptr v(new CommentPraiseRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_commentId = rt->getInt64(2);
        v->m_isDeleted = rt->getInt32(3);
        v->m_createTime = rt->getTime(4);
        v->m_updateTime = rt->getTime(5);
        results.push_back(v);
    };
    return 0;
}

int CommentPraiseRelInfoDao::QueryByCommentId(std::vector<CommentPraiseRelInfo::ptr>& results,  const int64_t& comment_id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, comment_id, is_deleted, create_time, update_time from comment_praise_rel where comment_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, comment_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        CommentPraiseRelInfo::ptr v(new CommentPraiseRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_commentId = rt->getInt64(2);
        v->m_isDeleted = rt->getInt32(3);
        v->m_createTime = rt->getTime(4);
        v->m_updateTime = rt->getTime(5);
        results.push_back(v);
    };
    return 0;
}

int CommentPraiseRelInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS comment_praise_rel("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "user_id INTEGER NOT NULL DEFAULT 0,"
            "comment_id INTEGER NOT NULL DEFAULT 0,"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE UNIQUE INDEX IF NOT EXISTS comment_praise_rel_user_id_comment_id ON comment_praise_rel(user_id,comment_id);"
            "CREATE INDEX IF NOT EXISTS comment_praise_rel_user_id ON comment_praise_rel(user_id);"
            "CREATE INDEX IF NOT EXISTS comment_praise_rel_comment_id ON comment_praise_rel(comment_id);"
            );
}

int CommentPraiseRelInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS comment_praise_rel("
            "`id` bigint AUTO_INCREMENT COMMENT '主键ID',"
            "`user_id` bigint NOT NULL DEFAULT 0 COMMENT '点赞用户ID',"
            "`comment_id` bigint NOT NULL DEFAULT 0 COMMENT '评论ID',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "UNIQUE KEY `comment_praise_rel_user_id_comment_id` (`user_id`,`comment_id`),"
            "KEY `comment_praise_rel_user_id` (`user_id`),"
            "KEY `comment_praise_rel_comment_id` (`comment_id`)) COMMENT='评论点赞关联表'");
}
} //namespace data
} //namespace blog
