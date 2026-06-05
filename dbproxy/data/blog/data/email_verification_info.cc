#include "email_verification_info.h"
#include "chen/log/log.h"
#include <map>

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

EmailVerificationInfo::EmailVerificationInfo()
    :m_type()
    ,m_state()
    ,m_id()
    ,m_email()
    ,m_code()
    ,m_clientIp()
    ,m_userAgent()
    ,m_createTime(time(0))
    ,m_expiresTime() {
}

std::string EmailVerificationInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["email"] = m_email;
    v["code"] = m_code;
    v["type"] = m_type;
    v["state"] = m_state;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["expires_time"] = chen::Time2Str(m_expiresTime);
    v["client_ip"] = m_clientIp;
    v["user_agent"] = m_userAgent;
    return chen::JsonUtil::ToString(v);
}

void EmailVerificationInfo::setId(const int64_t& v) {
    m_id = v;
}

void EmailVerificationInfo::setEmail(const std::string& v) {
    m_email = v;
}

void EmailVerificationInfo::setCode(const std::string& v) {
    m_code = v;
}

void EmailVerificationInfo::setType(const int32_t& v) {
    m_type = v;
}

void EmailVerificationInfo::setState(const int32_t& v) {
    m_state = v;
}

void EmailVerificationInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void EmailVerificationInfo::setExpiresTime(const int64_t& v) {
    m_expiresTime = v;
}

void EmailVerificationInfo::setClientIp(const std::string& v) {
    m_clientIp = v;
}

void EmailVerificationInfo::setUserAgent(const std::string& v) {
    m_userAgent = v;
}


int EmailVerificationInfoDao::Update(EmailVerificationInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update email_verification set email = ?, code = ?, type = ?, state = ?, create_time = ?, expires_time = ?, client_ip = ?, user_agent = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_email);
    stmt->bindString(2, info->m_code);
    stmt->bindInt32(3, info->m_type);
    stmt->bindInt32(4, info->m_state);
    stmt->bindTime(5, info->m_createTime);
    stmt->bindTime(6, info->m_expiresTime);
    stmt->bindString(7, info->m_clientIp);
    stmt->bindString(8, info->m_userAgent);
    stmt->bindInt64(9, info->m_id);
    return stmt->execute();
}

