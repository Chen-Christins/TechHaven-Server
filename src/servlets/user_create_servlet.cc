#include "user_create_servlet.h"
#include "log/log.h"
#include "../manager/user_manager.h"
#include "../my_module.h"
#include "email/email.h"
#include "email/smtp.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

UserCreateServlet::UserCreateServlet()
    :sylar::http::Servlet("UserCreate") {
}

int32_t UserCreateServlet::handle(sylar::http::HttpRequest::ptr request
        ,sylar::http::HttpResponse::ptr response, sylar::http::HttpSession::ptr session) {
    int code = 200;
    std::string msg = "ok";
    do {
        auto account = request->getParam("account");
        if (account.empty()) {
            code = 400;
            msg = "param account is null";
            break;
        }
        auto email = request->getParam("email");
        if (email.empty()) {
            code = 400;
            msg = "param email is null";
            break;
        }
        auto passwd = request->getParam("passwd");
        if (passwd.empty()) {
            code = 400;
            msg = "param passwd is null";
            break;
        }
        if (blog::UserMgr::GetInstance()->getByAccount(account)) {
            code = 401;
            msg = "account exists";
            break;
        }
        if (blog::UserMgr::GetInstance()->getByAccount(email)) {
            code = 401;
            msg = "email exists";
            break;
        }

        auto db = blog::GetSQLite3();
        if (!db) {
            code = 500;
            msg = "get db connection fail";
            break;
        }
        
        // TODO: 验证账号是否合法，验证邮箱是否合法

        std::string v = sylar::random_string(16);
        data::UserInfo::ptr info(new data::UserInfo);
        info->setAccount(account);
        info->setEmail(email);
        info->setPasswd(passwd);
        info->setState(1);
        info->setName(account);
        info->setCode(v);
        
        if (data::UserInfoDao::Insert(info, db)) {
            code = 500;
            msg = "insert user fail";
            break;
        }

        UserMgr::GetInstance()->add(info);
        
        auto mail = sylar::EMail::Create("17354303956@163.com", "ASVbGLfbcJSz7JAy"
                , "Blog Create Account Auth"
                , "Auth code[" + v +"]"
                , {email}, {}, {"17354303956@163.com"});

        auto client = sylar::SmtpClient::Create("smtp.163.com", 25);
        if (!client) {
            ERROR(logger) << "connect email server fail";
            code = 501;
            msg = "connect email server fail";
            break;
        }

        auto r = client->send(mail);
        if (r->result != 0) {
            code = 501;
            msg = std::to_string(r->result) + " " + r->msg;
            break;
        }
        INFO(logger) << info->toJsonString();
    } while (false);

    Json::Value v;
    v["code"] = std::to_string(code);
    v["msg"] = msg;

    INFO(logger) << "UserCreateServlet: " << *request;
    response->setBody(sylar::JsonUtil::ToString(v));
    return 0;
}

}
}