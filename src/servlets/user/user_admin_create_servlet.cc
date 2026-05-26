#include "user_admin_create_servlet.h"
#include <chen/log/log.h>
#include "../../manager/user_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

UserAdminCreateServlet::UserAdminCreateServlet()
    :BlogLoginedServlet("UserAdminCreateServlet") {
}

int32_t UserAdminCreateServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, account, "account");
        DEFINE_AND_CHECK_STRING(result, email, "email");
        DEFINE_AND_CHECK_STRING(result, passwd, "passwd");
        DEFINE_AND_CHECK_TYPE(result, int32_t, role, "role");
        DEFINE_AND_CHECK_TYPE(result, int32_t, state, "state");

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }
        int32_t userRole = UserMgr::GetInstance()->get(uid)->getRole();

        if (userRole != UserManager::Role::ADMIN) {
            result->setResult(403, "Access Denied");
            break;
        }

        if (account.empty() || passwd.empty()) {
            result->setResult(400, "param account passwd empty");
            break;
        }
        if (passwd.length() < 6) {
            result->setResult(400, "passwd must be at least 6 characters");
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
        if (role < 1 || role > 4) {
            result->setResult(400, "invalid role");
            break;
        }
        if (state < 1 || state > 2) {
            result->setResult(400, "invalid state");
            break;
        }

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db connection fail");
            break;
        }
        chen::ITransaction::ptr trans = db->openTransaction();
        data::UserInfo::ptr info(new data::UserInfo);
        info->setAccount(account);
        info->setEmail(email);
        info->setPasswd(chen::md5(passwd));
        info->setRole(role);
        info->setState(state);
        info->setName(account);

        if (data::UserInfoDao::Insert(info, db)) {
            result->setResult(500, "insert user fail");
            break;
        }
        trans->commit();
        UserMgr::GetInstance()->add(info);

        result->set("id", info->getId());
        result->set("account", info->getAccount());
        result->set("name", info->getName());
        result->set("email", info->getEmail());
        result->set("role", info->getRole());
        result->set("state", info->getState());
        result->set("create_time", info->getCreateTime());
        INFO(logger) << info->toJsonString();
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
