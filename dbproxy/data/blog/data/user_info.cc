#include "user_info.h"
#include "chen/log/log.h"
#include <set>

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

UserInfo::UserInfo()
    :m_role(1)
    ,m_state()
    ,m_isDeleted()
    ,m_id()
    ,m_tokenTime()
    ,m_name()
    ,m_account()
    ,m_avatar()
    ,m_email()
    ,m_passwd()
    ,m_bio()
    ,m_website()
    ,m_location()
    ,m_token()
    ,m_loginTime()
    ,m_createTime(time(0))
    ,m_updateTime(time(0)) {
}

std::string UserInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["name"] = m_name;
    v["account"] = m_account;
    v["avatar"] = m_avatar;
    v["email"] = m_email;
    v["role"] = m_role;
    v["passwd"] = m_passwd;
    v["state"] = m_state;
    v["bio"] = m_bio;
    v["website"] = m_website;
    v["location"] = m_location;
    v["token"] = m_token;
    v["token_time"] = std::to_string(m_tokenTime);
    v["login_time"] = chen::Time2Str(m_loginTime);
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void UserInfo::setId(const int64_t& v) {
    m_id = v;
}

void UserInfo::setName(const std::string& v) {
    m_name = v;
}

void UserInfo::setAccount(const std::string& v) {
    m_account = v;
}

void UserInfo::setAvatar(const std::string& v) {
    m_avatar = v;
}

void UserInfo::setEmail(const std::string& v) {
    m_email = v;
}

void UserInfo::setRole(const int32_t& v) {
    m_role = v;
}

void UserInfo::setPasswd(const std::string& v) {
    m_passwd = v;
}

void UserInfo::setState(const int32_t& v) {
    m_state = v;
}

void UserInfo::setBio(const std::string& v) {
    m_bio = v;
}

void UserInfo::setWebsite(const std::string& v) {
    m_website = v;
}

void UserInfo::setLocation(const std::string& v) {
    m_location = v;
}

void UserInfo::setToken(const std::string& v) {
    m_token = v;
}

void UserInfo::setTokenTime(const int64_t& v) {
    m_tokenTime = v;
}

void UserInfo::setLoginTime(const int64_t& v) {
    m_loginTime = v;
}

void UserInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void UserInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void UserInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int UserInfoDao::Update(UserInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update user set name = ?, account = ?, avatar = ?, email = ?, role = ?, passwd = ?, state = ?, bio = ?, website = ?, location = ?, token = ?, token_time = ?, login_time = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_name);
    stmt->bindString(2, info->m_account);
    stmt->bindString(3, info->m_avatar);
    stmt->bindString(4, info->m_email);
    stmt->bindInt32(5, info->m_role);
    stmt->bindString(6, info->m_passwd);
    stmt->bindInt32(7, info->m_state);
    stmt->bindString(8, info->m_bio);
    stmt->bindString(9, info->m_website);
    stmt->bindString(10, info->m_location);
    stmt->bindString(11, info->m_token);
    stmt->bindInt64(12, info->m_tokenTime);
    stmt->bindTime(13, info->m_loginTime);
    stmt->bindInt32(14, info->m_isDeleted);
    stmt->bindTime(15, info->m_createTime);
    stmt->bindTime(16, info->m_updateTime);
    stmt->bindInt64(17, info->m_id);
    return stmt->execute();
}

