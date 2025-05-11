#include "user_create_servlet.h"
#include "chen/log/log.h"
#include "../manager/user_manager.h"
#include "chen/email/email.h"
#include "chen/email/smtp.h"
#include "../util.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

UserCreateServlet::UserCreateServlet()
    :BlogServlet("UserCreate") {
}

int32_t UserCreateServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        , sylar::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, account, "account");
        DEFINE_AND_CHECK_STRING(result, email, "email");
        DEFINE_AND_CHECK_STRING(result, passwd, "passwd");

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
        // 开启事务
        sylar::ITransaction::ptr trans = db->openTransaction();
        std::string v = sylar::random_string(16);
        data::UserInfo::ptr info(new data::UserInfo);
        info->setAccount(account);
        info->setEmail(email);
        info->setPasswd(passwd);
        info->setState(1);
        info->setName(account);
        info->setCode(v);
        
        if (data::UserInfoDao::Insert(info, db)) {
            result->setResult(500, "insert user fail");
            break;
        }

        auto mail = sylar::EMail::Create("17354303956@163.com", "ASVbGLfbcJSz7JAy"
                , "Blog Create Account Auth - 验证码"
                , "验证码[" + v +"]"
                , {email}, {}, {"17354303956@163.com"});

        auto client = sylar::SmtpClient::Create("smtp.163.com", 25);
        if (!client) {
            ERROR(logger) << "connect email server fail";
            result->setResult(501, "connect email server fail");
            break;
        }

        auto r = client->send(mail, 5000);
        if (r->result != 0) {
            result->setResult(501, std::to_string(r->result) + " " + r->msg);
            break;
        }
        
        // 提交事务
        trans->commit();
        UserMgr::GetInstance()->add(info);
        INFO(logger) << info->toJsonString();
    } while (false);

    response->setBody(result->toJsonString());
    return 0;
}

}
}