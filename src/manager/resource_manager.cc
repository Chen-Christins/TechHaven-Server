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
    std::unordered_map<std::string, blog::data::ResourceInfo::ptr> path_map;
    std::unordered_map<std::string, std::unordered_map<std::string, blog::data::ResourceInfo::ptr>> biz_uid_name_map;
    for (auto& i : results) {
        datas[i->getId()] = i;
        path_map[i->getPath()] = i;
        std::string biz_type = i->getBizType();
        int64_t biz_id = i->getBizId();
        int64_t uid = i->getOwnerId();
        std::string filename = i->getName();
        std::string key = chen::md5(biz_type + "|" + std::to_string(biz_id) + "|" + std::to_string(uid));
        biz_uid_name_map[key][i->getName()] = i;
    }
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_path_map.swap(path_map);
    m_biz_uid_name_map.swap(biz_uid_name_map);

    return true;
}

void ResourceManager::add(blog::data::ResourceInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    // 添加资源信息
    m_datas[info->getId()] = info;
    std::string biz_type = info->getBizType();
    int64_t biz_id = info->getBizId();
    int64_t uid = info->getOwnerId();
    std::string filename = info->getName();
    std::string key = chen::md5(biz_type + "|" + std::to_string(biz_id) + "|" + std::to_string(uid));
    m_biz_uid_name_map[key][filename] = info;
    m_path_map[info->getPath()] = info;
}

data::ResourceInfo::ptr ResourceManager::get(int64_t id) {
    auto it = m_datas.find(id);
    return it == m_datas.end() ? nullptr : it->second;
}

void ResourceManager::getByHash(std::vector<data::ResourceInfo::ptr>& results, const std::string& hash) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);

    for (auto& [f, info] : m_biz_uid_name_map[hash]) {
        results.push_back(info);
    }
}

data::ResourceInfo::ptr ResourceManager::getByBizUidName(const std::string& biz_type
        , int64_t biz_id, int64_t uid, const std::string& filename) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    for (auto& [id, info] : m_datas) {
        if (info->getBizType() == biz_type && info->getBizId() == biz_id
                && info->getOwnerId() == uid && info->getName() == filename) {
            return info;
        }
    }
    return nullptr;
}

data::ResourceInfo::ptr ResourceManager::getByPath(const std::string& path) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_path_map.find(path);
    return it == m_path_map.end() ? nullptr : it->second;
}

} // namespace blog