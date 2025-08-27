#include "user_send_code_servlet.h"
#include <chen/log/log.h>
#include <chen/config/config.h>
#include "blog/data/email_verification_info.h"
#include <chen/email/email.h>
#include <chen/email/smtp.h>
#include "../../manager/user_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();
static sylar::ConfigVar<std::string>::ptr email_host = 
    sylar::Config::Lookup("server.email_service.host", std::string(), "the token of email service");

static sylar::ConfigVar<uint32_t>::ptr email_port = 
    sylar::Config::Lookup("server.email_service.port", uint32_t(25), "the port of email service");

static sylar::ConfigVar<std::string>::ptr email_addr = 
    sylar::Config::Lookup("server.email_service.address", std::string(), "the address of email service");

static sylar::ConfigVar<std::string>::ptr email_token = 
    sylar::Config::Lookup("server.email_service.token", std::string(), "the token of email service");


UserSendCodeServlet::UserSendCodeServlet()
    :BlogServlet("UserSendCodeServlet") {
}

int32_t UserSendCodeServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, email, "email");
        DEFINE_AND_CHECK_STRING(result, type, "type");
        DEFINE_AND_CHECK_STRING(result, agent, "agent");

        if (email.empty() && agent.empty()) {
            result->setResult(400, "no param");
            break;
        }

        if (!is_email(email)) {
            result->setResult(402, "invalid email format");
            break;
        }

        if (type == "1" && blog::UserMgr::GetInstance()->getByEmail(email)) {
            result->setResult(401, "email exists");
            break;
        }

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db connection error");
            break;
        }

        // 开启事务
        sylar::ITransaction::ptr trans = db->openTransaction();
        // 生成验证码和连接端(对端)ip
        std::string code = sylar::random_string(8);
        std::string ipaddr = session->getRemoteAddressString();
        // 设置插入信息
        data::EmailVerificationInfo::ptr info(new data::EmailVerificationInfo);
        info->setEmail(email);
        info->setCode(code);
        info->setType(std::stoi(type));
        info->setState(0);
        info->setExpiresTime(time(0) + 10 * 60);
        info->setClientIp(ipaddr);
        info->setUserAgent(agent);

        if (data::EmailVerificationInfoDao::Insert(info, db)) {
            result->setResult(500, "insert email fail");
            break;
        }
        // 发送邮件
        std::string title = (type == "1" ? "Blog Create Account Auth - 验证码" : "Blog 重置密码 - 验证码");
        auto mail = sylar::EMail::Create(email_addr->getValue(), email_token->getValue()
                , title
                , "验证码[" + code +"]"
                , {email}, {}, {email_addr->getValue()});

        auto client = sylar::SmtpClient::Create(email_host->getValue(), email_port->getValue(), true);
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
        // 提交事务，将验证码存入数据库
        trans->commit();
        INFO(logger) << info->toJsonString();
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
