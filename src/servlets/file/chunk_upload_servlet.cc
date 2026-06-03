#include "chunk_upload_servlet.h"
#include <chen/log/log.h>
#include <chen/config/config.h>
#include <chen/util/util.h>
#include <fstream>
#include <vector>
#include <cstdio>
#include <sstream>
#include "../../chunk_upload.h"
#include "../../manager/resource_manager.h"
#include "../../manager/user_manager.h"
#include "../../manager/assignment_manager.h"
#include "../../manager/assignment_user_rel_manager.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();
static chen::ConfigVar<std::string>::ptr server_work_path =
    chen::Config::Lookup<std::string>("server.work_path");

ChunkUploadServlet::ChunkUploadServlet()
    : BlogLoginedServlet("ChunkUploadServlet") {
}

int32_t ChunkUploadServlet::handle(chen::http::HttpRequest::ptr request
        , chen::http::HttpResponse::ptr response, chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        auto path = request->getPath();
        if(path == "/upload/init") {
            return handleInit(request, response, session, result);
        } else if(path == "/upload/complete") {
            return handleComplete(request, response, session, result);
        } else if(path == "/upload/cancel") {
            return handleCancel(request, response, session, result);
        } else if(path == "/upload/chunk") {
            return handleUpload(request, response, session, result);
        } else if (path == "/upload/status") {
            return handleStatus(request, response, session, result);
        } else {
            result->setResult(404, "Not Found");
            break;
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

void ChunkUploadServlet::parseBizInfo(const std::string& biz_info
        , std::string& biz_type,  std::string& biz_id) {
    biz_type = "";
    biz_id = "";
    do {
        auto pos = biz_info.find("|");
        if (pos == std::string::npos) {
            break;
        }
        biz_type = biz_info.substr(0, pos);
        biz_id = biz_info.substr(pos + 1);
    } while (0);
}

int32_t ChunkUploadServlet::handleInit(chen::http::HttpRequest::ptr request
        , chen::http::HttpResponse::ptr response, chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        std::string type = request->getHeader("X-Upload-Type");
        std::string file_name = chen::URLDecode(request->getHeader("X-Upload-Filename"));
        std::string dir_name = chen::URLDecode(request->getHeader("X-Upload-Dir-Name"));
        std::string biz_info = chen::URLDecode(request->getHeader("X-Upload-Biz-Info"));
        uint64_t total_size = std::stoull(request->getHeader("X-Upload-Total-Size"));
        uint64_t total_chunks = std::stoull(request->getHeader("X-Upload-Total-Chunks"));
        uint32_t chunk_size = std::stoul(request->getHeader("X-Upload-Chunk-Size"));

        if (type != "chunked") {
            result->setResult(400, "Invalid upload type");
            break;
        }
        if (file_name.empty() || dir_name.empty() || biz_info.empty()
                || total_size == 0 || total_chunks == 0 || chunk_size == 0) {
            result->setResult(400, "Invalid params");
            break;
        }

        std::string biz_type, biz_id;
        parseBizInfo(biz_info, biz_type, biz_id);

        int64_t uid = getUserId(request);

        // 生成 uploadId
        time_t now = time(0);
        std::string upload_id = chen::md5(std::to_string(now) + "|"
            + file_name + "|" + std::to_string(uid) + chen::random_string(6));

        auto session = ChunkUploadMgr::GetInstance()->createSession(upload_id, file_name
            , total_size, chunk_size, total_chunks, biz_type, std::stoll(biz_id), dir_name);

        result->set("upload_id", upload_id);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

int32_t ChunkUploadServlet::handleComplete(chen::http::HttpRequest::ptr request
        , chen::http::HttpResponse::ptr response, chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        std::string type = request->getHeader("X-Upload-Type");
        std::string upload_id = chen::URLDecode(request->getHeader("X-Upload-Id"));

        if (type != "chunked") {
            result->setResult(400, "Invalid upload type");
            break;
        }
        if (upload_id.empty()) {
            result->setResult(400, "Invalid params");
            break;
        }

        auto upload_session = ChunkUploadMgr::GetInstance()->getSession(upload_id);
        if (!upload_session) {
            result->setResult(404, "Upload session not found");
            break;
        }

        // 检查是否所有分块都已接收
        {
            std::lock_guard<std::mutex> lock(upload_session->m_mtx);
            if (upload_session->received_count != upload_session->total_chunks) {
                result->setResult(400, "Not all chunks uploaded");
                break;
            }
            upload_session->completed = true;
        }

        int64_t uid = getUserId(request);
        std::string subject_name = AssignmentMgr::GetInstance()->get(upload_session->biz_id)->getSubjectName();
        std::string user_name = UserMgr::GetInstance()->get(uid)->getName();
        std::string dir_name = upload_session->biz_type + "/" + subject_name +
            "/" + upload_session->dir_name + "/" + user_name;
        std::string save_dir = server_work_path->getValue() + "/uploads/" + dir_name;

        INFO(logger) << "File upload save dir: " << save_dir;

        std::string filename = save_dir + "/" + upload_session->file_name;
        std::ofstream ofs;
        bool rt = chen::FSUtil::OpenForWrite(ofs, filename, std::ios::binary);

        if (rt) {
            for (size_t i = 0; i < upload_session->total_chunks; ++i) {
                // 从临时文件读取并写入最终文件
                std::ifstream temp_file(upload_session->chunk_data[i], std::ios::binary);
                if (!temp_file.is_open()) {
                    ERROR(logger) << "Failed to open temp file: " << upload_session->chunk_data[i];
                    result->setResult(500, "Failed to assemble file");
                    ofs.close();
                    break;
                }

                // 使用缓冲区避免内存溢出
                std::vector<char> buffer(8192);  // 8KB buffer
                while (temp_file.read(buffer.data(), buffer.size())) {
                    std::streamsize bytes_read = temp_file.gcount();
                    ofs.write(buffer.data(), bytes_read);
                }
                // 处理最后剩余的数据
                std::streamsize bytes_read = temp_file.gcount();
                if (bytes_read > 0) {
                    ofs.write(buffer.data(), bytes_read);
                }

                temp_file.close();

                // 删除临时文件
                std::remove(upload_session->chunk_data[i].c_str());
            }
        }
        ofs.close();

        INFO(logger) << "File saved: " << filename << " (Size: " << upload_session->total_size
                << " bytes -- " << (1.0 * upload_session->total_size / 1024)
                << " kb -- " << (1.0 * upload_session->total_size / (1024 * 1024)) << " mb)";

        // 计算文件MD5，避免内存问题
        std::ifstream final_file(filename, std::ios::binary);
        std::string hash_key;
        if (final_file.is_open()) {
            std::stringstream file_content;
            file_content << final_file.rdbuf();
            hash_key = chen::md5(file_content.str());
            final_file.close();
        } else {
            ERROR(logger) << "Failed to open final file for hash calculation: " << filename;
            result->setResult(500, "Failed to calculate file hash");
            break;
        }
        std::string path = "/uploads/" + dir_name + "/" + upload_session->file_name;
        if (!dumpToResource(upload_session->biz_type, upload_session->biz_id, path, hash_key, uid, upload_session->total_size)) {
            result->setResult(500, "Dump to resource fail");
            break;
        }

        time_t now = time(0);
        auto db = getDB();
        if (!db) {
            ERROR(logger) << "Get SQLite3 connection fail";
            result->setResult(500, "Get DB connection fail");
            break;
        }
        // 查找是否已存在该用户该作业的记录
        auto old_info = AssignmentUserRelMgr::GetInstance()->getByAssignAndUser(upload_session->biz_id, uid);
        auto info = std::make_shared<data::AssignmentUserRelInfo>();
        info->setAssignmentId(upload_session->biz_id);
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

        result->set("file_path", path);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

int32_t ChunkUploadServlet::handleCancel(chen::http::HttpRequest::ptr request
        , chen::http::HttpResponse::ptr response, chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        std::string type = request->getHeader("X-Upload-Type");
        std::string upload_id = chen::URLDecode(request->getHeader("X-Upload-Id"));

        if (type != "chunked") {
            result->setResult(400, "Invalid upload type");
            break;
        }
        if (upload_id.empty()) {
            result->setResult(400, "Invalid params");
            break;
        }

        auto upload_session = ChunkUploadMgr::GetInstance()->getSession(upload_id);
        if (!upload_session) {
            result->setResult(404, "Upload session not found");
            break;
        }

        // 清理临时文件
        std::lock_guard<std::mutex> lock(upload_session->m_mtx);
        for (size_t i = 0; i < upload_session->total_chunks; ++i) {
            if (upload_session->received_chunks[i] && !upload_session->chunk_data[i].empty()) {
                std::remove(upload_session->chunk_data[i].c_str());
            }
        }

        ChunkUploadMgr::GetInstance()->removeSession(upload_id);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

int32_t ChunkUploadServlet::handleUpload(chen::http::HttpRequest::ptr request
        , chen::http::HttpResponse::ptr response, chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        std::string type = request->getHeader("X-Upload-Type");
        std::string upload_id = chen::URLDecode(request->getHeader("X-Upload-Id"));
        uint64_t chunk_index = std::stoul(request->getHeader("X-Upload-Chunk-Index"));
        // uint64_t offset = std::stoull(request->getHeader("X-Upload-Offset"));
        uint64_t chunk_size = std::stoul(request->getHeader("X-Upload-Chunk-Size"));
        // 数据部分
        const std::string& body = request->getBody();

        if (type != "chunked") {
            result->setResult(400, "Invalid upload type");
            break;
        }
        if (upload_id.empty() || chunk_size == 0 || body.size() == 0) {
            result->setResult(400, "Invalid params");
            break;
        }

        auto upload_session = ChunkUploadMgr::GetInstance()->getSession(upload_id);
        if (!upload_session) {
            result->setResult(404, "Upload session not found");
            break;
        }

        std::lock_guard<std::mutex> lock(upload_session->m_mtx);
        if (chunk_index >= upload_session->total_chunks) {
            result->setResult(400, "Invalid chunk index");
            break;
        }
        if (upload_session->received_chunks[chunk_index]) {
            result->setResult(400, "Chunk already received");
            break;
        }
        // 确保临时目录存在
        std::string temp_dir = server_work_path->getValue() + "/temp";
        if (!chen::FSUtil::Mkdir(temp_dir)) {
            ERROR(logger) << "Failed to create temp directory: " << temp_dir;
            result->setResult(500, "Failed to create temp directory");
            break;
        }

        // 保存分块数据到临时文件而不是内存
        std::string temp_file_path = temp_dir + "/" + upload_id + "_" + std::to_string(chunk_index);
        std::ofstream temp_file(temp_file_path, std::ios::binary);
        if (!temp_file.is_open()) {
            result->setResult(500, "Failed to create temp file");
            break;
        }
        temp_file.write(body.c_str(), body.size());
        temp_file.close();

        // 只保存文件路径，不保存数据内容
        upload_session->chunk_data[chunk_index] = temp_file_path;
        upload_session->received_chunks[chunk_index] = true;
        upload_session->received_count++;
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

int32_t ChunkUploadServlet::handleStatus(chen::http::HttpRequest::ptr request
        , chen::http::HttpResponse::ptr response, chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        std::string type = request->getHeader("X-Upload-Type");
        std::string upload_id = chen::URLDecode(request->getHeader("X-Upload-Id"));

        if (type != "chunked") {
            result->setResult(400, "Invalid upload type");
            break;
        }
        if (upload_id.empty()) {
            result->setResult(400, "Invalid params");
            break;
        }

        auto upload_session = ChunkUploadMgr::GetInstance()->getSession(upload_id);
        if (!upload_session) {
            result->setResult(404, "Upload session not found");
            break;
        }

        // FIXME: 这里返回的信息可以更详细一些，比如哪些分块已接收，哪些未接收
        result->set("upload_id", upload_session->upload_id);
        result->set("file_name", upload_session->file_name);
        result->set("total_size", upload_session->total_size);
        result->set("total_chunks", upload_session->total_chunks);
        result->set("received_chunks", upload_session->received_count);
        result->set("completed", upload_session->completed);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

bool ChunkUploadServlet::dumpToResource(const std::string& biz_type, int64_t biz_id
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
