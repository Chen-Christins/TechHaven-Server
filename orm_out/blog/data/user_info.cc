#include "user_info.h"
#include "log/log.h"

namespace blog {
namespace data {

static sylar::Logger::ptr logger = LOG_NAME("orm");

UserInfo::UserInfo()
    :m_state()
    ,m_isDeleted()
    ,m_id()
    ,m_account()
    ,m_email()
    ,m_passwd()
    ,m_name()
    ,m_code()
    ,m_createTime(time(0))
    ,m_updateTime(time(0)) {
}

std::string UserInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["account"] = m_account;
    v["email"] = m_email;
    v["passwd"] = m_passwd;
    v["name"] = m_name;
    v["code"] = m_code;
    v["state"] = m_state;
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = sylar::Time2Str(m_createTime);
    v["update_time"] = sylar::Time2Str(m_updateTime);
    return sylar::JsonUtil::ToString(v);
}

void UserInfo::setId(const int64_t& v) {
    m_id = v;
}

void UserInfo::setAccount(const std::string& v) {
    m_account = v;
}

void UserInfo::setEmail(const std::string& v) {
    m_email = v;
}

void UserInfo::setPasswd(const std::string& v) {
    m_passwd = v;
}

void UserInfo::setName(const std::string& v) {
    m_name = v;
}

void UserInfo::setCode(const std::string& v) {
    m_code = v;
}

void UserInfo::setState(const int32_t& v) {
    m_state = v;
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


int UserInfoDao::Update(UserInfo::ptr info, sylar::SQLite3::ptr conn) {
    std::string sql = "update user set account = ?, email = ?, passwd = ?, name = ?, code = ?, state = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_account);
    stmt->bindString(2, info->m_email);
    stmt->bindString(3, info->m_passwd);
    stmt->bindString(4, info->m_name);
    stmt->bindString(5, info->m_code);
    stmt->bindInt32(6, info->m_state);
    stmt->bindInt32(7, info->m_isDeleted);
    stmt->bindTime(8, info->m_createTime);
    stmt->bindTime(9, info->m_updateTime);
    stmt->bindInt64(10, info->m_id);
    return stmt->execute();
}

int UserInfoDao::Insert(UserInfo::ptr info, sylar::SQLite3::ptr conn) {
    std::string sql = "insert into user (account, email, passwd, name, code, state, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_account);
    stmt->bindString(2, info->m_email);
    stmt->bindString(3, info->m_passwd);
    stmt->bindString(4, info->m_name);
    stmt->bindString(5, info->m_code);
    stmt->bindInt32(6, info->m_state);
    stmt->bindInt32(7, info->m_isDeleted);
    stmt->bindTime(8, info->m_createTime);
    stmt->bindTime(9, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int UserInfoDao::InsertOrUpdate(UserInfo::ptr info, sylar::SQLite3::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into user (id, account, email, passwd, name, code, state, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindString(2, info->m_account);
    stmt->bindString(3, info->m_email);
    stmt->bindString(4, info->m_passwd);
    stmt->bindString(5, info->m_name);
    stmt->bindString(6, info->m_code);
    stmt->bindInt32(7, info->m_state);
    stmt->bindInt32(8, info->m_isDeleted);
    stmt->bindTime(9, info->m_createTime);
    stmt->bindTime(10, info->m_updateTime);
    return stmt->execute();
}

int UserInfoDao::Delete(UserInfo::ptr info, sylar::SQLite3::ptr conn) {
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

int UserInfoDao::DeleteById( const int64_t& id, sylar::SQLite3::ptr conn) {
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

int UserInfoDao::DeleteByAccount( const std::string& account, sylar::SQLite3::ptr conn) {
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

int UserInfoDao::DeleteByEmail( const std::string& email, sylar::SQLite3::ptr conn) {
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

int UserInfoDao::DeleteByName( const std::string& name, sylar::SQLite3::ptr conn) {
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

int UserInfoDao::QueryAll(std::vector<UserInfo::ptr>& results, sylar::SQLite3::ptr conn) {
    std::string sql = "select id, account, email, passwd, name, code, state, is_deleted, create_time, update_time from user";
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
        v->m_account = rt->getString(1);
        v->m_email = rt->getString(2);
        v->m_passwd = rt->getString(3);
        v->m_name = rt->getString(4);
        v->m_code = rt->getString(5);
        v->m_state = rt->getInt32(6);
        v->m_isDeleted = rt->getInt32(7);
        v->m_createTime = rt->getTime(8);
        v->m_updateTime = rt->getTime(9);
        results.push_back(v);
    }
    return 0;
}

UserInfo::ptr UserInfoDao::Query( const int64_t& id, sylar::SQLite3::ptr conn) {
    std::string sql = "select id, account, email, passwd, name, code, state, is_deleted, create_time, update_time from user where id = ?";
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
    v->m_account = rt->getString(1);
    v->m_email = rt->getString(2);
    v->m_passwd = rt->getString(3);
    v->m_name = rt->getString(4);
    v->m_code = rt->getString(5);
    v->m_state = rt->getInt32(6);
    v->m_isDeleted = rt->getInt32(7);
    v->m_createTime = rt->getTime(8);
    v->m_updateTime = rt->getTime(9);
    return v;
}

UserInfo::ptr UserInfoDao::QueryByAccount( const std::string& account, sylar::SQLite3::ptr conn) {
    std::string sql = "select id, account, email, passwd, name, code, state, is_deleted, create_time, update_time from user where account = ?";
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
    v->m_account = rt->getString(1);
    v->m_email = rt->getString(2);
    v->m_passwd = rt->getString(3);
    v->m_name = rt->getString(4);
    v->m_code = rt->getString(5);
    v->m_state = rt->getInt32(6);
    v->m_isDeleted = rt->getInt32(7);
    v->m_createTime = rt->getTime(8);
    v->m_updateTime = rt->getTime(9);
    return v;
}

UserInfo::ptr UserInfoDao::QueryByEmail( const std::string& email, sylar::SQLite3::ptr conn) {
    std::string sql = "select id, account, email, passwd, name, code, state, is_deleted, create_time, update_time from user where email = ?";
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
    v->m_account = rt->getString(1);
    v->m_email = rt->getString(2);
    v->m_passwd = rt->getString(3);
    v->m_name = rt->getString(4);
    v->m_code = rt->getString(5);
    v->m_state = rt->getInt32(6);
    v->m_isDeleted = rt->getInt32(7);
    v->m_createTime = rt->getTime(8);
    v->m_updateTime = rt->getTime(9);
    return v;
}

UserInfo::ptr UserInfoDao::QueryByName( const std::string& name, sylar::SQLite3::ptr conn) {
    std::string sql = "select id, account, email, passwd, name, code, state, is_deleted, create_time, update_time from user where name = ?";
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
    v->m_account = rt->getString(1);
    v->m_email = rt->getString(2);
    v->m_passwd = rt->getString(3);
    v->m_name = rt->getString(4);
    v->m_code = rt->getString(5);
    v->m_state = rt->getInt32(6);
    v->m_isDeleted = rt->getInt32(7);
    v->m_createTime = rt->getTime(8);
    v->m_updateTime = rt->getTime(9);
    return v;
}

int UserInfoDao::CreateTableSQLite3(sylar::SQLite3::ptr conn) {
    return conn->execute("CREATE TABLE user("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "account TEXT NOT NULL DEFAULT '',"
            "email TEXT NOT NULL DEFAULT '',"
            "passwd TEXT NOT NULL DEFAULT '',"
            "name TEXT NOT NULL DEFAULT '',"
            "code TEXT NOT NULL DEFAULT '',"
            "state INTEGER NOT NULL DEFAULT 0,"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "update_time TIMESTAMP NOT NULL DEFAULT current_timestamp);"
            "CREATE UNIQUE INDEX user_account ON user(account);"
            "CREATE UNIQUE INDEX user_email ON user(email);"
            "CREATE UNIQUE INDEX user_name ON user(name);"
            );
}

int UserInfoDao::CreateTableMySQL(sylar::SQLite3::ptr conn) {
    return conn->execute("CREATE TABLE user("
            "`id` bigint AUTO_INCREMENT,"
            "`account` varchar(128) NOT NULL DEFAULT '',"
            "`email` varchar(128) NOT NULL DEFAULT '',"
            "`passwd` varchar(128) NOT NULL DEFAULT '',"
            "`name` varchar(128) NOT NULL DEFAULT '',"
            "`code` varchar(128) NOT NULL DEFAULT '',"
            "`state` int NOT NULL DEFAULT 0,"
            "`is_deleted` int NOT NULL DEFAULT 0,"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp,"
            "`update_time` timestamp NOT NULL DEFAULT current_timestamp,"
            "PRIMARY KEY(`id`),"
            "UNIQUE KEY `user_account` (`account`),"
            "UNIQUE KEY `user_email` (`email`),"
            "UNIQUE KEY `user_name` (`name`))");
}
} //namespace data
} //namespace blog
