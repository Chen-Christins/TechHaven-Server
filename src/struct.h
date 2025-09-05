/**
 * @file struct.h
 * @brief 博客使用到的数据结构
 * @author Christins
 * @date 2025-05-11
 * @copyright Apache 2.0
 */
#ifndef __BLOG_STRUCT_H__
#define __BLOG_STRUCT_H__

#include <string>
#include <memory>
#include <json/json.h>
#include <chen/http/servlet.h>
#include <chen/db/db.h>
#include <chen/http/session_data.h>

namespace blog {

enum class State {
    VERIFYING = 1,
    PUBLISH   = 2,
    NOT_PASS  = 3,
    UNPUBLISH = 4
};

struct Result {
    typedef std::shared_ptr<Result> ptr;
    Result(int32_t c = 200, const std::string& msg = "ok");

    int32_t code;
    int64_t used;
    std::string msg;
    // std::map<std::string, std::string> datas;
    Json::Value jsondata;

    template <class T>
    void set(const std::string& key, const T& v) {
        jsondata[key] = v;
    }
    void set(const std::string& key, const char* v) {
        jsondata[key] = v;
    }
    void set(const std::string& key, const std::string& v) {
        jsondata[key] = v;
    }

    template<class T>
    void append(const std::string& key, const T& v) {
        jsondata[key].append(v);
    }

    void setResult(int32_t c, const std::string& m);

    std::string toJsonString() const;
};

struct CookieKey {
    static const std::string SESSION_KEY;
    static const std::string USER_ID;
    static const std::string TOKEN;
    static const std::string TOKEN_TIME;
    static const std::string IS_AUTH;
    static const std::string EMAIL_LAST_TIME;
};

class BlogServlet: public chen::http::Servlet {
public:
    BlogServlet(const std::string& name);
    int32_t handle(chen::http::HttpRequest::ptr request
                ,chen::http::HttpResponse::ptr response
                ,chen::http::HttpSession::ptr session) override;

    int64_t getUserId(chen::http::HttpRequest::ptr request);
protected:
    virtual bool handlePre(chen::http::HttpRequest::ptr request
                           ,chen::http::HttpResponse::ptr response
                           ,chen::http::HttpSession::ptr session
                           ,Result::ptr result);
    virtual int32_t handle(chen::http::HttpRequest::ptr request
                           ,chen::http::HttpResponse::ptr response
                           ,chen::http::HttpSession::ptr session
                           ,Result::ptr result) = 0;
    virtual bool handlePost(chen::http::HttpRequest::ptr request
                           ,chen::http::HttpResponse::ptr response
                           ,chen::http::HttpSession::ptr session
                           ,Result::ptr result);
protected:
    chen::http::SessionData::ptr getSessionData(chen::http::HttpRequest::ptr request
                                                 ,chen::http::HttpResponse::ptr response);
    bool initLogin(chen::http::HttpRequest::ptr request
                   ,chen::http::HttpResponse::ptr response
                   ,chen::http::HttpSession::ptr session);
protected:
    chen::IDB::ptr getDB();
};

class BlogLoginedServlet : public BlogServlet {
public:
    BlogLoginedServlet(const std::string& name);

    bool handlePre(chen::http::HttpRequest::ptr request
                   ,chen::http::HttpResponse::ptr response
                   ,chen::http::HttpSession::ptr session
                   ,Result::ptr result) override;
};

}

#endif // __BLOG_STRUCT_H__