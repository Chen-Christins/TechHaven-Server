#include "user_send_code_servlet.h"
#include <chen/log/log.h>
#include <chen/db/redis.h>
#include <chen/email/email.h>
#include <chen/email/smtp.h>
#include "../../manager/user_manager.h"
#include "../../manager/system_settings_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

UserSendCodeServlet::UserSendCodeServlet()
    :BlogServlet("UserSendCodeServlet") {
}

int32_t UserSendCodeServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, email, "email");
        DEFINE_AND_CHECK_STRING(result, type, "type");
        DEFINE_AND_CHECK_STRING(result, agent, "agent");

        if (email.empty() && agent.empty()) {
            result->setResult(400, "no param");
            break;
        }

        if (!IsEmail(email)) {
            result->setResult(402, "invalid email format");
            break;
        }

        if (type == "1" && blog::UserMgr::GetInstance()->getByEmail(email)) {
            result->setResult(401, "email exists");
            break;
        }

        // 检查 SMTP 配置（在写 DB 之前校验，避免产生无效验证码）
        auto sys_settings = SystemSettingsMgr::GetInstance()->get();
        if (!sys_settings || sys_settings->getSmtpHost().empty()) {
            result->setResult(501, "SMTP server not configured");
            break;
        }

        // 生成验证码并写入 Redis，10分钟过期
        std::string code = chen::random_string(8);
        auto rpy = chen::RedisUtil::Cmd("blog", "SETEX email:verify:%s:%s 600 %s", type.c_str(), email.c_str(), code.c_str());
        if (!rpy) {
            result->setResult(500, "redis setex fail");
            break;
        }

        INFO(logger) << "email=" << email << " type=" << type << " code=" << code;

        // 异步发送邮件，不阻塞请求响应
        std::string title = (type == "1" ? "Blog Create Account Auth - 验证码" : "Blog 重置密码 - 验证码");
        auto mail = chen::EMail::Create(sys_settings->getSmtpUsername(), sys_settings->getSmtpPassword()
                , title
                , "验证码[" + code +"]"
                , {email}, {}, {sys_settings->getFromEmail()});
        std::string smtp_host = sys_settings->getSmtpHost();
        int32_t smtp_port = sys_settings->getSmtpPort();

        chen::IOManager::GetThis()->schedule([mail, smtp_host, smtp_port]() {
            auto client = chen::SmtpClient::Create(smtp_host, smtp_port, true);
            if (!client) {
                ERROR(logger) << "connect email server fail";
                return;
            }
            auto r = client->send(mail, 5000);
            if (r->result != 0) {
                ERROR(logger) << "send email fail: " << r->result << " " << r->msg;
            }
        });

        result->setResult(200, "ok");
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
