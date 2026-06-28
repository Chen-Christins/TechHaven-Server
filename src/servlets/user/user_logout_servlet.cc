#include "user_logout_servlet.h"

#include <chen/log/log.h>

#include "../../manager/user_manager.h"
#include "../../include/tables.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

UserLogoutServlet::UserLogoutServlet()
    :BlogServlet("UserLogoutServlet") {
}

int32_t UserLogoutServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        auto sdata = getSessionData(request, response);
        if (!sdata->getData<int64_t>(CookieKey::USER_ID)) {
            result->setErrno(errcode::NOT_LOGIN);
            break;
        }

        int64_t uid = sdata->getData<int64_t>(CookieKey::USER_ID);
        result->setErrno(errcode::SUCCESS);

        // 清除数据库中存储的 token，使其他设备登录失效
        auto uinfo = UserMgr::GetInstance()->get(uid);
        if (uinfo) {
            uinfo->setToken("");
            uinfo->setTokenTime(0);
            auto db = getDB();
            if (db) {
                data::UserInfoDao::Update(uinfo, db);
            }
        }

        int64_t token_time = time(0) - 3600 * 24;
        response->setCookie(CookieKey::USER_ID, "", token_time, "/");
        response->setCookie(CookieKey::TOKEN, "", token_time, "/");
        response->setCookie(CookieKey::TOKEN_TIME, "", token_time, "/");
        sdata->setData(CookieKey::USER_ID, (int64_t)0);
        std::string id = sdata->getId();
        chen::http::SessionDataMgr::GetInstance()->del(id);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
