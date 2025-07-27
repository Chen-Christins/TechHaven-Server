#include "user_login_servlet.h"
#include "chen/log/log.h"
#include "../../util.h"
#include "../../manager/user_manager.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

UserLoginServlet::UserLoginServlet()
    :BlogServlet("UserLoginServlet") {
}

int32_t UserLoginServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, auth_id, "auth_id");
        DEFINE_AND_CHECK_STRING(result, passwd, "passwd");

        auto sdata = getSessionData(request, response);
        if (sdata->getData<int64_t>(CookieKey::USER_ID)) {
            result->setResult(410, "already login");
            break;
        }

        data::UserInfo::ptr info;
        if (is_email(auth_id)) {
            info = UserMgr::GetInstance()->getByEmail(auth_id);
        } else if (is_vaild_account(auth_id)) {
            info = UserMgr::GetInstance()->getByAccount(auth_id);
        } else {
            result->setResult(402, "invalid auth_id");
            break;
        }

        if (!info) {
            result->setResult(403, "invalid auth_id");
            break;
        }
        if (info->getPasswd() != sylar::md5(passwd)) {
            result->setResult(410, "invalid passwd");
            break;
        }

        if (info->getState() != 1) {
            result->setResult(410, "account invalid state");
        }

        auto db = getDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }
        
        info->setLoginTime(time(0));
        uint64_t ts1 = sylar::GetCurrentUs();
        data::UserInfoDao::Update(info, db);
        INFO(logger) << "update used: " << (sylar::GetCurrentUs() - ts1) / 1000.0 << " ms";

        int64_t token_time = time(0) + 3600 * 24;
        response->setCookie(CookieKey::USER_ID, std::to_string(info->getId()), token_time, "/");
        auto token = UserManager::GetToken(info, token_time);
        response->setCookie(CookieKey::TOKEN, token, token_time, "/");
        response->setCookie(CookieKey::TOKEN_TIME, std::to_string(token_time), token_time, "/");
        sdata->setData(CookieKey::USER_ID, info->getId());
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
