#include "resource_manager.h"
#include <chen/config/config.h>
#include <chen/util/util.h>
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();
static chen::ConfigVar<std::string>::ptr server_work_path = chen::Config::Lookup<std::string>("server.work_path");

static const size_t kCacheMaxSize = 500;

ResourceManager::ResourceManager()
    :m_cache(kCacheMaxSize, 0, nullptr) {
}

ResourceManager::ResourceType ResourceManager::GetResourceType(const std::string& filename) {
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

data::ResourceInfo::ptr ResourceManager::parseRow(chen::ISQLData::ptr rt) {
    data::ResourceInfo::ptr v(new data::ResourceInfo);
    v->setId(rt->getInt64(0));
    v->setName(rt->getString(1));
    v->setPath(rt->getString(2));
    v->setType(rt->getInt32(3));
    v->setSize(rt->getInt64(4));
    v->setHash(rt->getString(5));
    v->setOwnerId(rt->getInt64(6));
    v->setBizType(rt->getString(7));
    v->setBizId(rt->getInt64(8));
    v->setStatus(rt->getInt32(9));
    v->setIsDeleted(rt->getInt32(10));
    v->setCreateTime(rt->getTime(11));
    v->setUpdateTime(rt->getTime(12));
    return v;
}

bool ResourceManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }
    INFO(logger) << "ResourceManager loadAll: DB connection verified, no preloading needed";
    return true;
}

void ResourceManager::add(blog::data::ResourceInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

data::ResourceInfo::ptr ResourceManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::ResourceInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

void ResourceManager::getByBizUid(std::vector<data::ResourceInfo::ptr>& results
        , const std::string& biz_type, int64_t biz_id, int64_t uid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return;
    }
    auto qb = chen::QueryBuilder::Create("resource");
    qb->where("biz_type", "=", biz_type);
    qb->where("biz_id", "=", biz_id);
    qb->where("owner_id", "=", uid);
    qb->where("is_deleted", "=", (int64_t)0);
    qb->orderBy("id", "DESC");
    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (!rt) {
        return;
    }
    while (rt->next()) {
        results.push_back(parseRow(rt));
    }
}

data::ResourceInfo::ptr ResourceManager::getByBizUidName(const std::string& biz_type
        , int64_t biz_id, int64_t uid, const std::string& filename) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    auto qb = chen::QueryBuilder::Create("resource");
    qb->where("biz_type", "=", biz_type);
    qb->where("biz_id", "=", biz_id);
    qb->where("owner_id", "=", uid);
    qb->where("name", "=", filename);
    qb->limit(1);
    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return nullptr;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (rt && rt->next()) {
        return parseRow(rt);
    }
    return nullptr;
}

data::ResourceInfo::ptr ResourceManager::getByPath(const std::string& path) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    auto qb = chen::QueryBuilder::Create("resource");
    qb->where("path", "=", path);
    qb->limit(1);
    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return nullptr;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (rt && rt->next()) {
        return parseRow(rt);
    }
    return nullptr;
}

}
