#include "user_admin_update_servlet.h"
#include <chen/log/log.h>
#include "../../manager/user_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

UserAdminUpdateServlet::UserAdminUpdateServlet()
    :BlogLoginedServlet("UserAdminUpdateServlet") {
}

int32_t UserAdminUpdateServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, user_id, "user_id");

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

        auto info = UserMgr::GetInstance()->get(user_id);
        if (!info) {
            result->setResult(404, "user not found");
            break;
        }

        int32_t old_role = info->getRole();
        std::string old_account = info->getAccount();
        std::string old_email = info->getEmail();

        std::string account = request->getParam("account");
        std::string email = request->getParam("email");
        std::string passwd = request->getParam("passwd");
        int32_t role = 0;
        int32_t state = 0;
        bool has_role = request->checkGetParamAs("role", role);
        bool has_state = request->checkGetParamAs("state", state);

        if (account.empty() && email.empty() && passwd.empty() && !has_role && !has_state) {
            result->setResult(400, "no param to update");
            break;
        }

        if (!account.empty()) {
            if (!is_vaild_account(account)) {
                result->setResult(402, "invalid account");
                break;
            }
            auto exist = UserMgr::GetInstance()->getByAccount(account);
            if (exist && exist->getId() != user_id) {
                result->setResult(401, "account exists");
                break;
            }
            info->setAccount(account);
        }
        if (!email.empty()) {
            if (!is_email(email)) {
                result->setResult(402, "invalid email format");
                break;
            }
            auto exist = UserMgr::GetInstance()->getByEmail(email);
            if (exist && exist->getId() != user_id) {
                result->setResult(401, "email exists");
                break;
            }
            info->setEmail(email);
        }
        if (!passwd.empty()) {
            if (passwd.length() < 6) {
                result->setResult(400, "passwd must be at least 6 characters");
                break;
            }
            info->setPasswd(chen::md5(passwd));
        }
        if (has_role) {
            if (role < 1 || role > 4) {
                result->setResult(400, "invalid role");
                break;
            }
            info->setRole(role);
        }
        if (has_state) {
            if (state < 1 || state > 2) {
                result->setResult(400, "invalid state");
                break;
            }
            info->setState(state);
        }

        info->setUpdateTime(time(0));

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db error");
            break;
        }
        if (data::UserInfoDao::Update(info, db)) {
            result->setResult(500, "update user fail");
            ERROR(logger) << "db error errno=" << db->getErrno()
                << " errstr=" << db->getErrStr();
            break;
        }

        UserMgr::GetInstance()->update(info, old_role, old_account, old_email);

        result->set("id", info->getId());
        result->set("account", info->getAccount());
        result->set("name", info->getName());
        result->set("email", info->getEmail());
        result->set("role", info->getRole());
        result->set("state", info->getState());
        result->set("update_time", info->getUpdateTime());
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
