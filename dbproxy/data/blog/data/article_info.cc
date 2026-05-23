#include "article_info.h"
#include "chen/log/log.h"
#include <set>

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

ArticleInfo::ArticleInfo()
    :m_type()
    ,m_state()
    ,m_isDeleted()
    ,m_id()
    ,m_userId()
    ,m_channel()
    ,m_weight()
    ,m_views()
    ,m_praise()
    ,m_favorites()
    ,m_title()
    ,m_content()
    ,m_publishTime()
    ,m_createTime()
    ,m_updateTime() {
}

std::string ArticleInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["user_id"] = std::to_string(m_userId);
    v["title"] = m_title;
    v["content"] = m_content;
    v["type"] = m_type;
    v["state"] = m_state;
    v["channel"] = std::to_string(m_channel);
    v["is_deleted"] = m_isDeleted;
    v["publish_time"] = chen::Time2Str(m_publishTime);
    v["weight"] = std::to_string(m_weight);
    v["views"] = std::to_string(m_views);
    v["praise"] = std::to_string(m_praise);
    v["favorites"] = std::to_string(m_favorites);
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void ArticleInfo::setId(const int64_t& v) {
    m_id = v;
}

void ArticleInfo::setUserId(const int64_t& v) {
    m_userId = v;
}

void ArticleInfo::setTitle(const std::string& v) {
    m_title = v;
}

void ArticleInfo::setContent(const std::string& v) {
    m_content = v;
}

void ArticleInfo::setType(const int32_t& v) {
    m_type = v;
}

void ArticleInfo::setState(const int32_t& v) {
    m_state = v;
}

void ArticleInfo::setChannel(const int64_t& v) {
    m_channel = v;
}

void ArticleInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void ArticleInfo::setPublishTime(const int64_t& v) {
    m_publishTime = v;
}

void ArticleInfo::setWeight(const int64_t& v) {
    m_weight = v;
}

void ArticleInfo::setViews(const int64_t& v) {
    m_views = v;
}

void ArticleInfo::setPraise(const int64_t& v) {
    m_praise = v;
}

void ArticleInfo::setFavorites(const int64_t& v) {
    m_favorites = v;
}

void ArticleInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void ArticleInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int ArticleInfoDao::Update(ArticleInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update article set user_id = ?, title = ?, content = ?, type = ?, state = ?, channel = ?, is_deleted = ?, publish_time = ?, weight = ?, views = ?, praise = ?, favorites = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_userId);
    stmt->bindString(2, info->m_title);
    stmt->bindString(3, info->m_content);
    stmt->bindInt32(4, info->m_type);
    stmt->bindInt32(5, info->m_state);
    stmt->bindInt64(6, info->m_channel);
    stmt->bindInt32(7, info->m_isDeleted);
    stmt->bindTime(8, info->m_publishTime);
    stmt->bindInt64(9, info->m_weight);
    stmt->bindInt64(10, info->m_views);
    stmt->bindInt64(11, info->m_praise);
    stmt->bindInt64(12, info->m_favorites);
    stmt->bindTime(13, info->m_createTime);
    stmt->bindTime(14, info->m_updateTime);
    stmt->bindInt64(15, info->m_id);
    return stmt->execute();
}

