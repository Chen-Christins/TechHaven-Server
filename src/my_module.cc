#include "my_module.h"
#include "chen/http/http_server.h"
#include "chen/log/log.h"
#include "chen/db/sqlite3.h"
#include "chen/config/config.h"
#include "blog/data/user_info.h"
#include "chen/application.h"
#include "servlets/user_create_servlet.h"
#include "servlets/user_login_servlet.h"
#include "servlets/user_active_servlet.h"
#include "servlets/user_logout_servlet.h"
#include "servlets/user_info_servlet.h"
#include "servlets/user_update_servlet.h"
#include "servlets/user_exists_servlet.h"
#include "servlets/user_forget_password_servlet.h"
#include "manager/user_manager.h"
#include "chen/http/resource_servlet.h"
#include "chen/env.h"

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

int32_t handle_request(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
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
        INFO(logger) << "init database begin";
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

    if (!UserMgr::GetInstance()->loadAll()) {
        ERROR(logger) << "user load all fail";
    }

    for (auto& i : servers) {
        auto hs = std::dynamic_pointer_cast<sylar::http::HttpServer>(i);
        auto dp = hs->getServletDispatch();
        
        sylar::http::ResourceServlet::ptr slt(std::make_shared<sylar::http::ResourceServlet>(
            sylar::EnvMgr::GetInstance()->getCwd()
        ));
        dp->addGlobServlet("/html/*", slt);

#define XX(clazz) sylar::http::Servlet::ptr(new servlet::clazz)
        dp->addServlet("/user/create", XX(UserCreateServlet));
        dp->addServlet("/user/login", XX(UserLoginServlet));
        dp->addServlet("/user/active", XX(UserActiveServlet));
        dp->addServlet("/user/logout", XX(UserLogoutServlet));
        dp->addServlet("/user/info", XX(UserInfoServlet));
        dp->addServlet("/user/update", XX(UserUpdateServlet));
        dp->addServlet("/user/exists", XX(UserExistsServlet));
        dp->addServlet("/user/user_forget_passwd", XX(UserForgetPasswordServlet));
    }

    return true;
}

#undef XX

bool MyModule::onServerUp() {
    INFO(logger) << "onServerUp";
    return true;
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