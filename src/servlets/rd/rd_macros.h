#ifndef __BLOG_SERVLETS_RD_RD_MACROS_H__
#define __BLOG_SERVLETS_RD_RD_MACROS_H__

#include "rd_helper.h" // IWYU pragma: keep

// ============================================================================
// RD servlet 参数获取宏
// 优先从 JSON body 读取，body 为空或 key 不存在时回退到 query params
// 依赖作用域内存在 body (Json::Value) 和 request (HttpRequest::ptr)
// ============================================================================

/// 获取 string 参数（body 优先，query 回退）
#define RD_GET_PARAM_STRING(var, key)                              \
    std::string var;                                               \
    do {                                                           \
        if (!body.isNull() && body.isMember(key)) {                \
            if (body[key].isString()) {                            \
                var = body[key].asString();                        \
            } else {                                               \
                var = std::to_string(rd::getJsonInt64(body, key)); \
            }                                                      \
        } else {                                                   \
            var = request->getParam(key);                          \
        }                                                          \
    } while (0)

/// 获取 int64 参数（body 优先，query 回退）
#define RD_GET_PARAM_INT64(var, key)                    \
    int64_t var = 0;                                    \
    do {                                                \
        if (!body.isNull() && body.isMember(key)) {     \
            var = rd::getJsonInt64(body, key);          \
        } else {                                        \
            var = request->getParamAs<int64_t>(key, 0); \
        }                                               \
    } while (0)

/// 获取必填 string 参数，缺失时设置 400 并 break
#define RD_REQUIRE_PARAM_STRING(result, var, key)            \
    RD_GET_PARAM_STRING(var, key);                           \
    if (var.empty()) {                                       \
        result->setResult(400, "param " key " is required"); \
        break;                                               \
    }

/// 获取必填 int64 参数，缺失时设置 400 并 break
#define RD_REQUIRE_PARAM_INT64(result, var, key)             \
    RD_GET_PARAM_INT64(var, key);                            \
    if (!var) {                                              \
        result->setResult(400, "param " key " is required"); \
        break;                                               \
    }

// ============================================================================
// 直观版参数获取宏（推荐使用）
// 直接声明变量并赋值，一行搞定。底层调用 rd_helper 中的同名函数。
// 同样需要作用域内存在 body (Json::Value) 和 request (HttpRequest::ptr)
// ============================================================================

/// 声明 string 变量并从 body/query 获取值
/// 用法: RD_PARAM_STR(title, "title")
#define RD_PARAM_STR(var, key) std::string var = rd::getParamString(body, request, key)

/// 声明 int64_t 变量并从 body/query 获取值
/// 用法: RD_PARAM_INT(assignee_id, "assignee_id")
#define RD_PARAM_INT(var, key) int64_t var = rd::getParamInt64(body, request, key)

/// 声明必填 string 变量，值为空时设置 400 并 break
/// 用法: RD_PARAM_STR_REQ(result, title, "title")
#define RD_PARAM_STR_REQ(result, var, key)                    \
    std::string var = rd::getParamString(body, request, key); \
    if (var.empty()) {                                        \
        result->setResult(400, "param " key " is required");  \
        break;                                                \
    }

/// 声明必填 int64_t 变量，值为零时设置 400 并 break
/// 用法: RD_PARAM_INT_REQ(result, org_id, "org_id")
#define RD_PARAM_INT_REQ(result, var, key)                   \
    int64_t var = rd::getParamInt64(body, request, key);     \
    if (!var) {                                              \
        result->setResult(400, "param " key " is required"); \
        break;                                               \
    }

#endif // __BLOG_SERVLETS_RD_RD_MACROS_H__
