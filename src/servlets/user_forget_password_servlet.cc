#include "user_forget_password_servlet.h"
#include "chen/log/log.h"
#include "chen/config/config.h"
#include "../util.h"
#include "../manager/user_manager.h"
#include "chen/email/email.h"
#include "chen/email/smtp.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();
static sylar::ConfigVar<int64_t>::ptr email_interval_time =
    sylar::Config::Lookup("email.interval_time", (int64_t)60, "email interval time second");

UserForgetPasswordServlet::UserForgetPasswordServlet()
    :BlogServlet("UserForgetPasswordServlet") {
}

int32_t UserForgetPasswordServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, email, "email");

        data::UserInfo::ptr info;
        if (is_email(email)) {
            info = UserMgr::GetInstance()->getByEmail(email);
        } else {
            result->setResult(402, "invalid email");
            break;
        }

        if (!info) {
            result->setResult(403, "email not register");
            break;
        }
        auto v = sylar::random_string(6);
        info->setCode(v);
        auto db = getDB();
        if (data::UserInfoDao::Update(info, db)) {
            result->setResult(500, "db update error");
            break;
        }
        auto mail = sylar::EMail::Create("17354303956@163.com", "ASVbGLfbcJSz7JAy"
                , "Blog 重制密码 - 验证码"
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
        result->setResult(200, "ok");
    } while (false);
    response->setBody(result->toJsonString());
    return 0;
};

}
}
