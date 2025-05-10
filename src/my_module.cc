#include "my_module.h"
#include "http/http_server.h"
#include "log/log.h"
#include "db/sqlite3.h"
#include "config/config.h"
#include "blog/data/user_info.h"
#include "application.h"
#include "servlets/user_create_servlet.h"
#include "manager/user_manager.h"

namespace blog {

static sylar::Logger::ptr logger = LOG_ROOT();
static sylar::ConfigVar<std::string>::ptr sqlite3_db_name = 
    sylar::Config::Lookup("sqlite3.db_name", std::string("blog.db"), "sqlite3 db file name");

MyModule::MyModule()
    :sylar::Module("Blog", "1.0", "") {
}

bool MyModule::onLoad() {
    INFO(logger) << "onLoad";
    return true;
}

bool MyModule::onUnload() {
    INFO(logger) << "onUnload";
    return true;
}

int32_t handle_request(sylar::http::HttpRequest::ptr request
                    ,sylar::http::HttpResponse::ptr response
                    ,sylar::http::HttpSession::ptr session) {
    INFO(logger) << *request;
    response->setBody("ok");
    return 0;
}

bool MyModule::onServerReady() {
    INFO(logger) << "onServerReady";

    auto work_path = sylar::Config::Lookup<std::string>("server.work_path");
    auto db_path = work_path->getValue() + "/" + sqlite3_db_name->getValue();

    sylar::SQLite3::ptr db;
    db = sylar::SQLite3::Create(db_path, sylar::SQLite3::READWRITE);
    if (!db) {
        INFO(logger) << "init database";
        db = sylar::SQLite3::Create(db_path);
        if (!db) {
            INFO(logger) << "open database db=" << db_path
                << " failed";
            return false;
        }
        if (blog::data::UserInfoDao::CreateTableSQLite3(db)) {
            ERROR(logger) << "create table user failed";
            return false;
        }
        INFO(logger) << "init database end";
    }

    std::vector<sylar::TcpServer::ptr> servers;
    if (!sylar::Application::GetInstance()->getServer("http", servers)) {
        ERROR(logger) << "http_server not open";
        return false;
    }

    if (!blog::UserMgr::GetInstance()->loadAll()) {
        ERROR(logger) << "user load all fail";
    }

    for (auto& i : servers) {
        auto hs = std::dynamic_pointer_cast<sylar::http::HttpServer>(i);
        auto dp = hs->getServletDispatch();

#define XX(clazz) sylar::http::Servlet::ptr(new servlet::clazz)

        dp->addServlet("/user/create", XX(UserCreateServlet));
        dp->addServlet("/user/active", handle_request);
        dp->addServlet("/user/login", handle_request);
        dp->addServlet("/user/update", handle_request);
        dp->addServlet("/user/exists", handle_request);
    }

    return true;
}

bool MyModule::onServerUp() {
    INFO(logger) << "onServerUp";
    return true;
}

sylar::SQLite3::ptr GetSQLite3() {
    auto work_path = sylar::Config::Lookup<std::string>("server.work_path");
    auto db_path = work_path->getValue() + "/" + sqlite3_db_name->getValue();
    sylar::SQLite3::ptr db = sylar::SQLite3::Create(db_path);
    return db;
}

}

extern "C" {

sylar::Module* CreateModule() {
    sylar::Module* module = new blog::MyModule;
    INFO(blog::logger) << "CreateModule " << module;
    return module;
}

void DestoryModule(sylar::Module* module) {
    INFO(blog::logger) << "DestoryModule " << module;
    delete module;
}

}