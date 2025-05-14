#include "user_forget_password_servlet.h"
#include "chen/log/log.h"
#include "chen/config/config.h"
#include "../util.h"
#include "../manager/user_manager.h"

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
            result->setResult(402, "email not register");
            break;
        }

        auto data = getSessionData(request, response);
        if (!data) {
            result->setResult(502, "not login");
            break;
        }

        int64_t now = time(0);
        int64_t email_time = data->getData<int64_t>(CookieKey::EMAIL_LAST_TIME);
        if ((now - email_time) < email_interval_time->getValue()) {
            result->setResult(502, "article too often");
            break;
        }

        // TODO: 验证发送
    } while (false);
    response->setBody(result->toJsonString());
    return 0;
};

}
}
