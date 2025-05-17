#include "user_create_servlet.h"
#include "chen/log/log.h"
#include "../util.h"
#include "../manager/user_manager.h"
#include "blog/data/email_verification_info.h"

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

        // TODO: 校验验证码

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db connection fail");
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

}
}
