#include "article_category_rel_info.h"
#include "chen/log/log.h"
#include <set>

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

ArticleCategoryRelInfo::ArticleCategoryRelInfo()
    :m_isDeleted()
    ,m_id()
    ,m_articleId()
    ,m_categoryId()
    ,m_publishTime()
    ,m_createTime(time(0))
    ,m_updateTime() {
}

std::string ArticleCategoryRelInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["article_id"] = std::to_string(m_articleId);
    v["category_id"] = std::to_string(m_categoryId);
    v["is_deleted"] = m_isDeleted;
    v["publish_time"] = chen::Time2Str(m_publishTime);
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void ArticleCategoryRelInfo::setId(const int64_t& v) {
    m_id = v;
}

void ArticleCategoryRelInfo::setArticleId(const int64_t& v) {
    m_articleId = v;
}

void ArticleCategoryRelInfo::setCategoryId(const int64_t& v) {
    m_categoryId = v;
}

void ArticleCategoryRelInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void ArticleCategoryRelInfo::setPublishTime(const int64_t& v) {
    m_publishTime = v;
}

void ArticleCategoryRelInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void ArticleCategoryRelInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int ArticleCategoryRelInfoDao::Update(ArticleCategoryRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update article_category_rel set article_id = ?, category_id = ?, is_deleted = ?, publish_time = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_articleId);
    stmt->bindInt64(2, info->m_categoryId);
    stmt->bindInt32(3, info->m_isDeleted);
    stmt->bindTime(4, info->m_publishTime);
    stmt->bindTime(5, info->m_createTime);
    stmt->bindTime(6, info->m_updateTime);
    stmt->bindInt64(7, info->m_id);
    return stmt->execute();
}

