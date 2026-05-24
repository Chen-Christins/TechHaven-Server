#include "system_settings_upload_servlet.h"
#include "../../manager/system_settings_manager.h"
#include "../../manager/user_manager.h"
#include <chen/log/log.h>
#include <chen/config/config.h>
#include <chen/parser/multi_part_parser.h>
#include <chen/util/util.h>

namespace blog::servlet {

static chen::Logger::ptr logger = LOG_ROOT();
static chen::ConfigVar<std::string>::ptr server_work_path = chen::Config::Lookup<std::string>("server.work_path");

SystemSettingsUploadServlet::SystemSettingsUploadServlet()
    : BlogLoginedServlet("SystemSettingsUploadServlet") {
}

int32_t SystemSettingsUploadServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }
        auto user = UserMgr::GetInstance()->get(uid);
        if (!user || user->getRole() != UserManager::Role::ADMIN) {
            result->setResult(403, "Access Denied");
            break;
        }

        std::string content_type = request->getHeader("content-type");
        chen::MultipartParser::ptr parser = std::make_shared<chen::MultipartParser>(content_type);
        auto data = parser->parseToMemory(request->getBody());

        if (data.size() < 2) {
            result->setResult(400, "protocol error: need [type, file]");
            break;
        }

        std::string type = data[0].content;
        if (type != "siteIcon" && type != "siteLogo" && type != "favicon") {
            result->setResult(400, "invalid type, must be siteIcon/siteLogo/favicon");
            break;
        }

        if (data[1].content.empty()) {
            result->setResult(400, "file content is empty");
            break;
        }

        std::string save_dir = server_work_path->getValue() + "/uploads/settings";
        std::string filename = type + "_" + std::to_string(time(0));
        if (!data[1].filename.empty()) {
            std::string ext;
            auto pos = data[1].filename.rfind('.');
            if (pos != std::string::npos) {
                ext = data[1].filename.substr(pos);
            }
            filename += ext;
        }

        std::string full_path = save_dir + "/" + filename;
        std::ofstream ofs;
        if (!chen::FSUtil::OpenForWrite(ofs, full_path, std::ios::binary)) {
            ERROR(logger) << "Open file for write failed: " << full_path;
            result->setResult(500, "save file fail");
            break;
        }
        ofs.write(data[1].content.c_str(), data[1].content.size());
        ofs.close();

        std::string url = "/uploads/settings/" + filename;
        INFO(logger) << "Settings image uploaded: " << url
                     << " (Size: " << data[1].content.size() << " bytes)";

        auto settings = SystemSettingsMgr::GetInstance()->get();
        if (!settings) {
            result->setResult(500, "system settings not loaded");
            break;
        }

        if (type == "siteIcon") {
            settings->setSiteIcon(url);
        } else if (type == "siteLogo") {
            settings->setSiteLogo(url);
        } else if (type == "favicon") {
            settings->setFavicon(url);
        }

        if (!SystemSettingsMgr::GetInstance()->update(settings)) {
            result->setResult(500, "save settings fail");
            break;
        }

        result->jsondata["url"] = url;
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

} // namespace blog::servlet
