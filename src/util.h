#ifndef __BLOG_UTIL_H__
#define __BLOG_UTIL_H__

#include "db/sqlite3.h"
#include <regex>

namespace blog {

inline bool is_email(const std::string& str) {
    static const std::regex pattern("([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,8}(\\.[a-z]{2,8})?)");
    return std::regex_match(str, pattern);
}

inline bool is_vaild_account(const std::string& str) {
    static const std::regex s_account_regex("[A-Za-z][0-9A-Za-z\\-_\\.]{4,15}");
    return std::regex_match(str, s_account_regex);
}

inline sylar::IDB::ptr GetDB() {
    return sylar::SQLite3Mgr::GetInstance()->get("blog");
}

#define DEFINE_AND_CHECK_STRING(result, var, param)         \
    std::string var = request->getParam(param);             \
    if (var.empty()) {                                      \
        result->setResult(400, "param " param " is null" ); \
        break;                                              \
    }

#define DEFINE_AND_CHECK_TYPE(result, type, var, param)    \
    type var;                                              \
    if (!request->checkGetParamAs(param, var)) {           \
        result->setResult(400, "param " param " is null"); \
        break;                                             \
    }

}

#endif // __BLOG_UTIL_H__