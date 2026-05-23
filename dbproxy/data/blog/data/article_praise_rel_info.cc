#include "article_praise_rel_info.h"
#include "chen/log/log.h"
#include <set>

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

ArticlePraiseRelInfo::ArticlePraiseRelInfo()
    :m_isDeleted(0)
    ,m_id()
    ,m_userId()
    ,m_articleId()
    ,m_createTime()
    ,m_updateTime() {
}

std::string ArticlePraiseRelInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["user_id"] = std::to_string(m_userId);
    v["article_id"] = std::to_string(m_articleId);
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void ArticlePraiseRelInfo::setId(const int64_t& v) {
    m_id = v;
}

void ArticlePraiseRelInfo::setUserId(const int64_t& v) {
    m_userId = v;
}

void ArticlePraiseRelInfo::setArticleId(const int64_t& v) {
    m_articleId = v;
}

void ArticlePraiseRelInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void ArticlePraiseRelInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void ArticlePraiseRelInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int ArticlePraiseRelInfoDao::Update(ArticlePraiseRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update article_praise_rel set user_id = ?, article_id = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_userId);
    stmt->bindInt64(2, info->m_articleId);
    stmt->bindInt32(3, info->m_isDeleted);
    stmt->bindTime(4, info->m_createTime);
    stmt->bindTime(5, info->m_updateTime);
    stmt->bindInt64(6, info->m_id);
    return stmt->execute();
}

