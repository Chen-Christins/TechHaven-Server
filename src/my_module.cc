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
#include "blog/data/subject_info.h"
#include "blog/data/assignment_info.h"
#include <chen/application.h>
#include "manager/user_manager.h"
#include "manager/article_manager.h"
#include "manager/category_manager.h"
#include "manager/label_manager.h"
#include "manager/article_category_rel_manager.h"
#include "manager/article_label_rel_manager.h"
#include "manager/subject_manager.h"
#include "manager/assignment_manager.h"
#include <chen/http/ws_server.h>
#include "servlets/user/user_admin_create_servlet.h"
#include "servlets/user/user_admin_delete_servlet.h"
#include "servlets/user/user_admin_lists_servlet.h"
#include "servlets/user/user_admin_recover_servlet.h"
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
#include "servlets/article/article_admin_lists_servlet.h"
#include "servlets/article/article_create_servlet.h"
#include "servlets/article/article_detail_servlet.h"
#include "servlets/article/article_publish_servlet.h"
#include "servlets/article/article_query_servlet.h"
#include "servlets/article/article_switch_state_servlet.h"
#include "servlets/article/article_delete_servlet.h"
#include "servlets/article/article_verify_servlet.h"
#include "servlets/article/article_update_servlet.h"
#include "servlets/article/article_update_category_servlet.h"
#include "servlets/category/category_create_servlet.h"
#include "servlets/category/category_delete_servlet.h"
#include "servlets/category/category_query_servlet.h"
#include "servlets/file/file_upload_servlet.h"
#include "servlets/label/label_create_servlet.h"
#include "servlets/label/label_delete_servlet.h"
#include "servlets/label/label_query_servlet.h"
#include "servlets/assignment/subject_create_servlet.h"
#include "servlets/assignment/subject_delete_servlet.h"
#include "servlets/assignment/assignment_create_servlet.h"
#include "servlets/assignment/assignment_delete_servlet.h"
#include "servlets/assignment/subject_details_servlet.h"
#include <chen/env.h>

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();
static chen::ConfigVar<std::string>::ptr sqlite3_db_name = 
    chen::Config::Lookup("sqlite3.db_name", std::string("blog.db"), "sqlite3 db file name");

MyModule::MyModule()
    :chen::Module("Blog", "1.0", "") {
}

bool MyModule::onLoad() {
    INFO(logger) << "onLoad";
    return true;
}

bool MyModule::onUnload() {
    INFO(logger) << "onUnload";
    return true;
}

int32_t handle_request(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session) {
    INFO(logger) << *request;
    response->setBody("ok");
    return 0;
}

bool MyModule::onServerReady() {
    INFO(logger) << "onServerReady";

    auto work_path = chen::Config::Lookup<std::string>("server.work_path");
    auto db_path = work_path->getValue() + "/" + sqlite3_db_name->getValue();

    chen::SQLite3::ptr db;
    db = chen::SQLite3::Create(db_path, chen::SQLite3::READWRITE);
    if (!db) {
        INFO(logger) << "init database begin";
        db = chen::SQLite3::Create(db_path);
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
	XX(SubjectInfoDao, "subject")
	XX(AssignmentInfoDao, "assignment")
#undef XX
        INFO(logger) << "init database end";
    }

    std::vector<chen::TcpServer::ptr> servers;
    if (!chen::Application::GetInstance()->getServer("http", servers)) {
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
	XX(SubjectMgr)
	XX(AssignmentMgr)
#undef XX

    for (auto& i : servers) {
        auto hs = std::dynamic_pointer_cast<chen::http::HttpServer>(i);
        auto dp = hs->getServletDispatch();

#define XX(clazz) chen::http::Servlet::ptr(new servlet::clazz)
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
        dp->addServlet("/user/admin/recover", XX(UserAdminRecoverServlet));
        dp->addServlet("/user/admin/reset_passwd", XX(UserAdminResetPasswdServlet));
        dp->addServlet("/user/admin/lists", XX(UserAdminListsServlet));
        
        dp->addServlet("/article/admin/lists", XX(ArticleAdminListsServlet));
        dp->addServlet("/article/create", XX(ArticleCreateServlet));
        dp->addServlet("/article/detail", XX(ArticleDetailServlet));
        dp->addServlet("/article/publish", XX(ArticlePublishServlet));
        dp->addServlet("/article/query", XX(ArticleQueryServlet));
        dp->addServlet("/article/delete", XX(ArticleDeleteServlet));
        dp->addServlet("/article/verify", XX(ArticleVerifyServlet));
        dp->addServlet("/article/update", XX(ArticleUpdateServlet));
        dp->addServlet("/article/update_category", XX(ArticleUpdateCategoryServlet));
        dp->addServlet("/article/switch_state", XX(ArticleSwitchStateServlet));

        dp->addServlet("/category/admin/create", XX(CategoryCreateServlet));
        dp->addServlet("/category/admin/delete", XX(CategoryDeleteServlet));
        dp->addServlet("/category/admin/query", XX(CategoryQueryServlet));

		dp->addServlet("/label/create", XX(LabelCreateServlet));
        dp->addServlet("/label/delete", XX(LabelDeleteServlet));
        dp->addServlet("/label/query", XX(LabelQueryServlet));
        
		dp->addServlet("/file/upload", XX(FileUploadServlet));
		dp->addServlet("/subject/create", XX(SubjectCreateServlet));
		dp->addServlet("/subject/delete", XX(SubjectDeleteServlet));
		dp->addServlet("/subject/details", XX(SubjectDetailsServlet));

		dp->addServlet("/assignment/create", XX(AssignmentCreateServlet));
		dp->addServlet("/assignment/delete", XX(AssignmentDeleteServlet));
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

chen::Module* CreateModule() {
    chen::Module* module = new blog::MyModule;
    INFO(blog::logger) << "CreateModule " << module;
    return module;
}

void DestoryModule(chen::Module* module) {
    INFO(blog::logger) << "DestoryModule " << module;
    delete module;
}

}