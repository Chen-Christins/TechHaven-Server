/**
 * @file cache_util.h
 * @brief Redis 缓存工具（COUNT 缓存、列表结果缓存）
 * @author Christins
 * @date 2026-06-06
 * @copyright Apache 2.0
 */
#pragma once

#include <chen/db/query_builder.h>
#include <chen/db/redis.h>
#include <chen/util/util.h>

#include <sstream>

namespace blog {

/// COUNT 缓存 TTL（秒）
static const int kCountCacheTTL = 30;
/// 列表结果缓存 TTL（秒）
static const int kListCacheTTL = 10;
/// Stats 缓存 TTL（秒）
static const int kStatsCacheTTL = 60;
/// ID 映射缓存 TTL（秒）— alternate_key → id
static const int kIdMapCacheTTL = 300;

/**
 * @brief 执行 COUNT 查询，优先从 Redis 缓存读取
 * @param qb QueryBuilder（已配置好 WHERE/JOIN）
 * @param conn 数据库连接
 * @param cacheKey 缓存键（不含前缀）
 * @param ttlSec 缓存 TTL 秒数
 * @return COUNT 结果，失败返回 0
 */
inline int64_t executeCountCached(chen::QueryBuilder::ptr qb, chen::IDB::ptr conn
        , const std::string& cacheKey, int ttlSec = kCountCacheTTL) {
    std::string redisKey = "cache:count:" + cacheKey;
    auto rpy = chen::RedisUtil::Cmd("blog", "get %s", redisKey.c_str());
    if (rpy && rpy->str) {
        return chen::TypeUtil::Atoi(rpy->str);
    }
    int64_t total = 0;
    if (qb->executeCount(total, conn)) {
        return 0;
    }
    chen::RedisUtil::Cmd("blog", "setex %s %d %lld", redisKey.c_str(), ttlSec, total);
    return total;
}

/**
 * @brief 缓存分页列表结果（ID 数组）
 * @param cacheKey 缓存键
 * @param ids 文章/评论 ID 列表
 * @param ttlSec TTL 秒数
 */
inline void cacheListResult(const std::string& cacheKey, const std::vector<int64_t>& ids, int ttlSec = kListCacheTTL) {
    if (ids.empty()) {
        return;
    }
    std::stringstream ss;
    for (size_t i = 0; i < ids.size(); ++i) {
        if (i) {
            ss << ",";
        }
        ss << ids[i];
    }
    std::string redisKey = "cache:list:" + cacheKey;
    chen::RedisUtil::Cmd("blog", "setex %s %d %s", redisKey.c_str(), ttlSec, ss.str().c_str());
}

/**
 * @brief 读取缓存的分页列表结果
 * @param cacheKey 缓存键
 * @param[out] ids 解析后的 ID 列表
 * @return true 表示命中缓存
 */
inline bool getCachedListResult(const std::string& cacheKey, std::vector<int64_t>& ids) {
    std::string redisKey = "cache:list:" + cacheKey;
    auto rpy = chen::RedisUtil::Cmd("blog", "get %s", redisKey.c_str());
    if (!rpy || !rpy->str) {
        return false;
    }
    std::string data(rpy->str);
    size_t pos = 0;
    while (pos < data.size()) {
        size_t end = data.find(',', pos);
        if (end == std::string::npos) {
            end = data.size();
        }
        ids.push_back(std::stoll(data.substr(pos, end - pos)));
        pos = end + 1;
    }
    return true;
}

/**
 * @brief 缓存字符串结果
 * @param cacheKey 缓存键
 * @param value 字符串值
 * @param ttlSec TTL 秒数
 */
inline void cacheStringResult(const std::string& cacheKey, const std::string& value, int ttlSec = kStatsCacheTTL) {
    std::string redisKey = "cache:str:" + cacheKey;
    chen::RedisUtil::Cmd("blog", "setex %s %d %s", redisKey.c_str(), ttlSec, value.c_str());
}

/**
 * @brief 读取缓存的字符串结果
 * @param cacheKey 缓存键
 * @param[out] value 缓存的字符串
 * @return true 表示命中缓存
 */
inline bool getCachedStringResult(const std::string& cacheKey, std::string& value) {
    std::string redisKey = "cache:str:" + cacheKey;
    auto rpy = chen::RedisUtil::Cmd("blog", "get %s", redisKey.c_str());
    if (!rpy || !rpy->str) {
        return false;
    }
    value = rpy->str;
    return true;
}

/**
 * @brief 缓存 alternate_key → id 映射
 * @param cacheKey 缓存键（不含前缀）
 * @param id 主键 ID
 * @param ttlSec TTL 秒数
 */
inline void cacheIdMapping(const std::string& cacheKey, int64_t id, int ttlSec = kIdMapCacheTTL) {
    std::string redisKey = "cache:map:" + cacheKey;
    chen::RedisUtil::Cmd("blog", "setex %s %d %lld", redisKey.c_str(), ttlSec, id);
}

/**
 * @brief 读取 alternate_key → id 映射
 * @param cacheKey 缓存键（不含前缀）
 * @return ID，未命中返回 0
 */
inline int64_t getCachedIdMapping(const std::string& cacheKey) {
    std::string redisKey = "cache:map:" + cacheKey;
    auto rpy = chen::RedisUtil::Cmd("blog", "get %s", redisKey.c_str());
    if (rpy && rpy->str) {
        return chen::TypeUtil::Atoi(rpy->str);
    }
    return 0;
}

} // namespace blog
