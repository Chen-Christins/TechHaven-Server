#include "user_admin_lists_servlet.h"
#include "../../util.h"
#include "../../manager/user_manager.h"
#include <chen/util/json_util.h>

namespace blog {
namespace servlet {

UserAdminListsServlet::UserAdminListsServlet()
    :BlogLoginedServlet("UserAdminListsServlet") {
}

int32_t UserAdminListsServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) { 
    do {
        DEFINE_AND_CHECK_TYPE(result, uint64_t, page_size, "page_size");
        DEFINE_AND_CHECK_TYPE(result, uint64_t, page_num, "page_num");

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }
        auto role = UserMgr::GetInstance()->get(uid)->getRole();

        if (role != "admin") {
            result->setResult(403, "Access Denied");
            break;
        }

        uint64_t offset = (page_num - 1) * page_size;
        std::vector<data::UserInfo::ptr> users;
        uint64_t total = UserMgr::GetInstance()->listByPages(users, offset, page_size, false);
        
        result->set("total", total);
        auto& list = result->jsondata["list"];
        for (const auto& user : users) {
            Json::Value item;
            item["id"] = user->getId();
            item["account"] = user->getAccount();
            item["name"] = user->getName();
            item["email"] = user->getEmail();
            item["role"] = user->getRole();
            item["create_time"] = user->getCreateTime();
            item["login_time"] = user->getLoginTime();
            item["update_time"] = user->getUpdateTime();
            item["is_deleted"] = user->getIsDeleted();
            list.append(item);
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}