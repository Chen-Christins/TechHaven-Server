#include "resource_manager.h"
#include <chen/config/config.h>
#include <chen/util/util.h>
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();
static chen::ConfigVar<std::string>::ptr server_work_path = chen::Config::Lookup<std::string>("server.work_path");

ResourceManager::ResourceType ResourceManager::GetResourceType(const std::string& filename) {
    // 根据文件扩展名判断资源类型
    auto pos = filename.rfind('.');
    if (pos == std::string::npos) {
        return TYPE_OTHER;
    }
    std::string ext = filename.substr(pos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "bmp" || ext == "gif" || ext == "webp") {
        return TYPE_IMAGE;
    } else if (ext == "mp4" || ext == "avi" || ext == "mov" || ext == "wmv" || ext == "flv" || ext == "mkv") {
        return TYPE_VIDEO;
    } else if (ext == "pdf" || ext == "doc" || ext == "docx" || ext == "xls" || ext == "xlsx" || ext == "ppt" ||
               ext == "pptx" || ext == "txt" || ext == "md") {
        return TYPE_DOCUMENT;
    } else if (ext == "zip" || ext == "rar" || ext == "7z" || ext == "tar" || ext == "gz") {
        return TYPE_COMPRESSED;
    } else if (ext == "mp3" || ext == "wav" || ext == "aac" || ext == "flac" || ext == "ogg" || ext == "m4a") {
        return TYPE_AUDIO;
    } else {
        return TYPE_OTHER;
    }
}

bool ResourceManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<data::ResourceInfo::ptr> results;
    if (blog::data::ResourceInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "ResourceManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, blog::data::ResourceInfo::ptr> datas;
    for (auto& i : results) {
        datas[i->getId()] = i;
    }
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);

    return true;
}

void ResourceManager::add(blog::data::ResourceInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    // 添加资源信息
    m_datas[info->getId()] = info;
}

}