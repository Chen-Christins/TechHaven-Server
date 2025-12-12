#include "my_module.h"
#include <chen/http/http_server.h>
#include <chen/log/log.h>
#include <chen/db/sqlite3.h>
#include <chen/config/config.h>
#include <chen/application.h>
#include <chen/http/ws_server.h>
#include "./include/tables.h"
#include "./include/managers.h"
#include "./include/servlets.h"
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
	XX(AssignmentInfoDao, "assignment")
    XX(OrganizationInfoDao, "organization")
    XX(OrganizationUserRelInfoDao, "organization_user_rel")
    XX(AssignmentOrganizationRelInfoDao, "assignment_organization_rel")
    XX(AssignmentUserRelInfoDao, "assignment_user_rel")
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
	XX(AssignmentMgr)
    XX(OrganizationMgr)
    XX(OrganizationUserRelMgr)
    XX(AssignmentOrganizationRelMgr)
    XX(AssignmentUserRelMgr)
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
        dp->addServlet("/user/organization/list", XX(UserOrganizationListServlet));

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

        dp->addServlet("/assignment/admin/lists", XX(AssignmentAdminListsServlet));
        dp->addServlet("/assignment/create", XX(AssignmentCreateServlet));
        dp->addServlet("/assignment/delete", XX(AssignmentDeleteServlet));

        dp->addServlet("/organization/admin/lists", XX(OrganizationAdminListsServlet));
        dp->addServlet("/organization/create", XX(OrganizationCreateServlet));
        dp->addServlet("/organization/delete", XX(OrganizationDeleteServlet));
        dp->addServlet("/organization/detail", XX(OrganizationDetailServlet));
        dp->addServlet("/organization/join", XX(OrganizationJoinServlet));
        dp->addServlet("/organization/join_check", XX(OrganizationJoinCheckServlet));
        dp->addServlet("/organization/list", XX(OrganizationListServlet));
        dp->addServlet("/organization/user_list", XX(OrganizationUserListServlet));
        dp->addServlet("/organization/user_switch_role", XX(OrganizationUserSwitchRoleServlet));
        dp->addServlet("/organization/user_kick", XX(OrganizationUserKickServlet));
        dp->addServlet("/organization/assignment_create", XX(AssignmentOrganizationCreateServlet));
        dp->addServlet("/organization/assignment_list", XX(OrganizationAssignmentListServlet));
#undef XX
    }

    return true;
}


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