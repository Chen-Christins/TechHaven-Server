#include "backup_create_servlet.h"

#include <chen/log/log.h>
#include <chen/config/config.h>
#include <chen/util/util.h>
#include <chen/iomanager/worker.h>

#include "../../manager/user_manager.h"
#include "../../manager/backup_record_manager.h"
#include "../../util.h"

#include <sys/stat.h>

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

static chen::ConfigVar<std::string>::ptr server_work_path = chen::Config::Lookup<std::string>("server.work_path");
static chen::ConfigVar<std::map<std::string, std::map<std::string, std::string>>>::ptr g_mysql_dbs =
    chen::Config::Lookup("mysql.dbs", std::map<std::string, std::map<std::string, std::string>>(), "mysql dbs");

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

        int64_t backup_id = info->getId();
        std::string work_path = server_work_path->getValue();
        auto mysql_dbs = g_mysql_dbs->getValue();

        chen::IOManager::GetThis()->schedule([backup_id, type, name, mysql_dbs, work_path]() {
            if (mysql_dbs.empty()) {
                ERROR(logger) << "mysql config not found for backup " << backup_id;
                auto info = BackupRecordMgr::GetInstance()->get(backup_id);
                if (info) {
                    info->setStatus("failed");
                    info->setDescription("mysql config not found");
                    data::BackupRecordInfoDao::Update(info, GetDB());
                }
                return;
            }
            auto& dbcfg = mysql_dbs.begin()->second;
            std::string host = dbcfg.at("host");
            std::string port = dbcfg.at("port");
            std::string user = dbcfg.at("user");
            std::string passwd = dbcfg.at("passwd");
            std::string dbname = dbcfg.at("dbname");

            std::string backup_dir = work_path + "/backups";
            mkdir(backup_dir.c_str(), 0755);

            std::string filename = name + ".sql.gz";
            std::string filepath = backup_dir + "/" + filename;

            int64_t t0 = time(0);
            std::string cmd = "mysqldump -h " + host + " -P " + port + " -u " + user
                            + " -p'" + passwd + "' " + dbname + " 2>/dev/null | gzip > " + filepath;

            INFO(logger) << "Backup " << backup_id << " starting, cmd=" << cmd;
            int rc = std::system(cmd.c_str());

            auto info = BackupRecordMgr::GetInstance()->get(backup_id);
            if (!info) {
                ERROR(logger) << "Backup record " << backup_id << " not found after execution";
                return;
            }

            if (rc != 0) {
                ERROR(logger) << "Backup " << backup_id << " failed, rc=" << rc;
                info->setStatus("failed");
                info->setDescription("mysqldump failed with exit code " + std::to_string(rc));
                data::BackupRecordInfoDao::Update(info, GetDB());
                return;
            }

            struct stat st;
            int64_t fileSize = 0;
            if (stat(filepath.c_str(), &st) == 0) {
                fileSize = st.st_size;
            }
            int64_t elapsed = time(0) - t0;

            info->setStatus("completed");
            info->setSize(fileSize);
            info->setFileCount(1);
            info->setFilePath("backups/" + filename);
            info->setCompletedAt(time(0));
            info->setDescription("completed in " + std::to_string(elapsed) + "s, size "
                                  + std::to_string(fileSize) + " bytes");
            data::BackupRecordInfoDao::Update(info, GetDB());

            INFO(logger) << "Backup " << backup_id << " completed: " << filepath
                << " (" << fileSize << " bytes, " << elapsed << "s)";
        });

        Json::Value item;
        item["id"] = info->getId();
        item["name"] = info->getName();
        item["type"] = info->getType();
        item["size"] = info->getSize();
        item["file_count"] = info->getFileCount();
        item["status"] = info->getStatus();
        item["created_at"] = info->getCreateTime();
        auto created_by_user = UserMgr::GetInstance()->get(info->getCreatedBy());
        if (created_by_user) {
            item["created_by"] = created_by_user->getName();
        }
        item["description"] = info->getDescription();

        result->set("data", item);
        result->setErrno(errcode::SUCCESS);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