int ArticleInfoDao::Insert(ArticleInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into article (user_id, title, content, type, state, channel, is_deleted, publish_time, weight, views, praise, favorites, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_userId);
    stmt->bindString(2, info->m_title);
    stmt->bindString(3, info->m_content);
    stmt->bindInt32(4, info->m_type);
    stmt->bindInt32(5, info->m_state);
    stmt->bindInt64(6, info->m_channel);
    stmt->bindInt32(7, info->m_isDeleted);
    stmt->bindTime(8, info->m_publishTime);
    stmt->bindInt64(9, info->m_weight);
    stmt->bindInt64(10, info->m_views);
    stmt->bindInt64(11, info->m_praise);
    stmt->bindInt64(12, info->m_favorites);
    stmt->bindTime(13, info->m_createTime);
    stmt->bindTime(14, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int ArticleInfoDao::InsertOrUpdate(ArticleInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into article (id, user_id, title, content, type, state, channel, is_deleted, publish_time, weight, views, praise, favorites, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_userId);
    stmt->bindString(3, info->m_title);
    stmt->bindString(4, info->m_content);
    stmt->bindInt32(5, info->m_type);
    stmt->bindInt32(6, info->m_state);
    stmt->bindInt64(7, info->m_channel);
    stmt->bindInt32(8, info->m_isDeleted);
    stmt->bindTime(9, info->m_publishTime);
    stmt->bindInt64(10, info->m_weight);
    stmt->bindInt64(11, info->m_views);
    stmt->bindInt64(12, info->m_praise);
    stmt->bindInt64(13, info->m_favorites);
    stmt->bindTime(14, info->m_createTime);
    stmt->bindTime(15, info->m_updateTime);
    return stmt->execute();
}

int ArticleInfoDao::Delete(ArticleInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from article where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int ArticleInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from article where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int ArticleInfoDao::DeleteByUserId( const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "delete from article where user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    return stmt->execute();
}

int ArticleInfoDao::QueryAll(std::vector<ArticleInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, title, content, type, state, channel, is_deleted, publish_time, weight, views, praise, favorites, create_time, update_time from article";
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
        ArticleInfo::ptr v(new ArticleInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_title = rt->getString(2);
        v->m_content = rt->getString(3);
        v->m_type = rt->getInt32(4);
        v->m_state = rt->getInt32(5);
        v->m_channel = rt->getInt64(6);
        v->m_isDeleted = rt->getInt32(7);
        v->m_publishTime = rt->getTime(8);
        v->m_weight = rt->getInt64(9);
        v->m_views = rt->getInt64(10);
        v->m_praise = rt->getInt64(11);
        v->m_favorites = rt->getInt64(12);
        v->m_createTime = rt->getTime(13);
        v->m_updateTime = rt->getTime(14);
        results.push_back(v);
    }
    return 0;
}

ArticleInfo::ptr ArticleInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, title, content, type, state, channel, is_deleted, publish_time, weight, views, praise, favorites, create_time, update_time from article where id = ?";
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
    ArticleInfo::ptr v(new ArticleInfo);
    v->m_id = rt->getInt64(0);
    v->m_userId = rt->getInt64(1);
    v->m_title = rt->getString(2);
    v->m_content = rt->getString(3);
    v->m_type = rt->getInt32(4);
    v->m_state = rt->getInt32(5);
    v->m_channel = rt->getInt64(6);
    v->m_isDeleted = rt->getInt32(7);
    v->m_publishTime = rt->getTime(8);
    v->m_weight = rt->getInt64(9);
    v->m_views = rt->getInt64(10);
    v->m_praise = rt->getInt64(11);
    v->m_favorites = rt->getInt64(12);
    v->m_createTime = rt->getTime(13);
    v->m_updateTime = rt->getTime(14);
    return v;
}

int ArticleInfoDao::QueryByUserId(std::vector<ArticleInfo::ptr>& results,  const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, title, content, type, state, channel, is_deleted, publish_time, weight, views, praise, favorites, create_time, update_time from article where user_id = ?";
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
        ArticleInfo::ptr v(new ArticleInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_title = rt->getString(2);
        v->m_content = rt->getString(3);
        v->m_type = rt->getInt32(4);
        v->m_state = rt->getInt32(5);
        v->m_channel = rt->getInt64(6);
        v->m_isDeleted = rt->getInt32(7);
        v->m_publishTime = rt->getTime(8);
        v->m_weight = rt->getInt64(9);
        v->m_views = rt->getInt64(10);
        v->m_praise = rt->getInt64(11);
        v->m_favorites = rt->getInt64(12);
        v->m_createTime = rt->getTime(13);
        v->m_updateTime = rt->getTime(14);
        results.push_back(v);
    };
    return 0;
}

int ArticleInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS article("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "user_id INTEGER NOT NULL DEFAULT 0,"
            "title TEXT NOT NULL DEFAULT '',"
            "content TEXT NOT NULL DEFAULT '',"
            "type INTEGER NOT NULL DEFAULT 0,"
            "state INTEGER NOT NULL DEFAULT 0,"
            "channel INTEGER NOT NULL DEFAULT 0,"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "publish_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "weight INTEGER NOT NULL DEFAULT 0,"
            "views INTEGER NOT NULL DEFAULT 0,"
            "praise INTEGER NOT NULL DEFAULT 0,"
            "favorites INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE INDEX IF NOT EXISTS article_user_id ON article(user_id);"
            );
}

int ArticleInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS article("
            "`id` bigint AUTO_INCREMENT COMMENT '文章id',"
            "`user_id` bigint NOT NULL DEFAULT 0 COMMENT '用户id',"
            "`title` varchar(256) NOT NULL DEFAULT '' COMMENT '文章标题',"
            "`content` text NOT NULL DEFAULT '' COMMENT '文章内容',"
            "`type` int NOT NULL DEFAULT 0 COMMENT '类型 1:原创,2:转发',"
            "`state` int NOT NULL DEFAULT 0 COMMENT '状态: 0全部 1审核中 2已发布 3未通过 4私密',"
            "`channel` bigint NOT NULL DEFAULT 0 COMMENT '频道id',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`publish_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '发布时间',"
            "`weight` bigint NOT NULL DEFAULT 0 COMMENT '权重',"
            "`views` bigint NOT NULL DEFAULT 0 COMMENT '流量数',"
            "`praise` bigint NOT NULL DEFAULT 0 COMMENT '点赞数',"
            "`favorites` bigint NOT NULL DEFAULT 0 COMMENT '收藏数',"
            "`create_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "KEY `article_user_id` (`user_id`)) COMMENT='博客文章'");
}

