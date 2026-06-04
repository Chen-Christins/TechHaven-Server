#include "user_create_servlet.h"
#include <chen/log/log.h>
#include <chen/db/redis.h>

#include "../../util.h"
#include "../../manager/user_manager.h"
#include "../../manager/system_settings_manager.h"

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
        if (!IsEmail(email)) {
            result->setResult(402, "invalid email format");
            break;
        }
        if (!IsValidAccount(account)) {
            result->setResult(402, "invalid account");
            break;
        }

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db connection fail");
            break;
        }

        // 根据系统设置决定是否需要校验邮箱验证码
        auto sys_settings = SystemSettingsMgr::GetInstance()->get();
        bool require_verification = true;
        if (sys_settings) {
            require_verification = sys_settings->getRequireEmailVerification() != 0;
        }
        if (require_verification) {
            if (!verificationEmailCode(email, auth_code)) {
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

bool UserCreateServlet::verificationEmailCode(const std::string& email, const std::string& code) {
    auto rpy = chen::RedisUtil::Cmd("blog", "GET email:verify:1:%s", email.c_str());
    if (!rpy || rpy->type != REDIS_REPLY_STRING) {
        return false;
    }
    if (code != rpy->str) {
        return false;
    }
    // 一次性使用，校验通过后删除
    chen::RedisUtil::Cmd("blog", "DEL email:verify:1:%s", email.c_str());
    return true;
}

}
}