int ArticleCategoryRelInfoDao::Insert(ArticleCategoryRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into article_category_rel (article_id, category_id, is_deleted, publish_time, create_time, update_time) values (?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_articleId);
    stmt->bindInt64(2, info->m_categoryId);
    stmt->bindInt32(3, info->m_isDeleted);
    stmt->bindTime(4, info->m_publishTime);
    stmt->bindTime(5, info->m_createTime);
    stmt->bindTime(6, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int ArticleCategoryRelInfoDao::InsertOrUpdate(ArticleCategoryRelInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into article_category_rel (id, article_id, category_id, is_deleted, publish_time, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_articleId);
    stmt->bindInt64(3, info->m_categoryId);
    stmt->bindInt32(4, info->m_isDeleted);
    stmt->bindTime(5, info->m_publishTime);
    stmt->bindTime(6, info->m_createTime);
    stmt->bindTime(7, info->m_updateTime);
    return stmt->execute();
}

int ArticleCategoryRelInfoDao::Delete(ArticleCategoryRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from article_category_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int ArticleCategoryRelInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from article_category_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int ArticleCategoryRelInfoDao::DeleteByArticleId( const int64_t& article_id, chen::IDB::ptr conn) {
    std::string sql = "delete from article_category_rel where article_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, article_id);
    return stmt->execute();
}

int ArticleCategoryRelInfoDao::DeleteByArticleIdCategoryId( const int64_t& article_id,  const int64_t& category_id, chen::IDB::ptr conn) {
    std::string sql = "delete from article_category_rel where article_id = ? and category_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, article_id);
    stmt->bindInt64(1, category_id);
    return stmt->execute();
}

int ArticleCategoryRelInfoDao::QueryAll(std::vector<ArticleCategoryRelInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, article_id, category_id, is_deleted, publish_time, create_time, update_time from article_category_rel";
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
        ArticleCategoryRelInfo::ptr v(new ArticleCategoryRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_articleId = rt->getInt64(1);
        v->m_categoryId = rt->getInt64(2);
        v->m_isDeleted = rt->getInt32(3);
        v->m_publishTime = rt->getTime(4);
        v->m_createTime = rt->getTime(5);
        v->m_updateTime = rt->getTime(6);
        results.push_back(v);
    }
    return 0;
}

ArticleCategoryRelInfo::ptr ArticleCategoryRelInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, article_id, category_id, is_deleted, publish_time, create_time, update_time from article_category_rel where id = ?";
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
    ArticleCategoryRelInfo::ptr v(new ArticleCategoryRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_articleId = rt->getInt64(1);
    v->m_categoryId = rt->getInt64(2);
    v->m_isDeleted = rt->getInt32(3);
    v->m_publishTime = rt->getTime(4);
    v->m_createTime = rt->getTime(5);
    v->m_updateTime = rt->getTime(6);
    return v;
}

int ArticleCategoryRelInfoDao::QueryByArticleId(std::vector<ArticleCategoryRelInfo::ptr>& results,  const int64_t& article_id, chen::IDB::ptr conn) {
    std::string sql = "select id, article_id, category_id, is_deleted, publish_time, create_time, update_time from article_category_rel where article_id = ?";
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
        ArticleCategoryRelInfo::ptr v(new ArticleCategoryRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_articleId = rt->getInt64(1);
        v->m_categoryId = rt->getInt64(2);
        v->m_isDeleted = rt->getInt32(3);
        v->m_publishTime = rt->getTime(4);
        v->m_createTime = rt->getTime(5);
        v->m_updateTime = rt->getTime(6);
        results.push_back(v);
    };
    return 0;
}

ArticleCategoryRelInfo::ptr ArticleCategoryRelInfoDao::QueryByArticleIdCategoryId( const int64_t& article_id,  const int64_t& category_id, chen::IDB::ptr conn) {
    std::string sql = "select id, article_id, category_id, is_deleted, publish_time, create_time, update_time from article_category_rel where article_id = ? and category_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindInt64(1, article_id);
    stmt->bindInt64(2, category_id);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    ArticleCategoryRelInfo::ptr v(new ArticleCategoryRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_articleId = rt->getInt64(1);
    v->m_categoryId = rt->getInt64(2);
    v->m_isDeleted = rt->getInt32(3);
    v->m_publishTime = rt->getTime(4);
    v->m_createTime = rt->getTime(5);
    v->m_updateTime = rt->getTime(6);
    return v;
}

int ArticleCategoryRelInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS article_category_rel("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "article_id INTEGER NOT NULL DEFAULT 0,"
            "category_id INTEGER NOT NULL DEFAULT 0,"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "publish_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE INDEX IF NOT EXISTS article_category_rel_article_id ON article_category_rel(article_id);"
            "CREATE UNIQUE INDEX IF NOT EXISTS article_category_rel_article_id_category_id ON article_category_rel(article_id,category_id);"
            );
}

int ArticleCategoryRelInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS article_category_rel("
            "`id` bigint AUTO_INCREMENT,"
            "`article_id` bigint NOT NULL DEFAULT 0,"
            "`category_id` bigint NOT NULL DEFAULT 0,"
            "`is_deleted` int NOT NULL DEFAULT 0,"
            "`publish_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp,"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp ,"
            "PRIMARY KEY(`id`),"
            "KEY `article_category_rel_article_id` (`article_id`),"
            "UNIQUE KEY `article_category_rel_article_id_category_id` (`article_id`,`category_id`))");
}

int ArticleCategoryRelInfoDao::MigrateTableSQLite3(chen::IDB::ptr conn) {
    auto data = conn->query("PRAGMA table_info(article_category_rel)");
    if (!data) {
        ERROR(logger) << "PRAGMA table_info(article_category_rel) errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(1));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("article_id");
    expected_cols.insert("category_id");
    expected_cols.insert("is_deleted");
    expected_cols.insert("publish_time");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("article_id") == existing_cols.end()) {
        INFO(logger) << "Adding column article_category_rel.article_id";
        int rt = conn->execute("ALTER TABLE article_category_rel ADD COLUMN article_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_category_rel ADD COLUMN article_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("category_id") == existing_cols.end()) {
        INFO(logger) << "Adding column article_category_rel.category_id";
        int rt = conn->execute("ALTER TABLE article_category_rel ADD COLUMN category_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_category_rel ADD COLUMN category_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column article_category_rel.is_deleted";
        int rt = conn->execute("ALTER TABLE article_category_rel ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_category_rel ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("publish_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article_category_rel.publish_time";
        int rt = conn->execute("ALTER TABLE article_category_rel ADD COLUMN publish_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_category_rel ADD COLUMN publish_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article_category_rel.create_time";
        int rt = conn->execute("ALTER TABLE article_category_rel ADD COLUMN create_time TIMESTAMP NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_category_rel ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article_category_rel.update_time";
        int rt = conn->execute("ALTER TABLE article_category_rel ADD COLUMN update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_category_rel ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column article_category_rel." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE article_category_rel DROP COLUMN " + col);
            if (rt) {
                ERROR(logger) << "ALTER TABLE article_category_rel DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}

int ArticleCategoryRelInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM article_category_rel");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM article_category_rel errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(0));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("article_id");
    expected_cols.insert("category_id");
    expected_cols.insert("is_deleted");
    expected_cols.insert("publish_time");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("article_id") == existing_cols.end()) {
        INFO(logger) << "Adding column article_category_rel.article_id";
        int rt = conn->execute("ALTER TABLE article_category_rel ADD COLUMN `article_id` bigint NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_category_rel ADD COLUMN article_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("category_id") == existing_cols.end()) {
        INFO(logger) << "Adding column article_category_rel.category_id";
        int rt = conn->execute("ALTER TABLE article_category_rel ADD COLUMN `category_id` bigint NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_category_rel ADD COLUMN category_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column article_category_rel.is_deleted";
        int rt = conn->execute("ALTER TABLE article_category_rel ADD COLUMN `is_deleted` int NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_category_rel ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("publish_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article_category_rel.publish_time";
        int rt = conn->execute("ALTER TABLE article_category_rel ADD COLUMN `publish_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_category_rel ADD COLUMN publish_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article_category_rel.create_time";
        int rt = conn->execute("ALTER TABLE article_category_rel ADD COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_category_rel ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article_category_rel.update_time";
        int rt = conn->execute("ALTER TABLE article_category_rel ADD COLUMN `update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article_category_rel ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column article_category_rel." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE article_category_rel DROP COLUMN `" + col + "`");
            if (rt) {
                ERROR(logger) << "ALTER TABLE article_category_rel DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}


} //namespace data
} //namespace blog
