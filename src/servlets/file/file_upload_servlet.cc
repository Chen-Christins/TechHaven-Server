#include "file_upload_servlet.h"
#include <chen/log/log.h>
#include <chen/parser/multi_part_parser.h>
#include <chen/config/config.h>
#include <chen/util/util.h>
#include "../../manager/user_manager.h"
#include "../../manager/assignment_user_rel_manager.h"
#include "../../manager/resource_manager.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

static chen::ConfigVar<std::string>::ptr server_work_path = chen::Config::Lookup<std::string>("server.work_path");

FileUploadServlet::FileUploadServlet() 
	: BlogLoginedServlet("FileUploadServlet") {
}

int32_t FileUploadServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
		, chen::http::HttpSession::ptr session, Result::ptr result) {
	do {
		std::string content_type = request->getHeader("content-type");
		chen::MultipartParser::ptr parser = std::make_shared<chen::MultipartParser>(content_type);
		
		auto data = parser->parseToMemory(request->getBody());
		
        // [0]:dir_name  [1]:biz_type|biz_id  [2...]:files
		if (data.size() < 3) {
			result->setResult(400, "protocol error");
			break;
		}

        auto biz_info = chen::split(data[1].content, "|");
        if (biz_info.size() != 2) {
            result->setResult(400, "protocol error");
            break;
        }

        int64_t uid = getUserId(request);
        std::string user_name = UserMgr::GetInstance()->get(uid)->getName();
		std::string dir_name = data[0].content + "/" + user_name;
		std::string save_dir = server_work_path->getValue() + "/uploads/" + dir_name;

        std::string biz_type = biz_info[0];
        int64_t biz_id = chen::TypeUtil::Atoi(biz_info[1]);
		for (size_t i = 2; i < data.size(); ++i) {
			std::string filename = save_dir + "/" + data[i].filename;
			std::ofstream ofs;
			bool rt = chen::FSUtil::OpenForWrite(ofs, filename, std::ios::binary);
			if (rt) {
				ofs.write(data[i].content.c_str(), data[i].content.size());
			}
			ofs.close();
			INFO(logger) << "File saved: " << filename << " (Size: " << data[i].content.size()
					<< " bytes -- " << (1.0 * data[i].content.size() / 1024)
					<< " kb -- " << (1.0 * data[i].content.size() / (1024 * 1024)) << " mb)";
            
            // generate hash key
            std::string hash_key = chen::md5(data[i].content);
            size_t size = data[i].content.size();

            std::string path = "/uploads/" + dir_name + "/" + data[i].filename;
            if (!dumpToResource(biz_type, biz_id, path, hash_key, uid, size)) {
                result->setResult(500, "Dump to resource fail");
                break;
            }
		}
        
        time_t now = time(0);
        auto db = getDB();
        if (!db) {
            ERROR(logger) << "Get SQLite3 connection fail";
            result->setResult(500, "Get DB connection fail");
            break;
        }
        // 查找是否已存在该用户该作业的记录
        auto old_info = AssignmentUserRelMgr::GetInstance()->getByAssignAndUser(biz_id, uid);
        auto info = std::make_shared<data::AssignmentUserRelInfo>();
        info->setAssignmentId(biz_id);
        info->setUserId(uid);
        info->setStatus(AssignmentUserRelManager::SUBMITTED);
        info->setSubmitTime(now);
        if (old_info) {
            info->setId(old_info->getId()); // 复用原id，update
            info->setUpdateTime(now);
            if (blog::data::AssignmentUserRelInfoDao::Update(info, db)) {
                ERROR(logger) << "AssignmentUserRelInfo Update fail";
                result->setResult(500, "Database error");
                break;
            }
        } else {
            info->setCreateTime(now);
            if (blog::data::AssignmentUserRelInfoDao::Insert(info, db)) {
                ERROR(logger) << "AssignmentUserRelInfo Insert fail";
                result->setResult(500, "Database error");
                break;
            }
        }
        AssignmentUserRelMgr::GetInstance()->add(info);
	} while (0);
	response->setBody(result->toJsonString());
    return 0;
}

bool FileUploadServlet::dumpToResource(const std::string& biz_type, int64_t biz_id
        , const std::string& path, const std::string& hash_key, int64_t uid, size_t size) {
    auto it = path.rfind("/");
    if (it == std::string::npos) {
        return false;
    }
    std::string filename = path.substr(it + 1);
    ResourceManager::ResourceType type = ResourceMgr::GetInstance()->GetResourceType(filename);
    auto db = getDB();
    if (!db) {
        ERROR(logger) << "Get SQLite3 connection fail";
        return false;
    }
    // 先查找是否已存在同业务同用户同名文件
    auto old_info = ResourceMgr::GetInstance()->getByBizUidName(biz_type, biz_id, uid, filename);
    auto resource_info = std::make_shared<data::ResourceInfo>();
    resource_info->setName(filename);
    resource_info->setPath(path);
    resource_info->setType(type);
    resource_info->setSize(size);
    resource_info->setHash(hash_key);
    resource_info->setOwnerId(uid);
    resource_info->setBizType(biz_type);
    resource_info->setBizId(biz_id);
    resource_info->setStatus(ResourceManager::Status::NORMAL);
    time_t now = time(0);
    resource_info->setCreateTime(now);
    resource_info->setUpdateTime(now);
    if (old_info) {
        resource_info->setId(old_info->getId()); // 复用原id，update
        if (blog::data::ResourceInfoDao::Update(resource_info, db)) {
            ERROR(logger) << "ResourceInfo Update fail";
            return false;
        }
    } else {
        if (blog::data::ResourceInfoDao::Insert(resource_info, db)) {
            ERROR(logger) << "ResourceInfo Insert fail";
            return false;
        }
    }
    ResourceMgr::GetInstance()->add(resource_info);
    return true;
}

} // namespace servlet
} // namespace blog