int EmailVerificationInfoDao::Insert(EmailVerificationInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into email_verification (email, code, type, state, create_time, expires_time, client_ip, user_agent) values (?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_email);
    stmt->bindString(2, info->m_code);
    stmt->bindInt32(3, info->m_type);
    stmt->bindInt32(4, info->m_state);
    stmt->bindTime(5, info->m_createTime);
    stmt->bindTime(6, info->m_expiresTime);
    stmt->bindString(7, info->m_clientIp);
    stmt->bindString(8, info->m_userAgent);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int EmailVerificationInfoDao::InsertOrUpdate(EmailVerificationInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into email_verification (id, email, code, type, state, create_time, expires_time, client_ip, user_agent) values (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindString(2, info->m_email);
    stmt->bindString(3, info->m_code);
    stmt->bindInt32(4, info->m_type);
    stmt->bindInt32(5, info->m_state);
    stmt->bindTime(6, info->m_createTime);
    stmt->bindTime(7, info->m_expiresTime);
    stmt->bindString(8, info->m_clientIp);
    stmt->bindString(9, info->m_userAgent);
    return stmt->execute();
}

int EmailVerificationInfoDao::Delete(EmailVerificationInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from email_verification where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int EmailVerificationInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from email_verification where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int EmailVerificationInfoDao::DeleteByEmailCode( const std::string& email,  const std::string& code, chen::IDB::ptr conn) {
    std::string sql = "delete from email_verification where email = ? and code = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, email);
    stmt->bindString(1, code);
    return stmt->execute();
}

int EmailVerificationInfoDao::DeleteByEmailType( const std::string& email,  const int32_t& type, chen::IDB::ptr conn) {
    std::string sql = "delete from email_verification where email = ? and type = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, email);
    stmt->bindInt32(1, type);
    return stmt->execute();
}

int EmailVerificationInfoDao::DeleteByExpiresTime( const int64_t& expires_time, chen::IDB::ptr conn) {
    std::string sql = "delete from email_verification where expires_time = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindTime(1, expires_time);
    return stmt->execute();
}

int EmailVerificationInfoDao::DeleteByCreateTime( const int64_t& create_time, chen::IDB::ptr conn) {
    std::string sql = "delete from email_verification where create_time = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindTime(1, create_time);
    return stmt->execute();
}

int EmailVerificationInfoDao::QueryAll(std::vector<EmailVerificationInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, email, code, type, state, create_time, expires_time, client_ip, user_agent from email_verification";
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
        EmailVerificationInfo::ptr v(new EmailVerificationInfo);
        v->m_id = rt->getInt64(0);
        v->m_email = rt->getString(1);
        v->m_code = rt->getString(2);
        v->m_type = rt->getInt32(3);
        v->m_state = rt->getInt32(4);
        v->m_createTime = rt->getTime(5);
        v->m_expiresTime = rt->getTime(6);
        v->m_clientIp = rt->getString(7);
        v->m_userAgent = rt->getString(8);
        results.push_back(v);
    }
    return 0;
}

EmailVerificationInfo::ptr EmailVerificationInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, email, code, type, state, create_time, expires_time, client_ip, user_agent from email_verification where id = ?";
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
    EmailVerificationInfo::ptr v(new EmailVerificationInfo);
    v->m_id = rt->getInt64(0);
    v->m_email = rt->getString(1);
    v->m_code = rt->getString(2);
    v->m_type = rt->getInt32(3);
    v->m_state = rt->getInt32(4);
    v->m_createTime = rt->getTime(5);
    v->m_expiresTime = rt->getTime(6);
    v->m_clientIp = rt->getString(7);
    v->m_userAgent = rt->getString(8);
    return v;
}

int EmailVerificationInfoDao::QueryByEmailCode(std::vector<EmailVerificationInfo::ptr>& results,  const std::string& email,  const std::string& code, chen::IDB::ptr conn) {
    std::string sql = "select id, email, code, type, state, create_time, expires_time, client_ip, user_agent from email_verification where email = ? and code = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, email);
    stmt->bindString(2, code);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        EmailVerificationInfo::ptr v(new EmailVerificationInfo);
        v->m_id = rt->getInt64(0);
        v->m_email = rt->getString(1);
        v->m_code = rt->getString(2);
        v->m_type = rt->getInt32(3);
        v->m_state = rt->getInt32(4);
        v->m_createTime = rt->getTime(5);
        v->m_expiresTime = rt->getTime(6);
        v->m_clientIp = rt->getString(7);
        v->m_userAgent = rt->getString(8);
        results.push_back(v);
    };
    return 0;
}

int EmailVerificationInfoDao::QueryByEmailCodePages(std::vector<EmailVerificationInfo::ptr>& results, int64_t& total,  const std::string& email,  const std::string& code, int32_t offset, int32_t limit, chen::IDB::ptr conn) {
    std::string countSql = "select count(*) from email_verification where email = ? and code = ?";
    auto countStmt = conn->prepare(countSql);
    if (!countStmt) {
        ERROR(logger) << "stmt=" << countSql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    countStmt->bindString(1, email);
    countStmt->bindString(2, code);
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
    std::string sql = "select id, email, code, type, state, create_time, expires_time, client_ip, user_agent from email_verification where email = ? and code = ? order by id desc limit ? offset ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, email);
    stmt->bindString(2, code);
    stmt->bindInt32(3, limit);
    stmt->bindInt32(4, offset);
    auto rt = stmt->query();
    if (!rt) {
        return 0;
    }
    while (rt->next()) {
        EmailVerificationInfo::ptr v(new EmailVerificationInfo);
        v->m_id = rt->getInt64(0);
        v->m_email = rt->getString(1);
        v->m_code = rt->getString(2);
        v->m_type = rt->getInt32(3);
        v->m_state = rt->getInt32(4);
        v->m_createTime = rt->getTime(5);
        v->m_expiresTime = rt->getTime(6);
        v->m_clientIp = rt->getString(7);
        v->m_userAgent = rt->getString(8);
        results.push_back(v);
    };
    return 0;
}

int EmailVerificationInfoDao::QueryByEmailType(std::vector<EmailVerificationInfo::ptr>& results,  const std::string& email,  const int32_t& type, chen::IDB::ptr conn) {
    std::string sql = "select id, email, code, type, state, create_time, expires_time, client_ip, user_agent from email_verification where email = ? and type = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, email);
    stmt->bindInt32(2, type);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        EmailVerificationInfo::ptr v(new EmailVerificationInfo);
        v->m_id = rt->getInt64(0);
        v->m_email = rt->getString(1);
        v->m_code = rt->getString(2);
        v->m_type = rt->getInt32(3);
        v->m_state = rt->getInt32(4);
        v->m_createTime = rt->getTime(5);
        v->m_expiresTime = rt->getTime(6);
        v->m_clientIp = rt->getString(7);
        v->m_userAgent = rt->getString(8);
        results.push_back(v);
    };
    return 0;
}

