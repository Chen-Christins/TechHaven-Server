#include "my_module.h"
#include <chen/http/http_server.h>
#include <chen/log/log.h>
#include <chen/db/sqlite3.h>
#include <chen/config/config.h>
#include "blog/data/email_verification_info.h"
#include "blog/data/article_info.h"
#include "blog/data/category_info.h"
#include "blog/data/article_category_rel_info.h"
#include "blog/data/article_label_rel_info.h"
#include <chen/application.h>
#include "manager/user_manager.h"
#include "manager/article_manager.h"
#include "manager/category_manager.h"
#include "manager/label_manager.h"
#include "manager/article_category_rel_manager.h"
#include "manager/article_label_rel_manager.h"
#include <chen/http/resource_servlet.h>
#include "servlets/user/user_admin_create_servlet.h"
#include "servlets/user/user_admin_delete_servlet.h"
#include "servlets/user/user_admin_reset_passwd_servlet.h"
#include "servlets/user/user_create_servlet.h"
#include "servlets/user/user_exists_servlet.h"
#include "servlets/user/user_info_servlet.h"
#include "servlets/user/user_list_servlet.h"
#include "servlets/user/user_login_servlet.h"
#include "servlets/user/user_logout_servlet.h"
#include "servlets/user/user_reset_passwd_servlet.h"
#include "servlets/user/user_send_code_servlet.h"
#include "servlets/user/user_update_servlet.h"
#include "servlets/user/user_query_servlet.h"
#include "servlets/article/article_create_servlet.h"
#include "servlets/article/article_detail_servlet.h"
#include "servlets/article/article_publish_servlet.h"
#include "servlets/article/article_query_servlet.h"
#include "servlets/article/article_delete_servlet.h"
#include "servlets/article/article_verify_servlet.h"
#include "servlets/article/article_update_servlet.h"
#include "servlets/article/article_update_category_servlet.h"
#include "servlets/category/category_create_servlet.h"
#include "servlets/category/category_delete_servlet.h"
#include "servlets/category/category_query_servlet.h"
#include "servlets/label/label_create_servlet.h"
#include "servlets/label/label_delete_servlet.h"
#include "servlets/label/label_query_servlet.h"
#include <chen/env.h>

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

#define XX(clazz, t)                                   \
    if (blog::data::clazz::CreateTableSQLite3(db)) {   \
        ERROR(logger) << "create table " t " failed";  \
        return false;                                  \
    }
    XX(EmailVerificationInfoDao, "email_verification")
    XX(UserInfoDao, "user")
    XX(ArticleInfoDao, "article")
    XX(CategoryInfoDao, "category")
    XX(LabelInfoDao, "label")
    XX(ArticleCategoryRelInfoDao, "article_category_rel")
    XX(ArticleLabelRelInfoDao, "article_label_rel")
#undef XX
        INFO(logger) << "init database end";
    }

    std::vector<sylar::TcpServer::ptr> servers;
    if (!sylar::Application::GetInstance()->getServer("http", servers)) {
        ERROR(logger) << "http_server not open";
        return false;
    }

#define XX(clazz)                                 \
    if (!clazz::GetInstance()->loadAll()) {       \
        ERROR(logger) << #clazz " load all fail"; \
    }
    XX(UserMgr)
    XX(ArticleMgr)
    XX(CategoryMgr)
    XX(LabelMgr)
    XX(ArticleCategoryRelMgr)
    XX(ArticleLabelRelMgr)
#undef XX

    for (auto& i : servers) {
        auto hs = std::dynamic_pointer_cast<sylar::http::HttpServer>(i);
        auto dp = hs->getServletDispatch();
        
        sylar::http::ResourceServlet::ptr slt(std::make_shared<sylar::http::ResourceServlet>(
            sylar::EnvMgr::GetInstance()->getCwd()
        ));
        dp->addGlobServlet("/blog/*", slt);

#define XX(clazz) sylar::http::Servlet::ptr(new servlet::clazz)
        dp->addServlet("/user/send_code", XX(UserSendCodeServlet));
        dp->addServlet("/user/create", XX(UserCreateServlet));
        dp->addServlet("/user/login", XX(UserLoginServlet));
        dp->addServlet("/user/info", XX(UserInfoServlet));
        dp->addServlet("/user/list", XX(UserListServlet));
        dp->addServlet("/user/logout", XX(UserLogoutServlet));
        dp->addServlet("/user/forget_passwd", XX(UserResetPasswdServlet));
        dp->addServlet("/user/exists", XX(UserExistsServlet));
        dp->addServlet("/user/update", XX(UserUpdateServlet));
        dp->addServlet("/user/query", XX(UserQueryServlet));
        dp->addServlet("/user/admin/create", XX(UserAdminCreateServlet));
        dp->addServlet("/user/admin/delete", XX(UserAdminDeleteServlet));
        dp->addServlet("/user/admin/reset_passwd", XX(UserAdminResetPasswdServlet));
        
        dp->addServlet("/article/create", XX(ArticleCreateServlet));
        dp->addServlet("/article/detail", XX(ArticleDetailServlet));
        dp->addServlet("/article/publish", XX(ArticlePublishServlet));
        dp->addServlet("/article/query", XX(ArticleQueryServlet));
        dp->addServlet("/article/delete", XX(ArticleDeleteServlet));
        dp->addServlet("/article/verify", XX(ArticleVerifyServlet));
        dp->addServlet("/article/update", XX(ArticleUpdateServlet));
        dp->addServlet("/article/update_category", XX(ArticleUpdateCategoryServlet));

        dp->addServlet("/category/create", XX(CategoryCreateServlet));
        dp->addServlet("/category/delete", XX(CategoryDeleteServlet));
        dp->addServlet("/category/query", XX(CategoryQueryServlet));
        
		dp->addServlet("/label/create", XX(LabelCreateServlet));
        dp->addServlet("/label/delete", XX(LabelDeleteServlet));
        dp->addServlet("/label/query", XX(LabelQueryServlet));
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