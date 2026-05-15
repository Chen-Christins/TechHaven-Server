#include "blog_module.h"

#include <chen/http/http_server.h>
#include <chen/log/log.h>
#include <chen/db/sqlite3.h>
#include <chen/config/config.h>
#include <chen/application.h>
#include <chen/http/ws_server.h>
#include <chen/http/ws_servlet.h>
#include <chen/env.h>

#include "./include/tables.h"
#include "./include/managers.h"
#include "./include/servlets.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();
static chen::ConfigVar<std::string>::ptr sqlite3_db_name = 
    chen::Config::Lookup("sqlite3.db_name", std::string("blog.db"), "sqlite3 db file name");

BlogModule::BlogModule()
    :chen::Module("Blog", "1.0", "") {
}

bool BlogModule::onLoad() {
    INFO(logger) << "onLoad";
    return true;
}

bool BlogModule::onUnload() {
    INFO(logger) << "onUnload";
    return true;
}

bool BlogModule::onServerReady() {
    INFO(logger) << "onServerReady";

	if (!initDB()) {
		ERROR(logger) << "initDB failed";
		return false;
	}

	loadAllData();

    std::vector<chen::TcpServer::ptr> servers;
    if (chen::Application::GetInstance()->getServer("http", servers)) {
        registerServlets(servers);
    } else {
        ERROR(logger) << "http_server not open";
        return false;
    }

	std::vector<chen::TcpServer::ptr> wsservers;
	if (chen::Application::GetInstance()->getServer("ws", wsservers)) {
		registerWSServlets(wsservers);
	} else {
		INFO(logger) << "ws_server not open, skip WebSocket servlets";
	}

    return true;
}


bool BlogModule::onServerUp() {
    INFO(logger) << "onServerUp";
    return true;
}

bool BlogModule::initDB() {
	INFO(logger) << "initDB";

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
    XX(ResourceInfoDao, "resource")
    XX(ChunkUploadInfoDao, "chunk_upload")
    XX(NotificationInfoDao, "notification")
    XX(UserFollowRelInfoDao, "user_follow_rel")
    XX(ArticlePraiseRelInfoDao, "article_praise_rel")
#undef XX
        INFO(logger) << "init database end";
    }

	return true;
}

void BlogModule::loadAllData() {
	INFO(logger) << "loadAllData";

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
    XX(ResourceMgr)
    XX(ChunkUploadMgr)
    XX(NotificationMgr)
    XX(UserFollowRelMgr)
    XX(ArticlePraiseRelMgr)
#undef XX

}

