#include "article_label_rel_info.h"
#include "chen/log/log.h"
#include <set>

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

ArticleLabelRelInfo::ArticleLabelRelInfo()
    :m_isDeleted()
    ,m_id()
    ,m_articleId()
    ,m_labelId()
    ,m_createTime(time(0))
    ,m_updateTime() {
}

std::string ArticleLabelRelInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["article_id"] = std::to_string(m_articleId);
    v["label_id"] = std::to_string(m_labelId);
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void ArticleLabelRelInfo::setId(const int64_t& v) {
    m_id = v;
}

void ArticleLabelRelInfo::setArticleId(const int64_t& v) {
    m_articleId = v;
}

void ArticleLabelRelInfo::setLabelId(const int64_t& v) {
    m_labelId = v;
}

void ArticleLabelRelInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void ArticleLabelRelInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void ArticleLabelRelInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int ArticleLabelRelInfoDao::Update(ArticleLabelRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update article_label_rel set article_id = ?, label_id = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_articleId);
    stmt->bindInt64(2, info->m_labelId);
    stmt->bindInt32(3, info->m_isDeleted);
    stmt->bindTime(4, info->m_createTime);
    stmt->bindTime(5, info->m_updateTime);
    stmt->bindInt64(6, info->m_id);
    return stmt->execute();
}

