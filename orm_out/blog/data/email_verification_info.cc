#include "email_verification_info.h"
#include "chen/log/log.h"

namespace blog {
namespace data {

static sylar::Logger::ptr logger = LOG_NAME("orm");

EmailVerificationInfo::EmailVerificationInfo()
    :m_state()
    ,m_id()
    ,m_email()
    ,m_code()
    ,m_type()
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
    v["create_time"] = sylar::Time2Str(m_createTime);
    v["expires_time"] = sylar::Time2Str(m_expiresTime);
    v["client_ip"] = m_clientIp;
    v["user_agent"] = m_userAgent;
    return sylar::JsonUtil::ToString(v);
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

void EmailVerificationInfo::setType(const std::string& v) {
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


int EmailVerificationInfoDao::Update(EmailVerificationInfo::ptr info, sylar::IDB::ptr conn) {
    std::string sql = "update email_verification set email = ?, code = ?, type = ?, state = ?, create_time = ?, expires_time = ?, client_ip = ?, user_agent = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_email);
    stmt->bindString(2, info->m_code);
    stmt->bindString(3, info->m_type);
    stmt->bindInt32(4, info->m_state);
    stmt->bindTime(5, info->m_createTime);
    stmt->bindTime(6, info->m_expiresTime);
    stmt->bindString(7, info->m_clientIp);
    stmt->bindString(8, info->m_userAgent);
    stmt->bindInt64(9, info->m_id);
    return stmt->execute();
}

int EmailVerificationInfoDao::Insert(EmailVerificationInfo::ptr info, sylar::IDB::ptr conn) {
    std::string sql = "insert into email_verification (email, code, type, state, create_time, expires_time, client_ip, user_agent) values (?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_email);
    stmt->bindString(2, info->m_code);
    stmt->bindString(3, info->m_type);
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

int EmailVerificationInfoDao::InsertOrUpdate(EmailVerificationInfo::ptr info, sylar::IDB::ptr conn) {
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
    stmt->bindString(4, info->m_type);
    stmt->bindInt32(5, info->m_state);
    stmt->bindTime(6, info->m_createTime);
    stmt->bindTime(7, info->m_expiresTime);
    stmt->bindString(8, info->m_clientIp);
    stmt->bindString(9, info->m_userAgent);
    return stmt->execute();
}

int EmailVerificationInfoDao::Delete(EmailVerificationInfo::ptr info, sylar::IDB::ptr conn) {
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

int EmailVerificationInfoDao::DeleteById( const int64_t& id, sylar::IDB::ptr conn) {
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

int EmailVerificationInfoDao::DeleteByEmailCode( const std::string& email,  const std::string& code, sylar::IDB::ptr conn) {
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

int EmailVerificationInfoDao::DeleteByEmailType( const std::string& email,  const std::string& type, sylar::IDB::ptr conn) {
    std::string sql = "delete from email_verification where email = ? and type = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, email);
    stmt->bindString(1, type);
    return stmt->execute();
}

int EmailVerificationInfoDao::DeleteByExpiresTime( const int64_t& expires_time, sylar::IDB::ptr conn) {
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

int EmailVerificationInfoDao::DeleteByCreateTime( const int64_t& create_time, sylar::IDB::ptr conn) {
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

int EmailVerificationInfoDao::QueryAll(std::vector<EmailVerificationInfo::ptr>& results, sylar::IDB::ptr conn) {
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
        v->m_type = rt->getString(3);
        v->m_state = rt->getInt32(4);
        v->m_createTime = rt->getTime(5);
        v->m_expiresTime = rt->getTime(6);
        v->m_clientIp = rt->getString(7);
        v->m_userAgent = rt->getString(8);
        results.push_back(v);
    }
    return 0;
}

EmailVerificationInfo::ptr EmailVerificationInfoDao::Query( const int64_t& id, sylar::IDB::ptr conn) {
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
    v->m_type = rt->getString(3);
    v->m_state = rt->getInt32(4);
    v->m_createTime = rt->getTime(5);
    v->m_expiresTime = rt->getTime(6);
    v->m_clientIp = rt->getString(7);
    v->m_userAgent = rt->getString(8);
    return v;
}

int EmailVerificationInfoDao::QueryByEmailCode(std::vector<EmailVerificationInfo::ptr>& results,  const std::string& email,  const std::string& code, sylar::IDB::ptr conn) {
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
        v->m_type = rt->getString(3);
        v->m_state = rt->getInt32(4);
        v->m_createTime = rt->getTime(5);
        v->m_expiresTime = rt->getTime(6);
        v->m_clientIp = rt->getString(7);
        v->m_userAgent = rt->getString(8);
        results.push_back(v);
    };
    return 0;
}

int EmailVerificationInfoDao::QueryByEmailType(std::vector<EmailVerificationInfo::ptr>& results,  const std::string& email,  const std::string& type, sylar::IDB::ptr conn) {
    std::string sql = "select id, email, code, type, state, create_time, expires_time, client_ip, user_agent from email_verification where email = ? and type = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, email);
    stmt->bindString(2, type);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        EmailVerificationInfo::ptr v(new EmailVerificationInfo);
        v->m_id = rt->getInt64(0);
        v->m_email = rt->getString(1);
        v->m_code = rt->getString(2);
        v->m_type = rt->getString(3);
        v->m_state = rt->getInt32(4);
        v->m_createTime = rt->getTime(5);
        v->m_expiresTime = rt->getTime(6);
        v->m_clientIp = rt->getString(7);
        v->m_userAgent = rt->getString(8);
        results.push_back(v);
    };
    return 0;
}

int EmailVerificationInfoDao::QueryByExpiresTime(std::vector<EmailVerificationInfo::ptr>& results,  const int64_t& expires_time, sylar::IDB::ptr conn) {
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
        v->m_type = rt->getString(3);
        v->m_state = rt->getInt32(4);
        v->m_createTime = rt->getTime(5);
        v->m_expiresTime = rt->getTime(6);
        v->m_clientIp = rt->getString(7);
        v->m_userAgent = rt->getString(8);
        results.push_back(v);
    };
    return 0;
}

int EmailVerificationInfoDao::QueryByCreateTime(std::vector<EmailVerificationInfo::ptr>& results,  const int64_t& create_time, sylar::IDB::ptr conn) {
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
        v->m_type = rt->getString(3);
        v->m_state = rt->getInt32(4);
        v->m_createTime = rt->getTime(5);
        v->m_expiresTime = rt->getTime(6);
        v->m_clientIp = rt->getString(7);
        v->m_userAgent = rt->getString(8);
        results.push_back(v);
    };
    return 0;
}

int EmailVerificationInfoDao::CreateTableSQLite3(sylar::IDB::ptr conn) {
    return conn->execute("CREATE TABLE email_verification("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "email TEXT NOT NULL DEFAULT '',"
            "code TEXT NOT NULL DEFAULT '',"
            "type TEXT NOT NULL DEFAULT '',"
            "state INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "expires_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "client_ip TEXT NOT NULL DEFAULT '',"
            "user_agent TEXT NOT NULL DEFAULT '');"
            "CREATE INDEX email_verification_email_code ON email_verification(email,code);"
            "CREATE INDEX email_verification_email_type ON email_verification(email,type);"
            "CREATE INDEX email_verification_expires_time ON email_verification(expires_time);"
            "CREATE INDEX email_verification_create_time ON email_verification(create_time);"
            );
}

int EmailVerificationInfoDao::CreateTableMySQL(sylar::IDB::ptr conn) {
    return conn->execute("CREATE TABLE email_verification("
            "`id` bigint AUTO_INCREMENT COMMENT '主键id',"
            "`email` varchar(128) NOT NULL DEFAULT '' COMMENT '用户邮箱地址',"
            "`code` varchar(128) NOT NULL DEFAULT '' COMMENT '验证码',"
            "`type` varchar(128) NOT NULL DEFAULT '' COMMENT '验证类型: 1-注册, 2-登录, 3-密码重置, 4-更换邮箱',"
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
} //namespace data
} //namespace blog