int EmailVerificationInfoDao::QueryByEmailTypePages(std::vector<EmailVerificationInfo::ptr>& results, int64_t& total,  const std::string& email,  const int32_t& type, int32_t offset, int32_t limit, chen::IDB::ptr conn) {
    std::string countSql = "select count(*) from email_verification where email = ? and type = ?";
    auto countStmt = conn->prepare(countSql);
    if (!countStmt) {
        ERROR(logger) << "stmt=" << countSql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    countStmt->bindString(1, email);
    countStmt->bindInt32(2, type);
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
    std::string sql = "select id, email, code, type, state, create_time, expires_time, client_ip, user_agent from email_verification where email = ? and type = ? order by id desc limit ? offset ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, email);
    stmt->bindInt32(2, type);
    stmt->bindInt32(3, limit);
    stmt->bindInt32(4, offset);
    auto rt = stmt->query();
    if (!rt) {
        return 0;
    }
    while (rt->next()) {
        EmailVerificationInfo::ptr v(new EmailVerificationInfo);
        v->m_id = rt->getInt64(0);
        v->m_email = rt->getString(1);
        v->m_code = rt->getString(2);
        v->m_type = rt->getInt32(3);
        v->m_state = rt->getInt32(4);
        v->m_createTime = rt->getTime(5);
        v->m_expiresTime = rt->getTime(6);
        v->m_clientIp = rt->getString(7);
        v->m_userAgent = rt->getString(8);
        results.push_back(v);
    };
    return 0;
}

int EmailVerificationInfoDao::QueryByExpiresTime(std::vector<EmailVerificationInfo::ptr>& results,  const int64_t& expires_time, chen::IDB::ptr conn) {
    std::string sql = "select id, email, code, type, state, create_time, expires_time, client_ip, user_agent from email_verification where expires_time = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindTime(1, expires_time);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        EmailVerificationInfo::ptr v(new EmailVerificationInfo);
        v->m_id = rt->getInt64(0);
        v->m_email = rt->getString(1);
        v->m_code = rt->getString(2);
        v->m_type = rt->getInt32(3);
        v->m_state = rt->getInt32(4);
        v->m_createTime = rt->getTime(5);
        v->m_expiresTime = rt->getTime(6);
        v->m_clientIp = rt->getString(7);
        v->m_userAgent = rt->getString(8);
        results.push_back(v);
    };
    return 0;
}

