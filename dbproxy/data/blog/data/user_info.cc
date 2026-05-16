#include "user_info.h"
#include "chen/log/log.h"

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
} //namespace data
} //namespace blog
