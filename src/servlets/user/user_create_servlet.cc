#include "user_create_servlet.h"
#include <chen/log/log.h>
#include "../../util.h"
#include "../../manager/user_manager.h"
#include "../../manager/system_settings_manager.h"
#include <chen/db/sqlite3.h>

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

UserCreateServlet::UserCreateServlet()
    :BlogServlet("UserCreateServlet") {
}

int32_t UserCreateServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, account, "account");
        DEFINE_AND_CHECK_STRING(result, email, "email");
        DEFINE_AND_CHECK_STRING(result, passwd, "passwd");
        DEFINE_AND_CHECK_STRING(result, auth_code, "auth_code");

        if (account.empty() || passwd.empty()) {
            result->setResult(400, "param account passwd empty");
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

        // 根据系统设置决定是否需要校验邮箱验证码
        auto sysSettings = SystemSettingsMgr::GetInstance()->get();
        bool requireVerification = true;
        if (sysSettings) {
            requireVerification = sysSettings->getRequireEmailVerification() != 0;
        }
        if (requireVerification) {
            if (!verificationEmailCode(db, email, auth_code)) {
                result->setResult(403, "invalid auth_code");
                break;
            }
        }

        // 开启事务
        chen::ITransaction::ptr trans = db->openTransaction();
        data::UserInfo::ptr info(new data::UserInfo);
        info->setAccount(account);
        info->setEmail(email);
        info->setPasswd(chen::md5(passwd));
        info->setState(UserManager::Status::ACTIVE);
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
}

bool UserCreateServlet::verificationEmailCode(chen::IDB::ptr conn, const std::string& email
        ,const std::string& code) {
    // 开启事务
    chen::ITransaction::ptr trans = conn->openTransaction();
	// 先查询是否存在这个记录
	std::string select_sql = "SELECT 1 FROM email_verification WHERE email = ? AND code = ? AND type = 1 AND state = 0 AND expires_time > datetime('now')";
    
	auto stmt = std::dynamic_pointer_cast<chen::SQLite3Stmt>(conn->prepare(select_sql));
    if(!stmt) {
        ERROR(logger) << "stmt=" << select_sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return false;
    }
    stmt->bindString(1, email);
	stmt->bindString(2, code);

	int rt = stmt->step();
    bool canUpdate = (rt == SQLITE_ROW);
	stmt->finish();

	if (!canUpdate) {
		return false;
	}

    std::string update_sql = "UPDATE email_verification SET state = 1 WHERE email = ? AND code = ? AND type = 1 AND state = 0 AND expires_time > datetime('now')";
	stmt = std::dynamic_pointer_cast<chen::SQLite3Stmt>(conn->prepare(update_sql));

	stmt->bindString(1, email);
	stmt->bindString(2, code);

    // 获取影响的行数, 是否成功
    bool success = stmt->step() == SQLITE_DONE && sqlite3_changes(std::dynamic_pointer_cast<chen::SQLite3>(conn)->getDB()) > 0;
    trans->commit();
	
    return success;
}

}
}