void BlogModule::registerServlets(std::vector<chen::TcpServer::ptr>& servers) {
	INFO(logger) << "registerServlets";

	for (auto& i : servers) {
        auto hs = std::dynamic_pointer_cast<chen::http::HttpServer>(i);
        auto dp = hs->getServletDispatch();

#define XX(clazz) chen::http::Servlet::ptr(new servlet::clazz)
		// 用户相关
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
        dp->addServlet("/user/stats", XX(UserStatsServlet));
        dp->addServlet("/user/admin/create", XX(UserAdminCreateServlet));
        dp->addServlet("/user/admin/delete", XX(UserAdminDeleteServlet));
        dp->addServlet("/user/admin/recover", XX(UserAdminRecoverServlet));
        dp->addServlet("/user/admin/reset_passwd", XX(UserAdminResetPasswdServlet));
        dp->addServlet("/user/admin/lists", XX(UserAdminListsServlet));
        dp->addServlet("/user/organization/list", XX(UserOrganizationListServlet));
        dp->addServlet("/user/assignment/list", XX(UserAssignmentListServlet));
        dp->addServlet("/user/is_following", XX(UserIsFollowingServlet));
        dp->addServlet("/user/follow", XX(UserFollowServlet));
        dp->addServlet("/user/unfollow", XX(UserUnfollowServlet));
        dp->addServlet("/user/following/list", XX(UserFollowingListServlet));
        dp->addServlet("/user/follower/list", XX(UserFollowerListServlet));
        // 通知相关
        dp->addServlet("/notification/send", XX(NotificationSendServlet));
        dp->addServlet("/notification/list", XX(NotificationListServlet));
        dp->addServlet("/notification/unread_count", XX(NotificationUnreadCountServlet));
        dp->addServlet("/notification/read", XX(NotificationReadServlet));
        dp->addServlet("/notification/read_all", XX(NotificationReadAllServlet));
		// 文章相关
        dp->addServlet("/article/admin/lists", XX(ArticleAdminListsServlet));
        dp->addServlet("/article/admin/stats", XX(ArticleAdminStatsServlet));
        dp->addServlet("/article/create", XX(ArticleCreateServlet));
        dp->addServlet("/article/detail", XX(ArticleDetailServlet));
        dp->addServlet("/article/publish", XX(ArticlePublishServlet));
        dp->addServlet("/article/query", XX(ArticleQueryServlet));
        dp->addServlet("/article/list_by_label", XX(ArticleListByLabelServlet));
        dp->addServlet("/article/list_by_category", XX(ArticleListByCategoryServlet));
        dp->addServlet("/article/delete", XX(ArticleDeleteServlet));
        dp->addServlet("/article/verify", XX(ArticleVerifyServlet));
        dp->addServlet("/article/update", XX(ArticleUpdateServlet));
        dp->addServlet("/article/update_category", XX(ArticleUpdateCategoryServlet));
        dp->addServlet("/article/switch_state", XX(ArticleSwitchStateServlet));
        dp->addServlet("/article/is_praising", XX(ArticleIsPraisingServlet));
        dp->addServlet("/article/praise", XX(ArticlePraiseServlet));
        dp->addServlet("/article/praise/list", XX(ArticlePraiseListServlet));
		// 文章分类相关
        dp->addServlet("/category/admin/create", XX(CategoryCreateServlet));
        dp->addServlet("/category/admin/delete", XX(CategoryDeleteServlet));
        dp->addServlet("/category/admin/query", XX(CategoryQueryServlet));
		// 文章标签相关
        dp->addServlet("/label/create", XX(LabelCreateServlet));
        dp->addServlet("/label/delete", XX(LabelDeleteServlet));
        dp->addServlet("/label/query", XX(LabelQueryServlet));
		// 文件相关
        dp->addServlet("/file/upload", XX(FileUploadServlet));
        dp->addServlet("/file/download", XX(FileDownloadServlet));
		// 大文件分片上传相关
        dp->addServlet("/upload/init", XX(ChunkUploadServlet));
        dp->addServlet("/upload/chunk", XX(ChunkUploadServlet));
        dp->addServlet("/upload/complete", XX(ChunkUploadServlet));
        dp->addServlet("/upload/cancel", XX(ChunkUploadServlet));
        dp->addServlet("/upload/status", XX(ChunkUploadServlet));
		// 作业相关
        dp->addServlet("/assignment/admin/lists", XX(AssignmentAdminListsServlet));
        dp->addServlet("/assignment/admin/stats", XX(AssignmentAdminStatsServlet));
        dp->addServlet("/assignment/create", XX(AssignmentCreateServlet));
        dp->addServlet("/assignment/delete", XX(AssignmentDeleteServlet));
        dp->addServlet("/assignment/detail", XX(AssignmentDetailServlet));
        dp->addServlet("/assignment/submission/list", XX(AssignmentSubmissionListServlet));
		// 组织相关
        dp->addServlet("/organization/admin/lists", XX(OrganizationAdminListsServlet));
        dp->addServlet("/organization/admin/stats", XX(OrganizationAdminStatsServlet));
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

}

void BlogModule::registerWSServlets(std::vector<chen::TcpServer::ptr>& servers) {
    INFO(logger) << "registerWSServlets";

    for (auto& i : servers) {
        auto ws = std::dynamic_pointer_cast<chen::http::WSServer>(i);
		ASSERT(ws);

        chen::http::ServletDispatch::ptr dp = ws->getWSServletDispatch();
		ASSERT(dp);

		servlet::NotifyServlet::ptr notify_servlet(std::make_shared<servlet::NotifyServlet>());
        dp->addServlet("/notification", notify_servlet);
    }
}

}

extern "C" {

chen::Module* CreateModule() {
    chen::Module* module = new blog::BlogModule;
    INFO(blog::logger) << "CreateModule " << module;
    return module;
}

void DestroyModule(chen::Module* module) {
    INFO(blog::logger) << "DestroyModule " << module;
    delete module;
}

}