#include "user_follow_rel_info.h"
#include "chen/log/log.h"
#include <set>

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

UserFollowRelInfo::UserFollowRelInfo()
    :m_isDeleted(0)
    ,m_id()
    ,m_followerId()
    ,m_followingId()
    ,m_createTime(time(0))
    ,m_updateTime() {
}

std::string UserFollowRelInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["follower_id"] = std::to_string(m_followerId);
    v["following_id"] = std::to_string(m_followingId);
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void UserFollowRelInfo::setId(const int64_t& v) {
    m_id = v;
}

void UserFollowRelInfo::setFollowerId(const int64_t& v) {
    m_followerId = v;
}

void UserFollowRelInfo::setFollowingId(const int64_t& v) {
    m_followingId = v;
}

void UserFollowRelInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void UserFollowRelInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void UserFollowRelInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int UserFollowRelInfoDao::Update(UserFollowRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update user_follow_rel set follower_id = ?, following_id = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_followerId);
    stmt->bindInt64(2, info->m_followingId);
    stmt->bindInt32(3, info->m_isDeleted);
    stmt->bindTime(4, info->m_createTime);
    stmt->bindTime(5, info->m_updateTime);
    stmt->bindInt64(6, info->m_id);
    return stmt->execute();
}

int UserFollowRelInfoDao::Insert(UserFollowRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into user_follow_rel (follower_id, following_id, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_followerId);
    stmt->bindInt64(2, info->m_followingId);
    stmt->bindInt32(3, info->m_isDeleted);
    stmt->bindTime(4, info->m_createTime);
    stmt->bindTime(5, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int UserFollowRelInfoDao::InsertOrUpdate(UserFollowRelInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into user_follow_rel (id, follower_id, following_id, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_followerId);
    stmt->bindInt64(3, info->m_followingId);
    stmt->bindInt32(4, info->m_isDeleted);
    stmt->bindTime(5, info->m_createTime);
    stmt->bindTime(6, info->m_updateTime);
    return stmt->execute();
}

int UserFollowRelInfoDao::Delete(UserFollowRelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from user_follow_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int UserFollowRelInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from user_follow_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int UserFollowRelInfoDao::DeleteByFollowerIdFollowingId( const int64_t& follower_id,  const int64_t& following_id, chen::IDB::ptr conn) {
    std::string sql = "delete from user_follow_rel where follower_id = ? and following_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, follower_id);
    stmt->bindInt64(1, following_id);
    return stmt->execute();
}

int UserFollowRelInfoDao::DeleteByFollowerId( const int64_t& follower_id, chen::IDB::ptr conn) {
    std::string sql = "delete from user_follow_rel where follower_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, follower_id);
    return stmt->execute();
}

int UserFollowRelInfoDao::DeleteByFollowingId( const int64_t& following_id, chen::IDB::ptr conn) {
    std::string sql = "delete from user_follow_rel where following_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, following_id);
    return stmt->execute();
}

int UserFollowRelInfoDao::QueryAll(std::vector<UserFollowRelInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, follower_id, following_id, is_deleted, create_time, update_time from user_follow_rel";
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
        UserFollowRelInfo::ptr v(new UserFollowRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_followerId = rt->getInt64(1);
        v->m_followingId = rt->getInt64(2);
        v->m_isDeleted = rt->getInt32(3);
        v->m_createTime = rt->getTime(4);
        v->m_updateTime = rt->getTime(5);
        results.push_back(v);
    }
    return 0;
}

UserFollowRelInfo::ptr UserFollowRelInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, follower_id, following_id, is_deleted, create_time, update_time from user_follow_rel where id = ?";
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
    UserFollowRelInfo::ptr v(new UserFollowRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_followerId = rt->getInt64(1);
    v->m_followingId = rt->getInt64(2);
    v->m_isDeleted = rt->getInt32(3);
    v->m_createTime = rt->getTime(4);
    v->m_updateTime = rt->getTime(5);
    return v;
}

UserFollowRelInfo::ptr UserFollowRelInfoDao::QueryByFollowerIdFollowingId( const int64_t& follower_id,  const int64_t& following_id, chen::IDB::ptr conn) {
    std::string sql = "select id, follower_id, following_id, is_deleted, create_time, update_time from user_follow_rel where follower_id = ? and following_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindInt64(1, follower_id);
    stmt->bindInt64(2, following_id);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    UserFollowRelInfo::ptr v(new UserFollowRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_followerId = rt->getInt64(1);
    v->m_followingId = rt->getInt64(2);
    v->m_isDeleted = rt->getInt32(3);
    v->m_createTime = rt->getTime(4);
    v->m_updateTime = rt->getTime(5);
    return v;
}

int UserFollowRelInfoDao::QueryByFollowerId(std::vector<UserFollowRelInfo::ptr>& results,  const int64_t& follower_id, chen::IDB::ptr conn) {
    std::string sql = "select id, follower_id, following_id, is_deleted, create_time, update_time from user_follow_rel where follower_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, follower_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        UserFollowRelInfo::ptr v(new UserFollowRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_followerId = rt->getInt64(1);
        v->m_followingId = rt->getInt64(2);
        v->m_isDeleted = rt->getInt32(3);
        v->m_createTime = rt->getTime(4);
        v->m_updateTime = rt->getTime(5);
        results.push_back(v);
    };
    return 0;
}

int UserFollowRelInfoDao::QueryByFollowingId(std::vector<UserFollowRelInfo::ptr>& results,  const int64_t& following_id, chen::IDB::ptr conn) {
    std::string sql = "select id, follower_id, following_id, is_deleted, create_time, update_time from user_follow_rel where following_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, following_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        UserFollowRelInfo::ptr v(new UserFollowRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_followerId = rt->getInt64(1);
        v->m_followingId = rt->getInt64(2);
        v->m_isDeleted = rt->getInt32(3);
        v->m_createTime = rt->getTime(4);
        v->m_updateTime = rt->getTime(5);
        results.push_back(v);
    };
    return 0;
}

int UserFollowRelInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS user_follow_rel("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "follower_id INTEGER NOT NULL DEFAULT 0,"
            "following_id INTEGER NOT NULL DEFAULT 0,"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE UNIQUE INDEX IF NOT EXISTS user_follow_rel_follower_id_following_id ON user_follow_rel(follower_id,following_id);"
            "CREATE INDEX IF NOT EXISTS user_follow_rel_follower_id ON user_follow_rel(follower_id);"
            "CREATE INDEX IF NOT EXISTS user_follow_rel_following_id ON user_follow_rel(following_id);"
            );
}

int UserFollowRelInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS user_follow_rel("
            "`id` bigint AUTO_INCREMENT COMMENT '主键ID',"
            "`follower_id` bigint NOT NULL DEFAULT 0 COMMENT '关注者用户ID',"
            "`following_id` bigint NOT NULL DEFAULT 0 COMMENT '被关注者用户ID',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '关注时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "UNIQUE KEY `user_follow_rel_follower_id_following_id` (`follower_id`,`following_id`),"
            "KEY `user_follow_rel_follower_id` (`follower_id`),"
            "KEY `user_follow_rel_following_id` (`following_id`)) COMMENT='用户关注关联表'");
}

