#include "user_reset_passwd_servlet.h"
#include "chen/log/log.h"
#include "../manager/user_manager.h"
#include "../util.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

UserResetPasswdServlet::UserResetPasswdServlet()
    :BlogServlet("UserResetPasswdServlet") {
}

int32_t UserResetPasswdServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, email, "email");
        DEFINE_AND_CHECK_STRING(result, passwd, "passwd");
        DEFINE_AND_CHECK_STRING(result, auth_code, "auth_code");

        if (email.empty() || passwd.empty()) {
            result->setResult(400, "some param null");
            break;
        }
        if (!is_email(email)) {
            result->setResult(402, "invalid email format");
            break;
        }

        data::UserInfo::ptr info = UserMgr::GetInstance()->getByEmail(email);
        if (!info) {
            result->setResult(402, "email not register");
            break;
        }
        if (info->getCode() != auth_code) {
            result->setResult(410, "invalid auth_code");
            break;
        }
        info->setCode("");
        info->setPasswd(passwd);
        info->setUpdateTime(time(0));

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db error");
            break;
        }
        if (data::UserInfoDao::Update(info, db)) {
            result->setResult(500, "db update error");
            ERROR(logger) << "user_data: " << info->toJsonString()
                << " errno: " << db->getErrno()
                << " errstr: " << db->getErrStr();
            break;
        }
        result->setResult(200, "ok");
    } while (false);
    response->setBody(result->toJsonString());
    return 0;
};

}
}
