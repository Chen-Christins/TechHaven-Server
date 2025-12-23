#include "file_download_servlet.h"
#include <chen/log/log.h>
#include <chen/config/config.h>
#include "../../util.h"
#include "../../manager/user_manager.h"
#include "../../manager/resource_manager.h"


namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();
static chen::ConfigVar<std::string>::ptr server_work_path = chen::Config::Lookup<std::string>("server.work_path");

FileDownloadServlet::FileDownloadServlet()
    : BlogLoginedServlet("FileDownloadServlet") {
}

int32_t FileDownloadServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    bool success = false;
    do {
        DEFINE_AND_CHECK_STRING(result, url, "url");
        DEFINE_AND_CHECK_STRING(result, fileName, "fileName");

        auto res_info = ResourceMgr::GetInstance()->getByPath(url);
        if (!res_info) {
            result->setResult(404, "resource not found");
            break;
        }

        int64_t uid = getUserId(request);
        int32_t system_role = UserMgr::GetInstance()->get(uid)->getRole();
        if (!checkPermission(system_role) || res_info->getOwnerId() != uid) {
            result->setResult(403, "Access Denied");
            break;
        }
        INFO(logger) << "File download: " << res_info->getPath();

        response->setHeader("Content-Disposition", "attachment; filename=\"" + fileName + "\"");
        response->setHeader("Content-Length", std::to_string(res_info->getSize()));
        response->setHeader("Content-Type", "application/octet-stream");

        std::string filepath = server_work_path->getValue() + res_info->getPath();
        std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);
        if (!ifs) {
            result->setResult(404, "file not found");
            break;
        }
        std::streamsize size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::vector<char> buffer(size);
        if (!ifs.read(buffer.data(), size)) {
            result->setResult(500, "file read error");
            break;
        }
        response->setBody(std::string(buffer.begin(), buffer.end()));
        success = true;
    } while (0);
    if (!success) {
        response->setBody(result->toJsonString());
    }
    INFO(logger) << "File download response: " << response->toString();
    return 0;
}

bool FileDownloadServlet::checkPermission(int32_t system_role) {
    // 所有登录用户均可下载文件
    if (system_role == UserManager::Role::ADMIN) {
        return true;
    }
    return false;
}

}
}