int UserFollowRelInfoDao::MigrateTableSQLite3(chen::IDB::ptr conn) {
    auto data = conn->query("PRAGMA table_info(user_follow_rel)");
    if (!data) {
        ERROR(logger) << "PRAGMA table_info(user_follow_rel) errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(1));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("follower_id");
    expected_cols.insert("following_id");
    expected_cols.insert("is_deleted");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("follower_id") == existing_cols.end()) {
        INFO(logger) << "Adding column user_follow_rel.follower_id";
        int rt = conn->execute("ALTER TABLE user_follow_rel ADD COLUMN follower_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user_follow_rel ADD COLUMN follower_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("following_id") == existing_cols.end()) {
        INFO(logger) << "Adding column user_follow_rel.following_id";
        int rt = conn->execute("ALTER TABLE user_follow_rel ADD COLUMN following_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user_follow_rel ADD COLUMN following_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column user_follow_rel.is_deleted";
        int rt = conn->execute("ALTER TABLE user_follow_rel ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user_follow_rel ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column user_follow_rel.create_time";
        int rt = conn->execute("ALTER TABLE user_follow_rel ADD COLUMN create_time TIMESTAMP NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user_follow_rel ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column user_follow_rel.update_time";
        int rt = conn->execute("ALTER TABLE user_follow_rel ADD COLUMN update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user_follow_rel ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column user_follow_rel." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE user_follow_rel DROP COLUMN " + col);
            if (rt) {
                ERROR(logger) << "ALTER TABLE user_follow_rel DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}

int UserFollowRelInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM user_follow_rel");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM user_follow_rel errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(0));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("follower_id");
    expected_cols.insert("following_id");
    expected_cols.insert("is_deleted");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("follower_id") == existing_cols.end()) {
        INFO(logger) << "Adding column user_follow_rel.follower_id";
        int rt = conn->execute("ALTER TABLE user_follow_rel ADD COLUMN `follower_id` bigint NOT NULL DEFAULT 0 COMMENT '关注者用户ID'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user_follow_rel ADD COLUMN follower_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("following_id") == existing_cols.end()) {
        INFO(logger) << "Adding column user_follow_rel.following_id";
        int rt = conn->execute("ALTER TABLE user_follow_rel ADD COLUMN `following_id` bigint NOT NULL DEFAULT 0 COMMENT '被关注者用户ID'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user_follow_rel ADD COLUMN following_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column user_follow_rel.is_deleted";
        int rt = conn->execute("ALTER TABLE user_follow_rel ADD COLUMN `is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user_follow_rel ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column user_follow_rel.create_time";
        int rt = conn->execute("ALTER TABLE user_follow_rel ADD COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '关注时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user_follow_rel ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column user_follow_rel.update_time";
        int rt = conn->execute("ALTER TABLE user_follow_rel ADD COLUMN `update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '更新时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user_follow_rel ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column user_follow_rel." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE user_follow_rel DROP COLUMN `" + col + "`");
            if (rt) {
                ERROR(logger) << "ALTER TABLE user_follow_rel DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}


} //namespace data
} //namespace blog