int ArticleInfoDao::MigrateTableSQLite3(chen::IDB::ptr conn) {
    auto data = conn->query("PRAGMA table_info(article)");
    if (!data) {
        ERROR(logger) << "PRAGMA table_info(article) errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(1));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("user_id");
    expected_cols.insert("title");
    expected_cols.insert("content");
    expected_cols.insert("type");
    expected_cols.insert("state");
    expected_cols.insert("channel");
    expected_cols.insert("is_deleted");
    expected_cols.insert("publish_time");
    expected_cols.insert("weight");
    expected_cols.insert("views");
    expected_cols.insert("praise");
    expected_cols.insert("favorites");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("user_id") == existing_cols.end()) {
        INFO(logger) << "Adding column article.user_id";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN user_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN user_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("title") == existing_cols.end()) {
        INFO(logger) << "Adding column article.title";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN title TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN title failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("content") == existing_cols.end()) {
        INFO(logger) << "Adding column article.content";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN content TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN content failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("type") == existing_cols.end()) {
        INFO(logger) << "Adding column article.type";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN type INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("state") == existing_cols.end()) {
        INFO(logger) << "Adding column article.state";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN state INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN state failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("channel") == existing_cols.end()) {
        INFO(logger) << "Adding column article.channel";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN channel INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN channel failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column article.is_deleted";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("publish_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article.publish_time";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN publish_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN publish_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("weight") == existing_cols.end()) {
        INFO(logger) << "Adding column article.weight";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN weight INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN weight failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("views") == existing_cols.end()) {
        INFO(logger) << "Adding column article.views";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN views INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN views failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("praise") == existing_cols.end()) {
        INFO(logger) << "Adding column article.praise";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN praise INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN praise failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("favorites") == existing_cols.end()) {
        INFO(logger) << "Adding column article.favorites";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN favorites INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN favorites failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article.create_time";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN create_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article.update_time";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column article." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE article DROP COLUMN " + col);
            if (rt) {
                ERROR(logger) << "ALTER TABLE article DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}

int ArticleInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM article");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM article errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(0));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("user_id");
    expected_cols.insert("title");
    expected_cols.insert("content");
    expected_cols.insert("type");
    expected_cols.insert("state");
    expected_cols.insert("channel");
    expected_cols.insert("is_deleted");
    expected_cols.insert("publish_time");
    expected_cols.insert("weight");
    expected_cols.insert("views");
    expected_cols.insert("praise");
    expected_cols.insert("favorites");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("user_id") == existing_cols.end()) {
        INFO(logger) << "Adding column article.user_id";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN `user_id` bigint NOT NULL DEFAULT 0 COMMENT '用户id'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN user_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("title") == existing_cols.end()) {
        INFO(logger) << "Adding column article.title";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN `title` varchar(256) NOT NULL DEFAULT '' COMMENT '文章标题'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN title failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("content") == existing_cols.end()) {
        INFO(logger) << "Adding column article.content";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN `content` text NOT NULL DEFAULT '' COMMENT '文章内容'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN content failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("type") == existing_cols.end()) {
        INFO(logger) << "Adding column article.type";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN `type` int NOT NULL DEFAULT 0 COMMENT '类型 1:原创,2:转发'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("state") == existing_cols.end()) {
        INFO(logger) << "Adding column article.state";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN `state` int NOT NULL DEFAULT 0 COMMENT '状态: 0全部 1审核中 2已发布 3未通过 4私密'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN state failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("channel") == existing_cols.end()) {
        INFO(logger) << "Adding column article.channel";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN `channel` bigint NOT NULL DEFAULT 0 COMMENT '频道id'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN channel failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column article.is_deleted";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN `is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("publish_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article.publish_time";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN `publish_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '发布时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN publish_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("weight") == existing_cols.end()) {
        INFO(logger) << "Adding column article.weight";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN `weight` bigint NOT NULL DEFAULT 0 COMMENT '权重'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN weight failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("views") == existing_cols.end()) {
        INFO(logger) << "Adding column article.views";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN `views` bigint NOT NULL DEFAULT 0 COMMENT '流量数'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN views failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("praise") == existing_cols.end()) {
        INFO(logger) << "Adding column article.praise";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN `praise` bigint NOT NULL DEFAULT 0 COMMENT '点赞数'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN praise failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("favorites") == existing_cols.end()) {
        INFO(logger) << "Adding column article.favorites";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN `favorites` bigint NOT NULL DEFAULT 0 COMMENT '收藏数'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN favorites failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article.create_time";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN `create_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '创建时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column article.update_time";
        int rt = conn->execute("ALTER TABLE article ADD COLUMN `update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '更新时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE article ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column article." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE article DROP COLUMN `" + col + "`");
            if (rt) {
                ERROR(logger) << "ALTER TABLE article DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}


} //namespace data
} //namespace blog