int ArticlePraiseRelInfoDao::Insert(ArticlePraiseRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into article_praise_rel (user_id, article_id, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_userId);
    stmt->bindInt64(2, info->m_articleId);
    stmt->bindInt32(3, info->m_isDeleted);
    stmt->bindTime(4, info->m_createTime);
    stmt->bindTime(5, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int ArticlePraiseRelInfoDao::InsertOrUpdate(ArticlePraiseRelInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into article_praise_rel (id, user_id, article_id, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_userId);
    stmt->bindInt64(3, info->m_articleId);
    stmt->bindInt32(4, info->m_isDeleted);
    stmt->bindTime(5, info->m_createTime);
    stmt->bindTime(6, info->m_updateTime);
    return stmt->execute();
}

int ArticlePraiseRelInfoDao::Delete(ArticlePraiseRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from article_praise_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int ArticlePraiseRelInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from article_praise_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int ArticlePraiseRelInfoDao::DeleteByUserIdArticleId( const int64_t& user_id,  const int64_t& article_id, chen::IDB::ptr conn) {
    std::string sql = "delete from article_praise_rel where user_id = ? and article_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    stmt->bindInt64(1, article_id);
    return stmt->execute();
}

int ArticlePraiseRelInfoDao::DeleteByUserId( const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "delete from article_praise_rel where user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    return stmt->execute();
}

int ArticlePraiseRelInfoDao::DeleteByArticleId( const int64_t& article_id, chen::IDB::ptr conn) {
    std::string sql = "delete from article_praise_rel where article_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, article_id);
    return stmt->execute();
}

int ArticlePraiseRelInfoDao::QueryAll(std::vector<ArticlePraiseRelInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, article_id, is_deleted, create_time, update_time from article_praise_rel";
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
        ArticlePraiseRelInfo::ptr v(new ArticlePraiseRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_articleId = rt->getInt64(2);
        v->m_isDeleted = rt->getInt32(3);
        v->m_createTime = rt->getTime(4);
        v->m_updateTime = rt->getTime(5);
        results.push_back(v);
    }
    return 0;
}

ArticlePraiseRelInfo::ptr ArticlePraiseRelInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, article_id, is_deleted, create_time, update_time from article_praise_rel where id = ?";
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
    ArticlePraiseRelInfo::ptr v(new ArticlePraiseRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_userId = rt->getInt64(1);
    v->m_articleId = rt->getInt64(2);
    v->m_isDeleted = rt->getInt32(3);
    v->m_createTime = rt->getTime(4);
    v->m_updateTime = rt->getTime(5);
    return v;
}

ArticlePraiseRelInfo::ptr ArticlePraiseRelInfoDao::QueryByUserIdArticleId( const int64_t& user_id,  const int64_t& article_id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, article_id, is_deleted, create_time, update_time from article_praise_rel where user_id = ? and article_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindInt64(1, user_id);
    stmt->bindInt64(2, article_id);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    ArticlePraiseRelInfo::ptr v(new ArticlePraiseRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_userId = rt->getInt64(1);
    v->m_articleId = rt->getInt64(2);
    v->m_isDeleted = rt->getInt32(3);
    v->m_createTime = rt->getTime(4);
    v->m_updateTime = rt->getTime(5);
    return v;
}

int ArticlePraiseRelInfoDao::QueryByUserId(std::vector<ArticlePraiseRelInfo::ptr>& results,  const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, article_id, is_deleted, create_time, update_time from article_praise_rel where user_id = ?";
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
        ArticlePraiseRelInfo::ptr v(new ArticlePraiseRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_articleId = rt->getInt64(2);
        v->m_isDeleted = rt->getInt32(3);
        v->m_createTime = rt->getTime(4);
        v->m_updateTime = rt->getTime(5);
        results.push_back(v);
    };
    return 0;
}

int ArticlePraiseRelInfoDao::QueryByArticleId(std::vector<ArticlePraiseRelInfo::ptr>& results,  const int64_t& article_id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, article_id, is_deleted, create_time, update_time from article_praise_rel where article_id = ?";
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
        ArticlePraiseRelInfo::ptr v(new ArticlePraiseRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_articleId = rt->getInt64(2);
        v->m_isDeleted = rt->getInt32(3);
        v->m_createTime = rt->getTime(4);
        v->m_updateTime = rt->getTime(5);
        results.push_back(v);
    };
    return 0;
}

int ArticlePraiseRelInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS article_praise_rel("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "user_id INTEGER NOT NULL DEFAULT 0,"
            "article_id INTEGER NOT NULL DEFAULT 0,"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE UNIQUE INDEX IF NOT EXISTS article_praise_rel_user_id_article_id ON article_praise_rel(user_id,article_id);"
            "CREATE INDEX IF NOT EXISTS article_praise_rel_user_id ON article_praise_rel(user_id);"
            "CREATE INDEX IF NOT EXISTS article_praise_rel_article_id ON article_praise_rel(article_id);"
            );
}

int ArticlePraiseRelInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS article_praise_rel("
            "`id` bigint AUTO_INCREMENT COMMENT '主键ID',"
            "`user_id` bigint NOT NULL DEFAULT 0 COMMENT '点赞用户ID',"
            "`article_id` bigint NOT NULL DEFAULT 0 COMMENT '文章ID',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "UNIQUE KEY `article_praise_rel_user_id_article_id` (`user_id`,`article_id`),"
            "KEY `article_praise_rel_user_id` (`user_id`),"
            "KEY `article_praise_rel_article_id` (`article_id`)) COMMENT='文章点赞关联表'");
}

int ArticlePraiseRelInfoDao::MigrateTableSQLite3(chen::IDB::ptr conn) {
    auto data = conn->query("PRAGMA table_info(article_praise_rel)");
    if (!data) {
        ERROR(logger) << "PRAGMA table_info(article_praise_rel) errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(1));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("user_id");
    expected_cols.insert("article_id");
    expected_cols.insert("is_deleted");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("user_id") == existing_cols.end()) {
        INFO(logger) << "Adding column article_praise_rel.user_id";
        int rt = conn->execute("ALTER TABLE article_praise_rel ADD COLUMN user_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_praise_rel ADD COLUMN user_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("article_id") == existing_cols.end()) {
        INFO(logger) << "Adding column article_praise_rel.article_id";
        int rt = conn->execute("ALTER TABLE article_praise_rel ADD COLUMN article_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_praise_rel ADD COLUMN article_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column article_praise_rel.is_deleted";
        int rt = conn->execute("ALTER TABLE article_praise_rel ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_praise_rel ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article_praise_rel.create_time";
        int rt = conn->execute("ALTER TABLE article_praise_rel ADD COLUMN create_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_praise_rel ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article_praise_rel.update_time";
        int rt = conn->execute("ALTER TABLE article_praise_rel ADD COLUMN update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_praise_rel ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column article_praise_rel." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE article_praise_rel DROP COLUMN " + col);
            if (rt) {
                ERROR(logger) << "ALTER TABLE article_praise_rel DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}

int ArticlePraiseRelInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM article_praise_rel");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM article_praise_rel errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(0));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("user_id");
    expected_cols.insert("article_id");
    expected_cols.insert("is_deleted");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("user_id") == existing_cols.end()) {
        INFO(logger) << "Adding column article_praise_rel.user_id";
        int rt = conn->execute("ALTER TABLE article_praise_rel ADD COLUMN `user_id` bigint NOT NULL DEFAULT 0 COMMENT '点赞用户ID'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_praise_rel ADD COLUMN user_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("article_id") == existing_cols.end()) {
        INFO(logger) << "Adding column article_praise_rel.article_id";
        int rt = conn->execute("ALTER TABLE article_praise_rel ADD COLUMN `article_id` bigint NOT NULL DEFAULT 0 COMMENT '文章ID'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_praise_rel ADD COLUMN article_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column article_praise_rel.is_deleted";
        int rt = conn->execute("ALTER TABLE article_praise_rel ADD COLUMN `is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_praise_rel ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article_praise_rel.create_time";
        int rt = conn->execute("ALTER TABLE article_praise_rel ADD COLUMN `create_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '创建时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_praise_rel ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article_praise_rel.update_time";
        int rt = conn->execute("ALTER TABLE article_praise_rel ADD COLUMN `update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '更新时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_praise_rel ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column article_praise_rel." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE article_praise_rel DROP COLUMN `" + col + "`");
            if (rt) {
                ERROR(logger) << "ALTER TABLE article_praise_rel DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}


} //namespace data
} //namespace blog