int UserInfoDao::Insert(UserInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into user (name, account, avatar, email, role, passwd, state, bio, website, location, token, token_time, login_time, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_name);
    stmt->bindString(2, info->m_account);
    stmt->bindString(3, info->m_avatar);
    stmt->bindString(4, info->m_email);
    stmt->bindInt32(5, info->m_role);
    stmt->bindString(6, info->m_passwd);
    stmt->bindInt32(7, info->m_state);
    stmt->bindString(8, info->m_bio);
    stmt->bindString(9, info->m_website);
    stmt->bindString(10, info->m_location);
    stmt->bindString(11, info->m_token);
    stmt->bindInt64(12, info->m_tokenTime);
    stmt->bindTime(13, info->m_loginTime);
    stmt->bindInt32(14, info->m_isDeleted);
    stmt->bindTime(15, info->m_createTime);
    stmt->bindTime(16, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int UserInfoDao::InsertOrUpdate(UserInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into user (id, name, account, avatar, email, role, passwd, state, bio, website, location, token, token_time, login_time, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindString(2, info->m_name);
    stmt->bindString(3, info->m_account);
    stmt->bindString(4, info->m_avatar);
    stmt->bindString(5, info->m_email);
    stmt->bindInt32(6, info->m_role);
    stmt->bindString(7, info->m_passwd);
    stmt->bindInt32(8, info->m_state);
    stmt->bindString(9, info->m_bio);
    stmt->bindString(10, info->m_website);
    stmt->bindString(11, info->m_location);
    stmt->bindString(12, info->m_token);
    stmt->bindInt64(13, info->m_tokenTime);
    stmt->bindTime(14, info->m_loginTime);
    stmt->bindInt32(15, info->m_isDeleted);
    stmt->bindTime(16, info->m_createTime);
    stmt->bindTime(17, info->m_updateTime);
    return stmt->execute();
}

int UserInfoDao::Delete(UserInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from user where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int UserInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from user where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int UserInfoDao::DeleteByAccount( const std::string& account, chen::IDB::ptr conn) {
    std::string sql = "delete from user where account = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, account);
    return stmt->execute();
}

int UserInfoDao::DeleteByEmail( const std::string& email, chen::IDB::ptr conn) {
    std::string sql = "delete from user where email = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, email);
    return stmt->execute();
}

int UserInfoDao::DeleteByName( const std::string& name, chen::IDB::ptr conn) {
    std::string sql = "delete from user where name = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, name);
    return stmt->execute();
}

int UserInfoDao::QueryAll(std::vector<UserInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, name, account, avatar, email, role, passwd, state, bio, website, location, token, token_time, login_time, is_deleted, create_time, update_time from user";
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
        UserInfo::ptr v(new UserInfo);
        v->m_id = rt->getInt64(0);
        v->m_name = rt->getString(1);
        v->m_account = rt->getString(2);
        v->m_avatar = rt->getString(3);
        v->m_email = rt->getString(4);
        v->m_role = rt->getInt32(5);
        v->m_passwd = rt->getString(6);
        v->m_state = rt->getInt32(7);
        v->m_bio = rt->getString(8);
        v->m_website = rt->getString(9);
        v->m_location = rt->getString(10);
        v->m_token = rt->getString(11);
        v->m_tokenTime = rt->getInt64(12);
        v->m_loginTime = rt->getTime(13);
        v->m_isDeleted = rt->getInt32(14);
        v->m_createTime = rt->getTime(15);
        v->m_updateTime = rt->getTime(16);
        results.push_back(v);
    }
    return 0;
}

UserInfo::ptr UserInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, name, account, avatar, email, role, passwd, state, bio, website, location, token, token_time, login_time, is_deleted, create_time, update_time from user where id = ?";
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
    UserInfo::ptr v(new UserInfo);
    v->m_id = rt->getInt64(0);
    v->m_name = rt->getString(1);
    v->m_account = rt->getString(2);
    v->m_avatar = rt->getString(3);
    v->m_email = rt->getString(4);
    v->m_role = rt->getInt32(5);
    v->m_passwd = rt->getString(6);
    v->m_state = rt->getInt32(7);
    v->m_bio = rt->getString(8);
    v->m_website = rt->getString(9);
    v->m_location = rt->getString(10);
    v->m_token = rt->getString(11);
    v->m_tokenTime = rt->getInt64(12);
    v->m_loginTime = rt->getTime(13);
    v->m_isDeleted = rt->getInt32(14);
    v->m_createTime = rt->getTime(15);
    v->m_updateTime = rt->getTime(16);
    return v;
}

UserInfo::ptr UserInfoDao::QueryByAccount( const std::string& account, chen::IDB::ptr conn) {
    std::string sql = "select id, name, account, avatar, email, role, passwd, state, bio, website, location, token, token_time, login_time, is_deleted, create_time, update_time from user where account = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindString(1, account);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    UserInfo::ptr v(new UserInfo);
    v->m_id = rt->getInt64(0);
    v->m_name = rt->getString(1);
    v->m_account = rt->getString(2);
    v->m_avatar = rt->getString(3);
    v->m_email = rt->getString(4);
    v->m_role = rt->getInt32(5);
    v->m_passwd = rt->getString(6);
    v->m_state = rt->getInt32(7);
    v->m_bio = rt->getString(8);
    v->m_website = rt->getString(9);
    v->m_location = rt->getString(10);
    v->m_token = rt->getString(11);
    v->m_tokenTime = rt->getInt64(12);
    v->m_loginTime = rt->getTime(13);
    v->m_isDeleted = rt->getInt32(14);
    v->m_createTime = rt->getTime(15);
    v->m_updateTime = rt->getTime(16);
    return v;
}

UserInfo::ptr UserInfoDao::QueryByEmail( const std::string& email, chen::IDB::ptr conn) {
    std::string sql = "select id, name, account, avatar, email, role, passwd, state, bio, website, location, token, token_time, login_time, is_deleted, create_time, update_time from user where email = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindString(1, email);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    UserInfo::ptr v(new UserInfo);
    v->m_id = rt->getInt64(0);
    v->m_name = rt->getString(1);
    v->m_account = rt->getString(2);
    v->m_avatar = rt->getString(3);
    v->m_email = rt->getString(4);
    v->m_role = rt->getInt32(5);
    v->m_passwd = rt->getString(6);
    v->m_state = rt->getInt32(7);
    v->m_bio = rt->getString(8);
    v->m_website = rt->getString(9);
    v->m_location = rt->getString(10);
    v->m_token = rt->getString(11);
    v->m_tokenTime = rt->getInt64(12);
    v->m_loginTime = rt->getTime(13);
    v->m_isDeleted = rt->getInt32(14);
    v->m_createTime = rt->getTime(15);
    v->m_updateTime = rt->getTime(16);
    return v;
}

UserInfo::ptr UserInfoDao::QueryByName( const std::string& name, chen::IDB::ptr conn) {
    std::string sql = "select id, name, account, avatar, email, role, passwd, state, bio, website, location, token, token_time, login_time, is_deleted, create_time, update_time from user where name = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindString(1, name);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    UserInfo::ptr v(new UserInfo);
    v->m_id = rt->getInt64(0);
    v->m_name = rt->getString(1);
    v->m_account = rt->getString(2);
    v->m_avatar = rt->getString(3);
    v->m_email = rt->getString(4);
    v->m_role = rt->getInt32(5);
    v->m_passwd = rt->getString(6);
    v->m_state = rt->getInt32(7);
    v->m_bio = rt->getString(8);
    v->m_website = rt->getString(9);
    v->m_location = rt->getString(10);
    v->m_token = rt->getString(11);
    v->m_tokenTime = rt->getInt64(12);
    v->m_loginTime = rt->getTime(13);
    v->m_isDeleted = rt->getInt32(14);
    v->m_createTime = rt->getTime(15);
    v->m_updateTime = rt->getTime(16);
    return v;
}

int UserInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS user("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL DEFAULT '',"
            "account TEXT NOT NULL DEFAULT '',"
            "avatar TEXT NOT NULL DEFAULT '',"
            "email TEXT NOT NULL DEFAULT '',"
            "role INTEGER NOT NULL DEFAULT 1,"
            "passwd TEXT NOT NULL DEFAULT '',"
            "state INTEGER NOT NULL DEFAULT 0,"
            "bio TEXT NOT NULL DEFAULT '',"
            "website TEXT NOT NULL DEFAULT '',"
            "location TEXT NOT NULL DEFAULT '',"
            "token TEXT NOT NULL DEFAULT '',"
            "token_time INTEGER NOT NULL DEFAULT 0,"
            "login_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "update_time TIMESTAMP NOT NULL DEFAULT current_timestamp);"
            "CREATE UNIQUE INDEX IF NOT EXISTS user_account ON user(account);"
            "CREATE UNIQUE INDEX IF NOT EXISTS user_email ON user(email);"
            "CREATE UNIQUE INDEX IF NOT EXISTS user_name ON user(name);"
            );
}

int UserInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS user("
            "`id` bigint AUTO_INCREMENT COMMENT '主键id',"
            "`name` varchar(128) NOT NULL DEFAULT '' COMMENT '用户名',"
            "`account` varchar(128) NOT NULL DEFAULT '' COMMENT '账户名称',"
            "`avatar` varchar(128) NOT NULL DEFAULT '' COMMENT '头像地址',"
            "`email` varchar(128) NOT NULL DEFAULT '' COMMENT '邮箱地址',"
            "`role` int NOT NULL DEFAULT 1 COMMENT '角色: 1用户, 2管理员, 3编辑, 4审核员',"
            "`passwd` varchar(128) NOT NULL DEFAULT '' COMMENT '用户密码',"
            "`state` int NOT NULL DEFAULT 0 COMMENT '账号状态',"
            "`bio` varchar(128) NOT NULL DEFAULT '' COMMENT '用户简介',"
            "`website` varchar(128) NOT NULL DEFAULT '' COMMENT '个人网站',"
            "`location` varchar(128) NOT NULL DEFAULT '' COMMENT '用户所在地',"
            "`token` varchar(64) NOT NULL DEFAULT '' COMMENT '登录凭证',"
            "`token_time` bigint NOT NULL DEFAULT 0 COMMENT '凭证过期时间',"
            "`login_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '上次登录时间',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '账号是否已经删除',"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '账号创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '账号信息上一次更新时间',"
            "PRIMARY KEY(`id`),"
            "UNIQUE KEY `user_account` (`account`),"
            "UNIQUE KEY `user_email` (`email`),"
            "UNIQUE KEY `user_name` (`name`))");
}

