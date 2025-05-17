#include "user_create_servlet.h"
#include "chen/log/log.h"
#include "../util.h"
#include "../manager/user_manager.h"
#include "chen/db/sqlite3.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

UserCreateServlet::UserCreateServlet()
    :BlogServlet("UserCreateServlet") {
}

int32_t UserCreateServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, account, "account");
        DEFINE_AND_CHECK_STRING(result, email, "email");
        DEFINE_AND_CHECK_STRING(result, passwd, "passwd");
        DEFINE_AND_CHECK_STRING(result, auth_code, "auth_code");

        if (passwd.empty()) {
            result->setResult(400, "param passwd empty");
            break;
        }

        if (blog::UserMgr::GetInstance()->getByAccount(account)) {
            result->setResult(401, "account exists");
            break;
        }
        if (blog::UserMgr::GetInstance()->getByEmail(email)) {
            result->setResult(401, "email exists");
            break;
        }
        if (!is_email(email)) {
            result->setResult(402, "invalid email format");
            break;
        }
        if (!is_vaild_account(account)) {
            result->setResult(402, "invalid account");
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

        // 开启事务
        sylar::ITransaction::ptr trans = db->openTransaction();
        data::UserInfo::ptr info(new data::UserInfo);
        info->setAccount(account);
        info->setEmail(email);
        info->setPasswd(passwd);
        info->setState(1);
        info->setName(account);

        if (data::UserInfoDao::Insert(info, db)) {
            result->setResult(500, "insert user fail");
            break;
        }
        trans->commit();
        UserMgr::GetInstance()->add(info);
        INFO(logger) << info->toJsonString();
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
};

bool UserCreateServlet::verificationEmailCode(sylar::IDB::ptr conn, const std::string& email
        ,const std::string& code) {
    // 开启事务
    sylar::ITransaction::ptr trans = conn->openTransaction();
    std::string sql = "UPDATE email_verification SET state = 1 WHERE email = ? AND code = ? AND state = 0 AND expires_time > datetime('now')";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return false;
    }
    stmt->bindString(1, email);
    stmt->bindString(2, code);
    
    stmt->execute();

    trans->commit();
    // 获取影响的行数
    int rows = sqlite3_changes(std::dynamic_pointer_cast<sylar::SQLite3>(conn)->getDB());
    INFO(logger) << "rows = " << rows;
    return rows > 0;
}

}
}
