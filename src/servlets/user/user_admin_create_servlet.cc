#include "user_admin_create_servlet.h"
#include "chen/log/log.h"
#include "../../manager/user_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

UserAdminCreateServlet::UserAdminCreateServlet()
    :BlogLoginedServlet("UserAdminCreateServlet") {
}

int32_t UserAdminCreateServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
	do {
		DEFINE_AND_CHECK_STRING(result, account, "account");
        DEFINE_AND_CHECK_STRING(result, email, "email");
        DEFINE_AND_CHECK_STRING(result, passwd_f, "passwd_f");
        DEFINE_AND_CHECK_STRING(result, passwd_s, "passwd_s");
		
		int64_t uid = getUserId(request);
		auto role = UserMgr::GetInstance()->get(uid)->getRole();

		if (role != "admin") {
			result->setResult(403, "Access Denied");
			break;
		}

		if (account.empty() || passwd_f.empty() || passwd_s.empty()) {
            result->setResult(400, "param account passwd empty");
            break;
        }
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
		if (passwd_f != passwd_s) {
			result->setResult(400, "the passwords is different");
            break;
		}

		auto db = getDB();
        if (!db) {
            result->setResult(500, "get db connection fail");
            break;
        }
		// 开启事务
        sylar::ITransaction::ptr trans = db->openTransaction();
        data::UserInfo::ptr info(new data::UserInfo);
        info->setAccount(account);
        info->setEmail(email);
        info->setPasswd(sylar::md5(passwd_s));
        info->setState(1);
        info->setName(account);

        if (data::UserInfoDao::Insert(info, db)) {
            result->setResult(500, "insert user fail");
            break;
        }
        trans->commit();
        UserMgr::GetInstance()->add(info);
        INFO(logger) << info->toJsonString();
	} while (0);
	response->setBody(result->toJsonString());
    return 0;
}

}
}
