#include "backup_create_servlet.h"

#include "../../manager/user_manager.h"
#include "../../manager/backup_record_manager.h"

namespace blog {
namespace servlet {

BackupCreateServlet::BackupCreateServlet()
    : BlogLoginedServlet("BackupCreateServlet") {
}

int32_t BackupCreateServlet::handle(chen::http::HttpRequest::ptr request,
        chen::http::HttpResponse::ptr response, chen::http::HttpSession::ptr session,
        Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setErrno(errcode::NOT_LOGIN);
            break;
        }
        auto current_user = UserMgr::GetInstance()->get(uid);
        if (!current_user) {
            result->setErrno(errcode::USER_NOT_FOUND);
            break;
        }
        if (current_user->getRole() != UserManager::Role::ADMIN) {
            result->setErrno(errcode::ACCESS_DENIED);
            break;
        }

        std::string type;
        if (!request->checkGetParamAs("type", type) || type.empty()) {
            result->setErrno(errcode::PARAM_MISSING, "param type is required");
            break;
        }
        if (type != "full" && type != "incremental") {
            result->setErrno(errcode::PARAM_INVALID, "type must be full or incremental");
            break;
        }

        std::string timestamp = std::to_string(time(0));
        std::string name = "backup_" + type + "_" + timestamp;

        auto info = BackupRecordMgr::GetInstance()->create(uid, type, name, "");
        if (!info) {
            result->setErrno(errcode::DB_OPERATION_FAILED, "create backup failed");
            break;
        }

        Json::Value item;
        item["id"] = info->getId();
        item["name"] = info->getName();
        item["type"] = info->getType();
        item["size"] = info->getSize();
        item["fileCount"] = info->getFileCount();
        item["status"] = info->getStatus();
        item["createdAt"] = info->getCreateTime();
        item["createdBy"] = info->getCreatedBy();
        item["description"] = info->getDescription();

        result->set("data", item);
        result->setErrno(errcode::SUCCESS);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
