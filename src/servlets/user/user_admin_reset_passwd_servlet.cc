#include "user_admin_reset_passwd_servlet.h"
#include <chen/log/log.h>
#include "../../manager/user_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

UserAdminResetPasswdServlet::UserAdminResetPasswdServlet()
    :BlogLoginedServlet("UserAdminResetPasswdServlet") {
}

int32_t UserAdminResetPasswdServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");
        DEFINE_AND_CHECK_STRING(result, passwd_f, "passwd_f");
        DEFINE_AND_CHECK_STRING(result, passwd_s, "passwd_s");

        int64_t uid = getUserId(request);
		auto role = UserMgr::GetInstance()->get(uid)->getRole();

		if (role != "admin") {
			result->setResult(403, "Access Denied");
			break;
		}

        if (passwd_f.empty() || passwd_s.empty()) {
            result->setResult(400, "param passwd empty");
            break;
        }

        if (passwd_f != passwd_s) {
			result->setResult(400, "the passwords is different");
            break;
		}

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db connection fail");
            break;
        }

        sylar::ITransaction::ptr trans = db->openTransaction();
        data::UserInfo::ptr info = UserMgr::GetInstance()->get(id);
        info->setPasswd(sylar::md5(passwd_s));

        if (data::UserInfoDao::Update(info, db)) {
            result->setResult(500, "insert user fail");
            break;
        }
        trans->commit();
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
