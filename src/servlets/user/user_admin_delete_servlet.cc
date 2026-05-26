#include "user_admin_delete_servlet.h"
#include <chen/log/log.h>
#include "../../manager/user_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

UserAdminDeleteServlet::UserAdminDeleteServlet()
    :BlogLoginedServlet("UserAdminDeleteServlet") {
}

int32_t UserAdminDeleteServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, user_id, "user_id");

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }
        int32_t role = UserMgr::GetInstance()->get(uid)->getRole();

        if (role != UserManager::Role::ADMIN) {
            result->setResult(403, "Access Denied");
            break;
        }

        auto info = UserMgr::GetInstance()->get(user_id);
        if (!info) {
            result->setResult(404, "user not found");
            break;
        }
        if (info->getIsDeleted()) {
            result->setResult(400, "user already deleted");
            break;
        }

        auto db = getDB();
        auto trans = db->openTransaction();
        if (!trans) {
            result->setResult(500, "open transaction fail");
            break;
        }
        time_t now = time(0);
        info->setIsDeleted(1);
        info->setUpdateTime(now);
        if (data::UserInfoDao::Update(info, db)) {
            ERROR(logger) << "update user fail";
            result->setResult(500, "delete user fail");
            break;
        }
        if (!trans->commit()) {
            ERROR(logger) << "commit fail";
            info->setIsDeleted(0);
            result->setResult(500, "commit fail");
            break;
        }
        result->set("user_id", user_id);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
