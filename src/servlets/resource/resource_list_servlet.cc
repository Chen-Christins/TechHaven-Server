#include "resource_list_servlet.h"
#include <chen/log/log.h>
#include "../../util.h"
#include "../../manager/user_manager.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

ResourceListServlet::ResourceListServlet()
    : BlogLoginedServlet("ResourceListServlet") {
}

int32_t ResourceListServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, type, "type");
        DEFINE_AND_CHECK_STRING(result, folder, "folder");
        
        int64_t uid = getUserId(request);
        int32_t system_role = UserMgr::GetInstance()->get(uid)->getRole();
        if (!checkPermession(system_role)) {
            result->setResult(403, "Access Denied");
            break;
        }

        // TODO: 查询资源列表
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

bool ResourceListServlet::checkPermession(int32_t system_role) {
    if (system_role != UserManager::Role::ADMIN) {
        return false;
    }
    return true;
}

}
}