int ArticleLabelRelInfoDao::Insert(ArticleLabelRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into article_label_rel (article_id, label_id, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_articleId);
    stmt->bindInt64(2, info->m_labelId);
    stmt->bindInt32(3, info->m_isDeleted);
    stmt->bindTime(4, info->m_createTime);
    stmt->bindTime(5, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int ArticleLabelRelInfoDao::InsertOrUpdate(ArticleLabelRelInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into article_label_rel (id, article_id, label_id, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_articleId);
    stmt->bindInt64(3, info->m_labelId);
    stmt->bindInt32(4, info->m_isDeleted);
    stmt->bindTime(5, info->m_createTime);
    stmt->bindTime(6, info->m_updateTime);
    return stmt->execute();
}

int ArticleLabelRelInfoDao::Delete(ArticleLabelRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from article_label_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int ArticleLabelRelInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from article_label_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int ArticleLabelRelInfoDao::DeleteByArticleId( const int64_t& article_id, chen::IDB::ptr conn) {
    std::string sql = "delete from article_label_rel where article_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, article_id);
    return stmt->execute();
}

int ArticleLabelRelInfoDao::DeleteByArticleIdLabelId( const int64_t& article_id,  const int64_t& label_id, chen::IDB::ptr conn) {
    std::string sql = "delete from article_label_rel where article_id = ? and label_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, article_id);
    stmt->bindInt64(1, label_id);
    return stmt->execute();
}

int ArticleLabelRelInfoDao::QueryAll(std::vector<ArticleLabelRelInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, article_id, label_id, is_deleted, create_time, update_time from article_label_rel";
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
        ArticleLabelRelInfo::ptr v(new ArticleLabelRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_articleId = rt->getInt64(1);
        v->m_labelId = rt->getInt64(2);
        v->m_isDeleted = rt->getInt32(3);
        v->m_createTime = rt->getTime(4);
        v->m_updateTime = rt->getTime(5);
        results.push_back(v);
    }
    return 0;
}

ArticleLabelRelInfo::ptr ArticleLabelRelInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, article_id, label_id, is_deleted, create_time, update_time from article_label_rel where id = ?";
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
    ArticleLabelRelInfo::ptr v(new ArticleLabelRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_articleId = rt->getInt64(1);
    v->m_labelId = rt->getInt64(2);
    v->m_isDeleted = rt->getInt32(3);
    v->m_createTime = rt->getTime(4);
    v->m_updateTime = rt->getTime(5);
    return v;
}

int ArticleLabelRelInfoDao::QueryByArticleId(std::vector<ArticleLabelRelInfo::ptr>& results,  const int64_t& article_id, chen::IDB::ptr conn) {
    std::string sql = "select id, article_id, label_id, is_deleted, create_time, update_time from article_label_rel where article_id = ?";
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
        ArticleLabelRelInfo::ptr v(new ArticleLabelRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_articleId = rt->getInt64(1);
        v->m_labelId = rt->getInt64(2);
        v->m_isDeleted = rt->getInt32(3);
        v->m_createTime = rt->getTime(4);
        v->m_updateTime = rt->getTime(5);
        results.push_back(v);
    };
    return 0;
}

ArticleLabelRelInfo::ptr ArticleLabelRelInfoDao::QueryByArticleIdLabelId( const int64_t& article_id,  const int64_t& label_id, chen::IDB::ptr conn) {
    std::string sql = "select id, article_id, label_id, is_deleted, create_time, update_time from article_label_rel where article_id = ? and label_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindInt64(1, article_id);
    stmt->bindInt64(2, label_id);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    ArticleLabelRelInfo::ptr v(new ArticleLabelRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_articleId = rt->getInt64(1);
    v->m_labelId = rt->getInt64(2);
    v->m_isDeleted = rt->getInt32(3);
    v->m_createTime = rt->getTime(4);
    v->m_updateTime = rt->getTime(5);
    return v;
}

int ArticleLabelRelInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS article_label_rel("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "article_id INTEGER NOT NULL DEFAULT 0,"
            "label_id INTEGER NOT NULL DEFAULT 0,"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE INDEX IF NOT EXISTS article_label_rel_article_id ON article_label_rel(article_id);"
            "CREATE UNIQUE INDEX IF NOT EXISTS article_label_rel_article_id_label_id ON article_label_rel(article_id,label_id);"
            );
}

int ArticleLabelRelInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS article_label_rel("
            "`id` bigint AUTO_INCREMENT,"
            "`article_id` bigint NOT NULL DEFAULT 0,"
            "`label_id` bigint NOT NULL DEFAULT 0,"
            "`is_deleted` int NOT NULL DEFAULT 0,"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp,"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp ,"
            "PRIMARY KEY(`id`),"
            "KEY `article_label_rel_article_id` (`article_id`),"
            "UNIQUE KEY `article_label_rel_article_id_label_id` (`article_id`,`label_id`))");
}

int ArticleLabelRelInfoDao::MigrateTableSQLite3(chen::IDB::ptr conn) {
    auto data = conn->query("PRAGMA table_info(article_label_rel)");
    if (!data) {
        ERROR(logger) << "PRAGMA table_info(article_label_rel) errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(1));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("article_id");
    expected_cols.insert("label_id");
    expected_cols.insert("is_deleted");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("article_id") == existing_cols.end()) {
        INFO(logger) << "Adding column article_label_rel.article_id";
        int rt = conn->execute("ALTER TABLE article_label_rel ADD COLUMN article_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_label_rel ADD COLUMN article_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("label_id") == existing_cols.end()) {
        INFO(logger) << "Adding column article_label_rel.label_id";
        int rt = conn->execute("ALTER TABLE article_label_rel ADD COLUMN label_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_label_rel ADD COLUMN label_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column article_label_rel.is_deleted";
        int rt = conn->execute("ALTER TABLE article_label_rel ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_label_rel ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article_label_rel.create_time";
        int rt = conn->execute("ALTER TABLE article_label_rel ADD COLUMN create_time TIMESTAMP NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_label_rel ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article_label_rel.update_time";
        int rt = conn->execute("ALTER TABLE article_label_rel ADD COLUMN update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_label_rel ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column article_label_rel." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE article_label_rel DROP COLUMN " + col);
            if (rt) {
                ERROR(logger) << "ALTER TABLE article_label_rel DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}

int ArticleLabelRelInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM article_label_rel");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM article_label_rel errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(0));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("article_id");
    expected_cols.insert("label_id");
    expected_cols.insert("is_deleted");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("article_id") == existing_cols.end()) {
        INFO(logger) << "Adding column article_label_rel.article_id";
        int rt = conn->execute("ALTER TABLE article_label_rel ADD COLUMN `article_id` bigint NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_label_rel ADD COLUMN article_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("label_id") == existing_cols.end()) {
        INFO(logger) << "Adding column article_label_rel.label_id";
        int rt = conn->execute("ALTER TABLE article_label_rel ADD COLUMN `label_id` bigint NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_label_rel ADD COLUMN label_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column article_label_rel.is_deleted";
        int rt = conn->execute("ALTER TABLE article_label_rel ADD COLUMN `is_deleted` int NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_label_rel ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article_label_rel.create_time";
        int rt = conn->execute("ALTER TABLE article_label_rel ADD COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_label_rel ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article_label_rel.update_time";
        int rt = conn->execute("ALTER TABLE article_label_rel ADD COLUMN `update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_label_rel ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column article_label_rel." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE article_label_rel DROP COLUMN `" + col + "`");
            if (rt) {
                ERROR(logger) << "ALTER TABLE article_label_rel DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}


} //namespace data
} //namespace blog
