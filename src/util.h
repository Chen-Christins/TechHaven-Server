/**
 * @file util.h
 * @brief 通用工具函数（邮箱/账号校验、用户ID编解码等）
 * @author Christins
 * @date 2026-06-03
 * @copyright Apache 2.0
 */

#pragma once

#include <chen/db/mysql.h>
#include <chen/util/hash_util.h>
#include <chen/config/config.h>

#include <regex>

namespace blog {

static chen::ConfigVar<std::map<std::string, std::map<std::string, std::string>>>::ptr g_mysql_dbs =
    chen::Config::Lookup("mysql.dbs", std::map<std::string, std::map<std::string, std::string>>(), "mysql dbs");

inline chen::IDB::ptr GetDB() {
    return chen::MySQLMgr::GetInstance()->get(g_mysql_dbs->getValue().begin()->first);
}

inline bool IsEmail(const std::string& str) {
    static const std::regex pattern("([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,8}(\\.[a-z]{2,8})?)");
    return std::regex_match(str, pattern);
}

static const std::string s_uid_secret = "BlogServer!2025$%^UID#@!EncryptKey";

inline std::string EncryptUserId(int64_t uid) {
    std::string plain = std::to_string(uid);
    for (size_t i = 0; i < plain.size(); i++) {
        plain[i] ^= s_uid_secret[i % s_uid_secret.size()];
    }
    return chen::base64encode(plain);
}

inline int64_t DecryptUserId(const std::string& encrypted) {
    if (encrypted.empty()) {
        return 0;
    }
    std::string data = chen::base64decode(encrypted);
    if (data.empty()) {
        return 0;
    }
    for (size_t i = 0; i < data.size(); i++) {
        data[i] ^= s_uid_secret[i % s_uid_secret.size()];
    }
    try {
        return std::stoll(data);
    } catch (...) {
        return 0;
    }
}

inline bool IsValidAccount(const std::string& str) {
    static const std::regex s_account_regex("[A-Za-z][0-9A-Za-z\\-_\\.]{4,15}");
    return std::regex_match(str, s_account_regex);
}

inline void SendWX(const std::string& group, const std::string& msg) {
    // TODO: ...
}

#define DEFINE_AND_CHECK_STRING(result, var, param)         \
    std::string var = request->getParam(param);             \
    if (var.empty()) {                                      \
        result->setResult(400, "param " param " is null" ); \
        break;                                              \
    }

#define DEFINE_AND_CHECK_STRING_WITH_DEFAULT(result, var, param, default_val) \
    std::string var = request->getParam(param, default_val);

#define DEFINE_AND_CHECK_TYPE(result, type, var, param)    \
    type var;                                              \
    if (!request->checkGetParamAs(param, var)) {           \
        result->setResult(400, "param " param " is null"); \
        break;                                             \
    }

}