int EmailVerificationInfoDao::QueryByExpiresTimePages(std::vector<EmailVerificationInfo::ptr>& results, int64_t& total,  const int64_t& expires_time, int32_t offset, int32_t limit, chen::IDB::ptr conn) {
    std::string countSql = "select count(*) from email_verification where expires_time = ?";
    auto countStmt = conn->prepare(countSql);
    if (!countStmt) {
        ERROR(logger) << "stmt=" << countSql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    countStmt->bindTime(1, expires_time);
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
    std::string sql = "select id, email, code, type, state, create_time, expires_time, client_ip, user_agent from email_verification where expires_time = ? order by id desc limit ? offset ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindTime(1, expires_time);
    stmt->bindInt32(2, limit);
    stmt->bindInt32(3, offset);
    auto rt = stmt->query();
    if (!rt) {
        return 0;
    }
    while (rt->next()) {
        EmailVerificationInfo::ptr v(new EmailVerificationInfo);
        v->m_id = rt->getInt64(0);
        v->m_email = rt->getString(1);
        v->m_code = rt->getString(2);
        v->m_type = rt->getInt32(3);
        v->m_state = rt->getInt32(4);
        v->m_createTime = rt->getTime(5);
        v->m_expiresTime = rt->getTime(6);
        v->m_clientIp = rt->getString(7);
        v->m_userAgent = rt->getString(8);
        results.push_back(v);
    };
    return 0;
}

int EmailVerificationInfoDao::QueryByCreateTime(std::vector<EmailVerificationInfo::ptr>& results,  const int64_t& create_time, chen::IDB::ptr conn) {
    std::string sql = "select id, email, code, type, state, create_time, expires_time, client_ip, user_agent from email_verification where create_time = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindTime(1, create_time);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        EmailVerificationInfo::ptr v(new EmailVerificationInfo);
        v->m_id = rt->getInt64(0);
        v->m_email = rt->getString(1);
        v->m_code = rt->getString(2);
        v->m_type = rt->getInt32(3);
        v->m_state = rt->getInt32(4);
        v->m_createTime = rt->getTime(5);
        v->m_expiresTime = rt->getTime(6);
        v->m_clientIp = rt->getString(7);
        v->m_userAgent = rt->getString(8);
        results.push_back(v);
    };
    return 0;
}

int EmailVerificationInfoDao::QueryByCreateTimePages(std::vector<EmailVerificationInfo::ptr>& results, int64_t& total,  const int64_t& create_time, int32_t offset, int32_t limit, chen::IDB::ptr conn) {
    std::string countSql = "select count(*) from email_verification where create_time = ?";
    auto countStmt = conn->prepare(countSql);
    if (!countStmt) {
        ERROR(logger) << "stmt=" << countSql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    countStmt->bindTime(1, create_time);
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
    std::string sql = "select id, email, code, type, state, create_time, expires_time, client_ip, user_agent from email_verification where create_time = ? order by id desc limit ? offset ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindTime(1, create_time);
    stmt->bindInt32(2, limit);
    stmt->bindInt32(3, offset);
    auto rt = stmt->query();
    if (!rt) {
        return 0;
    }
    while (rt->next()) {
        EmailVerificationInfo::ptr v(new EmailVerificationInfo);
        v->m_id = rt->getInt64(0);
        v->m_email = rt->getString(1);
        v->m_code = rt->getString(2);
        v->m_type = rt->getInt32(3);
        v->m_state = rt->getInt32(4);
        v->m_createTime = rt->getTime(5);
        v->m_expiresTime = rt->getTime(6);
        v->m_clientIp = rt->getString(7);
        v->m_userAgent = rt->getString(8);
        results.push_back(v);
    };
    return 0;
}

int EmailVerificationInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS email_verification("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "email TEXT NOT NULL DEFAULT '',"
            "code TEXT NOT NULL DEFAULT '',"
            "type INTEGER NOT NULL DEFAULT 0,"
            "state INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "expires_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "client_ip TEXT NOT NULL DEFAULT '',"
            "user_agent TEXT NOT NULL DEFAULT '');"
            "CREATE INDEX IF NOT EXISTS email_verification_email_code ON email_verification(email,code);"
            "CREATE INDEX IF NOT EXISTS email_verification_email_type ON email_verification(email,type);"
            "CREATE INDEX IF NOT EXISTS email_verification_expires_time ON email_verification(expires_time);"
            "CREATE INDEX IF NOT EXISTS email_verification_create_time ON email_verification(create_time);"
            );
}

int EmailVerificationInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS email_verification("
            "`id` bigint AUTO_INCREMENT COMMENT '主键id',"
            "`email` varchar(128) NOT NULL DEFAULT '' COMMENT '用户邮箱地址',"
            "`code` varchar(128) NOT NULL DEFAULT '' COMMENT '验证码',"
            "`type` int NOT NULL DEFAULT 0 COMMENT '验证类型: 1-注册, 2-登录, 3-密码重置, 4-更换邮箱',"
            "`state` int NOT NULL DEFAULT 0 COMMENT '是否已使用',"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间',"
            "`expires_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '过期时间',"
            "`client_ip` varchar(128) NOT NULL DEFAULT '' COMMENT '请求IP地址',"
            "`user_agent` varchar(128) NOT NULL DEFAULT '' COMMENT '用户代理信息',"
            "PRIMARY KEY(`id`),"
            "KEY `email_verification_email_code` (`email`,`code`),"
            "KEY `email_verification_email_type` (`email`,`type`),"
            "KEY `email_verification_expires_time` (`expires_time`),"
            "KEY `email_verification_create_time` (`create_time`))");
}

int EmailVerificationInfoDao::MigrateTableSQLite3(chen::IDB::ptr conn) {
    auto data = conn->query("PRAGMA table_info(email_verification)");
    if (!data) {
        ERROR(logger) << "PRAGMA table_info(email_verification) errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
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
            INFO(logger) << "Column type changed: email_verification.id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("email");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: email_verification.email " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("code");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: email_verification.code " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("type");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: email_verification.type " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("state");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: email_verification.state " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("create_time");
        if (it != existing_cols.end() && it->second != "TIMESTAMP") {
            INFO(logger) << "Column type changed: email_verification.create_time " << it->second << " -> TIMESTAMP";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("expires_time");
        if (it != existing_cols.end() && it->second != "TIMESTAMP") {
            INFO(logger) << "Column type changed: email_verification.expires_time " << it->second << " -> TIMESTAMP";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("client_ip");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: email_verification.client_ip " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("user_agent");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: email_verification.user_agent " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    if (!need_recreate) {
        for (auto& [name, _] : existing_cols) {
            (void)_;  // suppress unused warning
            bool found = false;
            if (name == "id") found = true;
            if (name == "email") found = true;
            if (name == "code") found = true;
            if (name == "type") found = true;
            if (name == "state") found = true;
            if (name == "create_time") found = true;
            if (name == "expires_time") found = true;
            if (name == "client_ip") found = true;
            if (name == "user_agent") found = true;
            if (!found) {
                need_recreate = true;
                WARN(logger) << "Column email_verification." << name << " removed, table recreate required";
                break;
            }
        }
    }

    if (need_recreate) {
        INFO(logger) << "Recreating table email_verification";

        std::vector<std::string> common_cols;
        if (existing_cols.find("id") != existing_cols.end()) {
            common_cols.push_back("id");
        }
        if (existing_cols.find("email") != existing_cols.end()) {
            common_cols.push_back("email");
        }
        if (existing_cols.find("code") != existing_cols.end()) {
            common_cols.push_back("code");
        }
        if (existing_cols.find("type") != existing_cols.end()) {
            common_cols.push_back("type");
        }
        if (existing_cols.find("state") != existing_cols.end()) {
            common_cols.push_back("state");
        }
        if (existing_cols.find("create_time") != existing_cols.end()) {
            common_cols.push_back("create_time");
        }
        if (existing_cols.find("expires_time") != existing_cols.end()) {
            common_cols.push_back("expires_time");
        }
        if (existing_cols.find("client_ip") != existing_cols.end()) {
            common_cols.push_back("client_ip");
        }
        if (existing_cols.find("user_agent") != existing_cols.end()) {
            common_cols.push_back("user_agent");
        }

        if (conn->execute("ALTER TABLE email_verification RENAME TO email_verification_tmp")) {
            ERROR(logger) << "RENAME TABLE email_verification failed";
            return conn->getErrno();
        }
        CreateTableSQLite3(conn);
        if (!common_cols.empty()) {
            std::string cols;
            for (size_t i = 0; i < common_cols.size(); ++i) {
                if (i) cols += ",";
                cols += common_cols[i];
            }
            std::string sql = "INSERT INTO email_verification (" + cols + ") SELECT " + cols + " FROM email_verification_tmp";
            if (int rt = conn->execute(sql)) {
                ERROR(logger) << "copy data from email_verification_tmp to email_verification failed, errno=" << rt;
                // don't return; try to continue
            }
        }
        conn->execute("DROP TABLE email_verification_tmp");
        return 0;
    }

    if (existing_cols.find("email") == existing_cols.end()) {
        INFO(logger) << "Adding column email_verification.email";
        int rt = conn->execute("ALTER TABLE email_verification ADD COLUMN email TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE email_verification ADD COLUMN email failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("code") == existing_cols.end()) {
        INFO(logger) << "Adding column email_verification.code";
        int rt = conn->execute("ALTER TABLE email_verification ADD COLUMN code TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE email_verification ADD COLUMN code failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("type") == existing_cols.end()) {
        INFO(logger) << "Adding column email_verification.type";
        int rt = conn->execute("ALTER TABLE email_verification ADD COLUMN type INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE email_verification ADD COLUMN type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("state") == existing_cols.end()) {
        INFO(logger) << "Adding column email_verification.state";
        int rt = conn->execute("ALTER TABLE email_verification ADD COLUMN state INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE email_verification ADD COLUMN state failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column email_verification.create_time";
        int rt = conn->execute("ALTER TABLE email_verification ADD COLUMN create_time TIMESTAMP NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE email_verification ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("expires_time") == existing_cols.end()) {
        INFO(logger) << "Adding column email_verification.expires_time";
        int rt = conn->execute("ALTER TABLE email_verification ADD COLUMN expires_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE email_verification ADD COLUMN expires_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("client_ip") == existing_cols.end()) {
        INFO(logger) << "Adding column email_verification.client_ip";
        int rt = conn->execute("ALTER TABLE email_verification ADD COLUMN client_ip TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE email_verification ADD COLUMN client_ip failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("user_agent") == existing_cols.end()) {
        INFO(logger) << "Adding column email_verification.user_agent";
        int rt = conn->execute("ALTER TABLE email_verification ADD COLUMN user_agent TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE email_verification ADD COLUMN user_agent failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    return 0;
}

int EmailVerificationInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM email_verification");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM email_verification errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::map<std::string, std::string> existing_cols;  // name -> type
    while (data->next()) {
        existing_cols[data->getString(0)] = data->getString(1);
    }

    {
        auto it = existing_cols.find("id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column email_verification.id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE email_verification MODIFY COLUMN `id` bigint NOT NULL DEFAULT 0 COMMENT '主键id'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN email_verification.id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("email");
        if (it != existing_cols.end() && it->second != "varchar(128)") {
            INFO(logger) << "Modifying column email_verification.email " << it->second << " -> varchar(128)";
            int rt = conn->execute("ALTER TABLE email_verification MODIFY COLUMN `email` varchar(128) NOT NULL DEFAULT '' COMMENT '用户邮箱地址'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN email_verification.email failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("code");
        if (it != existing_cols.end() && it->second != "varchar(128)") {
            INFO(logger) << "Modifying column email_verification.code " << it->second << " -> varchar(128)";
            int rt = conn->execute("ALTER TABLE email_verification MODIFY COLUMN `code` varchar(128) NOT NULL DEFAULT '' COMMENT '验证码'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN email_verification.code failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("type");
        if (it != existing_cols.end() && it->second != "int") {
            INFO(logger) << "Modifying column email_verification.type " << it->second << " -> int";
            int rt = conn->execute("ALTER TABLE email_verification MODIFY COLUMN `type` int NOT NULL DEFAULT 0 COMMENT '验证类型: 1-注册, 2-登录, 3-密码重置, 4-更换邮箱'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN email_verification.type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("state");
        if (it != existing_cols.end() && it->second != "int") {
            INFO(logger) << "Modifying column email_verification.state " << it->second << " -> int";
            int rt = conn->execute("ALTER TABLE email_verification MODIFY COLUMN `state` int NOT NULL DEFAULT 0 COMMENT '是否已使用'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN email_verification.state failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("create_time");
        if (it != existing_cols.end() && it->second != "timestamp") {
            INFO(logger) << "Modifying column email_verification.create_time " << it->second << " -> timestamp";
            int rt = conn->execute("ALTER TABLE email_verification MODIFY COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN email_verification.create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("expires_time");
        if (it != existing_cols.end() && it->second != "timestamp") {
            INFO(logger) << "Modifying column email_verification.expires_time " << it->second << " -> timestamp";
            int rt = conn->execute("ALTER TABLE email_verification MODIFY COLUMN `expires_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '过期时间'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN email_verification.expires_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("client_ip");
        if (it != existing_cols.end() && it->second != "varchar(128)") {
            INFO(logger) << "Modifying column email_verification.client_ip " << it->second << " -> varchar(128)";
            int rt = conn->execute("ALTER TABLE email_verification MODIFY COLUMN `client_ip` varchar(128) NOT NULL DEFAULT '' COMMENT '请求IP地址'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN email_verification.client_ip failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("user_agent");
        if (it != existing_cols.end() && it->second != "varchar(128)") {
            INFO(logger) << "Modifying column email_verification.user_agent " << it->second << " -> varchar(128)";
            int rt = conn->execute("ALTER TABLE email_verification MODIFY COLUMN `user_agent` varchar(128) NOT NULL DEFAULT '' COMMENT '用户代理信息'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN email_verification.user_agent failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    for (auto& [name, _] : existing_cols) {
        (void)_;
        bool found = false;
        if (name == "id") found = true;
        if (name == "email") found = true;
        if (name == "code") found = true;
        if (name == "type") found = true;
        if (name == "state") found = true;
        if (name == "create_time") found = true;
        if (name == "expires_time") found = true;
        if (name == "client_ip") found = true;
        if (name == "user_agent") found = true;
        if (!found) {
            WARN(logger) << "Dropping column email_verification." << name << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE email_verification DROP COLUMN `" + name + "`");
            if (rt) {
                ERROR(logger) << "DROP COLUMN email_verification." << name << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    if (existing_cols.find("email") == existing_cols.end()) {
        INFO(logger) << "Adding column email_verification.email";
        int rt = conn->execute("ALTER TABLE email_verification ADD COLUMN `email` varchar(128) NOT NULL DEFAULT '' COMMENT '用户邮箱地址'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE email_verification ADD COLUMN email failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("code") == existing_cols.end()) {
        INFO(logger) << "Adding column email_verification.code";
        int rt = conn->execute("ALTER TABLE email_verification ADD COLUMN `code` varchar(128) NOT NULL DEFAULT '' COMMENT '验证码'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE email_verification ADD COLUMN code failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("type") == existing_cols.end()) {
        INFO(logger) << "Adding column email_verification.type";
        int rt = conn->execute("ALTER TABLE email_verification ADD COLUMN `type` int NOT NULL DEFAULT 0 COMMENT '验证类型: 1-注册, 2-登录, 3-密码重置, 4-更换邮箱'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE email_verification ADD COLUMN type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("state") == existing_cols.end()) {
        INFO(logger) << "Adding column email_verification.state";
        int rt = conn->execute("ALTER TABLE email_verification ADD COLUMN `state` int NOT NULL DEFAULT 0 COMMENT '是否已使用'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE email_verification ADD COLUMN state failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column email_verification.create_time";
        int rt = conn->execute("ALTER TABLE email_verification ADD COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE email_verification ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("expires_time") == existing_cols.end()) {
        INFO(logger) << "Adding column email_verification.expires_time";
        int rt = conn->execute("ALTER TABLE email_verification ADD COLUMN `expires_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '过期时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE email_verification ADD COLUMN expires_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("client_ip") == existing_cols.end()) {
        INFO(logger) << "Adding column email_verification.client_ip";
        int rt = conn->execute("ALTER TABLE email_verification ADD COLUMN `client_ip` varchar(128) NOT NULL DEFAULT '' COMMENT '请求IP地址'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE email_verification ADD COLUMN client_ip failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("user_agent") == existing_cols.end()) {
        INFO(logger) << "Adding column email_verification.user_agent";
        int rt = conn->execute("ALTER TABLE email_verification ADD COLUMN `user_agent` varchar(128) NOT NULL DEFAULT '' COMMENT '用户代理信息'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE email_verification ADD COLUMN user_agent failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    return 0;
}


} //namespace data
} //namespace blog
