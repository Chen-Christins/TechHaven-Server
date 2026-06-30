/**
 * @file session_manager.h
 * @brief 会话 Redis 持久化工具 — 内存 + Redis 双存储，支持进程重启后自动恢复
 * @author Christins
 * @date 2026-06-30
 * @copyright Apache 2.0
 */
#pragma once

#include <chen/db/redis.h>
#include <chen/http/session_data.h>
#include <chen/log/log.h>

#include <memory>
#include <string>

#include "system_settings_manager.h"

namespace blog {

/// Redis session key 前缀
static const char* kSessionKeyPrefix = "session:";
/// 默认 session TTL（24 小时 = 86400 秒）
static const int64_t kDefaultSessionTTL = 24 * 3600;

/**
 * @brief 获取 session TTL（秒），优先读取系统设置，默认 24 小时
 */
inline int64_t GetSessionTTL() {
    auto settings = SystemSettingsMgr::GetInstance()->get();
    if (settings && settings->getSessionTimeout() > 0) {
        return static_cast<int64_t>(settings->getSessionTimeout()) * 3600;
    }
    return kDefaultSessionTTL;
}

/**
 * @brief 保存会话到 Redis Hash，并设置 TTL
 * @param session_id 会话 ID
 * @param uid 用户 ID
 * @param is_auth 是否已尝试验证
 */
inline void SaveSessionToRedis(const std::string& session_id, int64_t uid, int32_t is_auth) {
    std::string key = std::string(kSessionKeyPrefix) + session_id;
    int64_t ttl = GetSessionTTL();
    chen::RedisUtil::Cmd("blog", "hset %s user_id %lld", key.c_str(), (long long)uid);
    chen::RedisUtil::Cmd("blog", "hset %s is_auth %d", key.c_str(), is_auth);
    chen::RedisUtil::Cmd("blog", "expire %s %lld", key.c_str(), (long long)ttl);
}

/**
 * @brief 从 Redis Hash 恢复会话到内存
 * @param session_id 会话 ID
 * @return 恢复的 SessionData 指针，Redis 中无数据或 uid=0 时返回 nullptr
 * @details 恢复成功后自动注册到 SessionDataMgr 并刷新 Redis TTL
 */
inline chen::http::SessionData::ptr LoadSessionFromRedis(const std::string& session_id) {
    std::string key = std::string(kSessionKeyPrefix) + session_id;
    auto rpy = chen::RedisUtil::Cmd("blog", "hgetall %s", key.c_str());
    if (!rpy || rpy->type != REDIS_REPLY_ARRAY || rpy->elements == 0) {
        return nullptr;
    }

    int64_t uid = 0;
    int32_t is_auth = 0;

    for (size_t i = 0; i + 1 < rpy->elements; i += 2) {
        std::string field(rpy->element[i]->str, rpy->element[i]->len);
        std::string value(rpy->element[i + 1]->str, rpy->element[i + 1]->len);
        if (field == "user_id") {
            uid = std::stoll(value);
        } else if (field == "is_auth") {
            is_auth = std::stoi(value);
        }
    }

    if (!uid) {
        return nullptr;
    }

    auto data = std::make_shared<chen::http::SessionData>(false);
    data->setId(session_id);
    data->setData("S_UID", uid);
    data->setData("IS_AUTH", is_auth);

    // 注册回内存 SessionDataMgr
    chen::http::SessionDataMgr::GetInstance()->add(data);

    // 刷新 Redis TTL
    int64_t ttl = GetSessionTTL();
    chen::RedisUtil::Cmd("blog", "expire %s %lld", key.c_str(), (long long)ttl);

    return data;
}

/**
 * @brief 从 Redis 删除会话数据
 * @param session_id 会话 ID
 */
inline void DeleteSessionFromRedis(const std::string& session_id) {
    std::string key = std::string(kSessionKeyPrefix) + session_id;
    chen::RedisUtil::Cmd("blog", "del %s", key.c_str());
}

}  // namespace blog
