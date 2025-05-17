#include "user_reset_passwd_servlet.h"
#include "chen/log/log.h"
#include "../manager/user_manager.h"
#include "../util.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

UserResetPasswdServlet::UserResetPasswdServlet()
    :BlogServlet("UserResetPasswdServlet") {
}

int32_t UserResetPasswdServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, email, "email");
        DEFINE_AND_CHECK_STRING(result, passwd, "passwd");
        DEFINE_AND_CHECK_STRING(result, auth_code, "auth_code");

        if (passwd.empty()) {
            result->setResult(400, "param passwd empty");
            break;
        }

        if (!is_email(email)) {
            result->setResult(402, "invalid email format");
            break;
        }
        if (!blog::UserMgr::GetInstance()->getByEmail(email)) {
            result->setResult(401, "email not register");
            break;
        }

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db connection fail");
            break;
        }

        if (!verificationEmailCode(db, email, auth_code)) {
            result->setResult(403, "invalid auth_code");
            break;
        }

        sylar::ITransaction::ptr trans = db->openTransaction();
        data::UserInfo::ptr info = UserMgr::GetInstance()->getByEmail(email);
        info->setPasswd(sylar::md5(passwd));

        if (data::UserInfoDao::Update(info, db)) {
            result->setResult(500, "insert user fail");
            break;
        }
        trans->commit();
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
};

bool UserResetPasswdServlet::verificationEmailCode(sylar::IDB::ptr conn, const std::string& email
        ,const std::string& code) {
    // 开启事务
    sylar::ITransaction::ptr trans = conn->openTransaction();
    std::string sql = "UPDATE email_verification SET state = 1 WHERE email = ? AND code = ? AND type = 3 AND state = 0 AND expires_time > datetime('now')";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return false;
    }
    stmt->bindString(1, email);
    stmt->bindString(2, code);
    
    stmt->execute();
    // 获取影响的行数
    int rows = sqlite3_changes(std::dynamic_pointer_cast<sylar::SQLite3>(conn)->getDB());
    trans->commit();
    INFO(logger) << "rows = " << rows;
    return rows > 0;
}

}
}
