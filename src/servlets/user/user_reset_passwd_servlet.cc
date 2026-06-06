#include "user_reset_passwd_servlet.h"
#include <chen/log/log.h>
#include <chen/db/redis.h>

#include "../../manager/user_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

UserResetPasswdServlet::UserResetPasswdServlet()
    :BlogServlet("UserResetPasswdServlet") {
}

int32_t UserResetPasswdServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, email, "email");
        DEFINE_AND_CHECK_STRING(result, passwd, "passwd");
        DEFINE_AND_CHECK_STRING(result, auth_code, "auth_code");

        if (passwd.empty()) {
            result->setResult(400, "param passwd empty");
            break;
        }

        if (!IsEmail(email)) {
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

        if (!verificationEmailCode(email, auth_code)) {
            result->setResult(403, "invalid auth_code");
            break;
        }

        chen::ITransaction::ptr trans = db->openTransaction();
        data::UserInfo::ptr info = UserMgr::GetInstance()->getByEmail(email);
        info->setPasswd(chen::md5(passwd));

        if (data::UserInfoDao::Update(info, db)) {
            result->setResult(500, "insert user fail");
            break;
        }
        trans->commit();
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

bool UserResetPasswdServlet::verificationEmailCode(const std::string& email, const std::string& code) {
    auto rpy = chen::RedisUtil::Cmd("blog", "GET email:verify:3:%s", email.c_str());
    if (!rpy || rpy->type != REDIS_REPLY_STRING) {
        return false;
    }
    if (code != rpy->str) {
        return false;
    }
    // 一次性使用，校验通过后删除
    chen::RedisUtil::Cmd("blog", "DEL email:verify:3:%s", email.c_str());
    return true;
}

}
}
