#include "article_label_rel_info.h"
#include "chen/log/log.h"
#include <map>

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

int ArticleLabelRelInfoDao::QueryByArticleIdPages(std::vector<ArticleLabelRelInfo::ptr>& results, int64_t& total,  const int64_t& article_id, int32_t offset, int32_t limit, chen::IDB::ptr conn) {
    std::string countSql = "select count(*) from article_label_rel where article_id = ?";
    auto countStmt = conn->prepare(countSql);
    if (!countStmt) {
        ERROR(logger) << "stmt=" << countSql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    countStmt->bindInt64(1, article_id);
    auto countRt = countStmt->query();
    if (!countRt) {
        return countStmt->getErrno();
    }
    if (countRt->next()) {
        total = countRt->getInt64(0);
    }
    if (total == 0) {
        return 0;
    }
    std::string sql = "select id, article_id, label_id, is_deleted, create_time, update_time from article_label_rel where article_id = ? order by id desc limit ? offset ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, article_id);
    stmt->bindInt32(2, limit);
    stmt->bindInt32(3, offset);
    auto rt = stmt->query();
    if (!rt) {
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
    std::map<std::string, std::string> existing_cols;  // name -> type
    while (data->next()) {
        existing_cols[data->getString(1)] = data->getString(2);
    }

    bool need_recreate = false;
    {
        auto it = existing_cols.find("id");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: article_label_rel.id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("article_id");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: article_label_rel.article_id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("label_id");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: article_label_rel.label_id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("is_deleted");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: article_label_rel.is_deleted " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("create_time");
        if (it != existing_cols.end() && it->second != "TIMESTAMP") {
            INFO(logger) << "Column type changed: article_label_rel.create_time " << it->second << " -> TIMESTAMP";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("update_time");
        if (it != existing_cols.end() && it->second != "TIMESTAMP") {
            INFO(logger) << "Column type changed: article_label_rel.update_time " << it->second << " -> TIMESTAMP";
            need_recreate = true;
        }
    }
    if (!need_recreate) {
        for (auto& [name, _] : existing_cols) {
            (void)_;  // suppress unused warning
            bool found = false;
            if (name == "id") found = true;
            if (name == "article_id") found = true;
            if (name == "label_id") found = true;
            if (name == "is_deleted") found = true;
            if (name == "create_time") found = true;
            if (name == "update_time") found = true;
            if (!found) {
                need_recreate = true;
                WARN(logger) << "Column article_label_rel." << name << " removed, table recreate required";
                break;
            }
        }
    }

    if (need_recreate) {
        INFO(logger) << "Recreating table article_label_rel";

        std::vector<std::string> common_cols;
        if (existing_cols.find("id") != existing_cols.end()) {
            common_cols.push_back("id");
        }
        if (existing_cols.find("article_id") != existing_cols.end()) {
            common_cols.push_back("article_id");
        }
        if (existing_cols.find("label_id") != existing_cols.end()) {
            common_cols.push_back("label_id");
        }
        if (existing_cols.find("is_deleted") != existing_cols.end()) {
            common_cols.push_back("is_deleted");
        }
        if (existing_cols.find("create_time") != existing_cols.end()) {
            common_cols.push_back("create_time");
        }
        if (existing_cols.find("update_time") != existing_cols.end()) {
            common_cols.push_back("update_time");
        }

        if (conn->execute("ALTER TABLE article_label_rel RENAME TO article_label_rel_tmp")) {
            ERROR(logger) << "RENAME TABLE article_label_rel failed";
            return conn->getErrno();
        }
        CreateTableSQLite3(conn);
        if (!common_cols.empty()) {
            std::string cols;
            for (size_t i = 0; i < common_cols.size(); ++i) {
                if (i) cols += ",";
                cols += common_cols[i];
            }
            std::string sql = "INSERT INTO article_label_rel (" + cols + ") SELECT " + cols + " FROM article_label_rel_tmp";
            if (int rt = conn->execute(sql)) {
                ERROR(logger) << "copy data from article_label_rel_tmp to article_label_rel failed, errno=" << rt;
                // don't return; try to continue
            }
        }
        conn->execute("DROP TABLE article_label_rel_tmp");
        return 0;
    }

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

    return 0;
}

int ArticleLabelRelInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM article_label_rel");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM article_label_rel errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::map<std::string, std::string> existing_cols;  // name -> type
    while (data->next()) {
        existing_cols[data->getString(0)] = data->getString(1);
    }

    {
        auto it = existing_cols.find("id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column article_label_rel.id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE article_label_rel MODIFY COLUMN `id` bigint NOT NULL DEFAULT 0");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN article_label_rel.id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("article_id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column article_label_rel.article_id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE article_label_rel MODIFY COLUMN `article_id` bigint NOT NULL DEFAULT 0");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN article_label_rel.article_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("label_id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column article_label_rel.label_id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE article_label_rel MODIFY COLUMN `label_id` bigint NOT NULL DEFAULT 0");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN article_label_rel.label_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("is_deleted");
        if (it != existing_cols.end() && it->second != "int") {
            INFO(logger) << "Modifying column article_label_rel.is_deleted " << it->second << " -> int";
            int rt = conn->execute("ALTER TABLE article_label_rel MODIFY COLUMN `is_deleted` int NOT NULL DEFAULT 0");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN article_label_rel.is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("create_time");
        if (it != existing_cols.end() && it->second != "timestamp") {
            INFO(logger) << "Modifying column article_label_rel.create_time " << it->second << " -> timestamp";
            int rt = conn->execute("ALTER TABLE article_label_rel MODIFY COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN article_label_rel.create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("update_time");
        if (it != existing_cols.end() && it->second != "timestamp") {
            INFO(logger) << "Modifying column article_label_rel.update_time " << it->second << " -> timestamp";
            int rt = conn->execute("ALTER TABLE article_label_rel MODIFY COLUMN `update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN article_label_rel.update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    for (auto& [name, _] : existing_cols) {
        (void)_;
        bool found = false;
        if (name == "id") found = true;
        if (name == "article_id") found = true;
        if (name == "label_id") found = true;
        if (name == "is_deleted") found = true;
        if (name == "create_time") found = true;
        if (name == "update_time") found = true;
        if (!found) {
            WARN(logger) << "Dropping column article_label_rel." << name << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE article_label_rel DROP COLUMN `" + name + "`");
            if (rt) {
                ERROR(logger) << "DROP COLUMN article_label_rel." << name << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

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

    return 0;
}


} //namespace data
} //namespace blog