int UserInfoDao::MigrateTableSQLite3(chen::IDB::ptr conn) {
    auto data = conn->query("PRAGMA table_info(user)");
    if (!data) {
        ERROR(logger) << "PRAGMA table_info(user) errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(1));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("name");
    expected_cols.insert("account");
    expected_cols.insert("avatar");
    expected_cols.insert("email");
    expected_cols.insert("role");
    expected_cols.insert("passwd");
    expected_cols.insert("state");
    expected_cols.insert("bio");
    expected_cols.insert("website");
    expected_cols.insert("location");
    expected_cols.insert("token");
    expected_cols.insert("token_time");
    expected_cols.insert("login_time");
    expected_cols.insert("is_deleted");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("name") == existing_cols.end()) {
        INFO(logger) << "Adding column user.name";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN name TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN name failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("account") == existing_cols.end()) {
        INFO(logger) << "Adding column user.account";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN account TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN account failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("avatar") == existing_cols.end()) {
        INFO(logger) << "Adding column user.avatar";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN avatar TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN avatar failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("email") == existing_cols.end()) {
        INFO(logger) << "Adding column user.email";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN email TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN email failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("role") == existing_cols.end()) {
        INFO(logger) << "Adding column user.role";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN role INTEGER NOT NULL DEFAULT 1");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN role failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("passwd") == existing_cols.end()) {
        INFO(logger) << "Adding column user.passwd";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN passwd TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN passwd failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("state") == existing_cols.end()) {
        INFO(logger) << "Adding column user.state";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN state INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN state failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("bio") == existing_cols.end()) {
        INFO(logger) << "Adding column user.bio";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN bio TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN bio failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("website") == existing_cols.end()) {
        INFO(logger) << "Adding column user.website";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN website TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN website failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("location") == existing_cols.end()) {
        INFO(logger) << "Adding column user.location";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN location TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN location failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("token") == existing_cols.end()) {
        INFO(logger) << "Adding column user.token";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN token TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN token failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("token_time") == existing_cols.end()) {
        INFO(logger) << "Adding column user.token_time";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN token_time INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN token_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("login_time") == existing_cols.end()) {
        INFO(logger) << "Adding column user.login_time";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN login_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN login_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column user.is_deleted";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column user.create_time";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN create_time TIMESTAMP NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column user.update_time";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN update_time TIMESTAMP NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column user." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE user DROP COLUMN " + col);
            if (rt) {
                ERROR(logger) << "ALTER TABLE user DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}

int UserInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM user");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM user errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(0));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("name");
    expected_cols.insert("account");
    expected_cols.insert("avatar");
    expected_cols.insert("email");
    expected_cols.insert("role");
    expected_cols.insert("passwd");
    expected_cols.insert("state");
    expected_cols.insert("bio");
    expected_cols.insert("website");
    expected_cols.insert("location");
    expected_cols.insert("token");
    expected_cols.insert("token_time");
    expected_cols.insert("login_time");
    expected_cols.insert("is_deleted");
    expected_cols.insert("create_time");
    expected_cols.insert("update_time");

    if (existing_cols.find("name") == existing_cols.end()) {
        INFO(logger) << "Adding column user.name";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN `name` varchar(128) NOT NULL DEFAULT '' COMMENT '用户名'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN name failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("account") == existing_cols.end()) {
        INFO(logger) << "Adding column user.account";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN `account` varchar(128) NOT NULL DEFAULT '' COMMENT '账户名称'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN account failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("avatar") == existing_cols.end()) {
        INFO(logger) << "Adding column user.avatar";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN `avatar` varchar(128) NOT NULL DEFAULT '' COMMENT '头像地址'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN avatar failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("email") == existing_cols.end()) {
        INFO(logger) << "Adding column user.email";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN `email` varchar(128) NOT NULL DEFAULT '' COMMENT '邮箱地址'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN email failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("role") == existing_cols.end()) {
        INFO(logger) << "Adding column user.role";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN `role` int NOT NULL DEFAULT 1 COMMENT '角色: 1用户, 2管理员, 3编辑, 4审核员'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN role failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("passwd") == existing_cols.end()) {
        INFO(logger) << "Adding column user.passwd";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN `passwd` varchar(128) NOT NULL DEFAULT '' COMMENT '用户密码'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN passwd failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("state") == existing_cols.end()) {
        INFO(logger) << "Adding column user.state";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN `state` int NOT NULL DEFAULT 0 COMMENT '账号状态'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN state failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("bio") == existing_cols.end()) {
        INFO(logger) << "Adding column user.bio";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN `bio` varchar(128) NOT NULL DEFAULT '' COMMENT '用户简介'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN bio failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("website") == existing_cols.end()) {
        INFO(logger) << "Adding column user.website";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN `website` varchar(128) NOT NULL DEFAULT '' COMMENT '个人网站'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN website failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("location") == existing_cols.end()) {
        INFO(logger) << "Adding column user.location";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN `location` varchar(128) NOT NULL DEFAULT '' COMMENT '用户所在地'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN location failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("token") == existing_cols.end()) {
        INFO(logger) << "Adding column user.token";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN `token` varchar(64) NOT NULL DEFAULT '' COMMENT '登录凭证'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN token failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("token_time") == existing_cols.end()) {
        INFO(logger) << "Adding column user.token_time";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN `token_time` bigint NOT NULL DEFAULT 0 COMMENT '凭证过期时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN token_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("login_time") == existing_cols.end()) {
        INFO(logger) << "Adding column user.login_time";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN `login_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '上次登录时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN login_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column user.is_deleted";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN `is_deleted` int NOT NULL DEFAULT 0 COMMENT '账号是否已经删除'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column user.create_time";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '账号创建时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column user.update_time";
        int rt = conn->execute("ALTER TABLE user ADD COLUMN `update_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '账号信息上一次更新时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE user ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column user." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE user DROP COLUMN `" + col + "`");
            if (rt) {
                ERROR(logger) << "ALTER TABLE user DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}


} //namespace data
} //namespace blog
