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
    ArticleMgr::GetInstance()->stop();
    NotificationMgr::GetInstance()->closeAllConnections();
    return true;
}

bool BlogModule::onServerReady() {
    INFO(logger) << "onServerReady";

	if (!initDB()) {
		ERROR(logger) << "initDB failed";
		return false;
	}

	loadAllData();
	
	ArticleMgr::GetInstance()->start();

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
    db = chen::SQLite3::Create(db_path);
    if (!db) {
        INFO(logger) << "init database begin";
        db = chen::SQLite3::Create(db_path);
        if (!db) {
            INFO(logger) << "open database db=" << db_path
                << " failed";
            return false;
        }
        INFO(logger) << "init database end";
    }

    // 确保所有表存在（CREATE TABLE IF NOT EXISTS 幂等，新旧数据库均可安全执行）
    {
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
    XX(CommentInfoDao, "comment")
    XX(CommentPraiseRelInfoDao, "comment_praise_rel")
    XX(RequirementInfoDao, "requirement")
    XX(BugInfoDao, "bug")
    XX(TaskInfoDao, "task")
    XX(SystemSettingsInfoDao, "system_settings")
#undef XX
    }

    // 数据库迁移：为已有表补充新增列
    {
        INFO(logger) << "migrate database begin";
#define XX(clazz) blog::data::clazz::MigrateTableSQLite3(db);
        XX(EmailVerificationInfoDao)
        XX(UserInfoDao)
        XX(ArticleInfoDao)
        XX(CategoryInfoDao)
        XX(LabelInfoDao)
        XX(ArticleCategoryRelInfoDao)
        XX(ArticleLabelRelInfoDao)
        XX(AssignmentInfoDao)
        XX(OrganizationInfoDao)
        XX(OrganizationUserRelInfoDao)
        XX(AssignmentOrganizationRelInfoDao)
        XX(AssignmentUserRelInfoDao)
        XX(ResourceInfoDao)
        XX(ChunkUploadInfoDao)
        XX(NotificationInfoDao)
        XX(UserFollowRelInfoDao)
        XX(ArticlePraiseRelInfoDao)
        XX(CommentInfoDao)
        XX(CommentPraiseRelInfoDao)
        XX(RequirementInfoDao)
        XX(BugInfoDao)
        XX(TaskInfoDao)
#undef XX
        INFO(logger) << "migrate database end";
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
    XX(CommentMgr)
    XX(CommentPraiseRelMgr)
    XX(RequirementMgr)
    XX(BugMgr)
    XX(TaskMgr)
    XX(SystemSettingsMgr)
#undef XX

}

void BlogModule::registerServlets(std::vector<chen::TcpServer::ptr>& servers) {
	INFO(logger) << "registerServlets";

	for (auto& i : servers) {
        auto hs = std::dynamic_pointer_cast<chen::http::HttpServer>(i);
        auto dp = hs->getServletDispatch();

#define XX(clazz) chen::http::Servlet::ptr(new servlet::clazz)
		// 用户相关
        dp->addServlet("/api/v1/user/send_code", XX(UserSendCodeServlet));
        dp->addServlet("/api/v1/user/create", XX(UserCreateServlet));
        dp->addServlet("/api/v1/user/login", XX(UserLoginServlet));
        dp->addServlet("/api/v1/user/info", XX(UserInfoServlet));
        dp->addServlet("/api/v1/user/list", XX(UserListServlet));
        dp->addServlet("/api/v1/user/logout", XX(UserLogoutServlet));
        dp->addServlet("/api/v1/user/forget_passwd", XX(UserResetPasswdServlet));
        dp->addServlet("/api/v1/user/exists", XX(UserExistsServlet));
        dp->addServlet("/api/v1/user/update", XX(UserUpdateServlet));
        dp->addServlet("/api/v1/user/query", XX(UserQueryServlet));
        dp->addServlet("/api/v1/user/stats", XX(UserStatsServlet));
        dp->addServlet("/api/v1/user/admin/create", XX(UserAdminCreateServlet));
        dp->addServlet("/api/v1/user/admin/delete", XX(UserAdminDeleteServlet));
        dp->addServlet("/api/v1/user/admin/recover", XX(UserAdminRecoverServlet));
        dp->addServlet("/api/v1/user/admin/reset_passwd", XX(UserAdminResetPasswdServlet));
        dp->addServlet("/api/v1/user/admin/lists", XX(UserAdminListsServlet));
        dp->addServlet("/api/v1/user/organization/list", XX(UserOrganizationListServlet));
        dp->addServlet("/api/v1/user/assignment/list", XX(UserAssignmentListServlet));
        dp->addServlet("/api/v1/user/is_following", XX(UserIsFollowingServlet));
        dp->addServlet("/api/v1/user/follow", XX(UserFollowServlet));
        dp->addServlet("/api/v1/user/unfollow", XX(UserUnfollowServlet));
        dp->addServlet("/api/v1/user/following/list", XX(UserFollowingListServlet));
        dp->addServlet("/api/v1/user/follower/list", XX(UserFollowerListServlet));
        // 通知相关
        dp->addServlet("/api/v1/notification/send", XX(NotificationSendServlet));
        dp->addServlet("/api/v1/notification/list", XX(NotificationListServlet));
        dp->addServlet("/api/v1/notification/unread_count", XX(NotificationUnreadCountServlet));
        dp->addServlet("/api/v1/notification/read", XX(NotificationReadServlet));
        dp->addServlet("/api/v1/notification/read_all", XX(NotificationReadAllServlet));
		// 文章相关
        dp->addServlet("/api/v1/article/admin/lists", XX(ArticleAdminListsServlet));
        dp->addServlet("/api/v1/article/admin/stats", XX(ArticleAdminStatsServlet));
        dp->addServlet("/api/v1/article/create", XX(ArticleCreateServlet));
        dp->addServlet("/api/v1/article/detail", XX(ArticleDetailServlet));
        dp->addServlet("/api/v1/article/publish", XX(ArticlePublishServlet));
        dp->addServlet("/api/v1/article/query", XX(ArticleQueryServlet));
        dp->addServlet("/api/v1/article/list_by_label", XX(ArticleListByLabelServlet));
        dp->addServlet("/api/v1/article/list_by_category", XX(ArticleListByCategoryServlet));
        dp->addServlet("/api/v1/article/delete", XX(ArticleDeleteServlet));
        dp->addServlet("/api/v1/article/verify", XX(ArticleVerifyServlet));
        dp->addServlet("/api/v1/article/update", XX(ArticleUpdateServlet));
        dp->addServlet("/api/v1/article/update_category", XX(ArticleUpdateCategoryServlet));
        dp->addServlet("/api/v1/article/switch_state", XX(ArticleSwitchStateServlet));
        dp->addServlet("/api/v1/article/is_praising", XX(ArticleIsPraisingServlet));
        dp->addServlet("/api/v1/article/praise", XX(ArticlePraiseServlet));
        dp->addServlet("/api/v1/article/praise/list", XX(ArticlePraiseListServlet));
        dp->addServlet("/api/v1/article/view", XX(ArticleViewServlet));
        // 文章评论相关
        dp->addServlet("/api/v1/article/comment/list", XX(CommentListServlet));
        dp->addServlet("/api/v1/article/comment/replies", XX(CommentRepliesServlet));
        dp->addServlet("/api/v1/article/comment/create", XX(CommentCreateServlet));
        dp->addServlet("/api/v1/article/comment/update", XX(CommentUpdateServlet));
        dp->addServlet("/api/v1/article/comment/delete", XX(CommentDeleteServlet));
        dp->addServlet("/api/v1/article/comment/praise", XX(CommentPraiseServlet));
		// 管理端评论相关
        dp->addServlet("/api/v1/admin/comment/list", XX(AdminCommentListServlet));
        dp->addServlet("/api/v1/admin/comment/approve", XX(AdminCommentApproveServlet));
        dp->addServlet("/api/v1/admin/comment/reject", XX(AdminCommentRejectServlet));
        dp->addServlet("/api/v1/admin/comment/spam", XX(AdminCommentSpamServlet));
        dp->addServlet("/api/v1/admin/comment/delete", XX(AdminCommentDeleteServlet));
        dp->addServlet("/api/v1/admin/comment/stats", XX(AdminCommentStatsServlet));
        // 仪表盘相关
        dp->addServlet("/api/v1/admin/dashboard/stats", XX(DashboardStatsServlet));
        dp->addServlet("/api/v1/admin/dashboard/trend", XX(DashboardTrendServlet));
        dp->addServlet("/api/v1/admin/dashboard/activities", XX(DashboardActivitiesServlet));
        dp->addServlet("/api/v1/admin/dashboard/recent-users", XX(DashboardRecentUsersServlet));
        // 文章分类相关
        dp->addServlet("/api/v1/category/admin/create", XX(CategoryCreateServlet));
        dp->addServlet("/api/v1/category/admin/delete", XX(CategoryDeleteServlet));
        dp->addServlet("/api/v1/category/admin/query", XX(CategoryQueryServlet));
        // 站点公开状态
        dp->addServlet("/api/v1/site/status", XX(SiteStatusServlet));
        // 系统设置相关
        dp->addServlet("/api/v1/admin/settings", XX(SystemSettingsServlet));
        dp->addServlet("/api/v1/admin/settings/upload", XX(SystemSettingsUploadServlet));
        // 首页统计（公开接口）
        dp->addServlet("/api/v1/stats", XX(StatsServlet));
		// 文章标签相关
        dp->addServlet("/api/v1/label/create", XX(LabelCreateServlet));
        dp->addServlet("/api/v1/label/delete", XX(LabelDeleteServlet));
        dp->addServlet("/api/v1/label/query", XX(LabelQueryServlet));
		// 文件相关
        dp->addServlet("/api/v1/file/upload", XX(FileUploadServlet));
        dp->addServlet("/api/v1/file/download", XX(FileDownloadServlet));
		// 大文件分片上传相关
        dp->addServlet("/api/v1/upload/init", XX(ChunkUploadServlet));
        dp->addServlet("/api/v1/upload/chunk", XX(ChunkUploadServlet));
        dp->addServlet("/api/v1/upload/complete", XX(ChunkUploadServlet));
        dp->addServlet("/api/v1/upload/cancel", XX(ChunkUploadServlet));
        dp->addServlet("/api/v1/upload/status", XX(ChunkUploadServlet));
		// 作业相关
        dp->addServlet("/api/v1/assignment/admin/lists", XX(AssignmentAdminListsServlet));
        dp->addServlet("/api/v1/assignment/admin/stats", XX(AssignmentAdminStatsServlet));
        dp->addServlet("/api/v1/assignment/create", XX(AssignmentCreateServlet));
        dp->addServlet("/api/v1/assignment/delete", XX(AssignmentDeleteServlet));
        dp->addServlet("/api/v1/assignment/detail", XX(AssignmentDetailServlet));
        dp->addServlet("/api/v1/assignment/submission/list", XX(AssignmentSubmissionListServlet));
		// 组织相关
        dp->addServlet("/api/v1/organization/admin/lists", XX(OrganizationAdminListsServlet));
        dp->addServlet("/api/v1/organization/admin/stats", XX(OrganizationAdminStatsServlet));
        dp->addServlet("/api/v1/organization/create", XX(OrganizationCreateServlet));
        dp->addServlet("/api/v1/organization/delete", XX(OrganizationDeleteServlet));
        dp->addServlet("/api/v1/organization/detail", XX(OrganizationDetailServlet));
        dp->addServlet("/api/v1/organization/join", XX(OrganizationJoinServlet));
        dp->addServlet("/api/v1/organization/join_check", XX(OrganizationJoinCheckServlet));
        dp->addServlet("/api/v1/organization/list", XX(OrganizationListServlet));
        dp->addServlet("/api/v1/organization/user_list", XX(OrganizationUserListServlet));
        dp->addServlet("/api/v1/organization/user_switch_role", XX(OrganizationUserSwitchRoleServlet));
        dp->addServlet("/api/v1/organization/user_kick", XX(OrganizationUserKickServlet));
        dp->addServlet("/api/v1/organization/assignment_create", XX(AssignmentOrganizationCreateServlet));
        dp->addServlet("/api/v1/organization/assignment_list", XX(OrganizationAssignmentListServlet));
        // R&D 平台相关（新版统一 API）
        dp->addServlet("/api/v1/rd/check_access", XX(RdCheckAccessServlet));
        dp->addServlet("/api/v1/rd/requirements", XX(RdRequirementServlet));
        dp->addServlet("/api/v1/rd/requirements/edit", XX(RdRequirementEditServlet));
        dp->addServlet("/api/v1/rd/requirements/detail", XX(RdRequirementDetailServlet));
        dp->addServlet("/api/v1/rd/requirements/delete", XX(RdRequirementDeleteServlet));
        dp->addServlet("/api/v1/rd/bugs", XX(RdBugServlet));
        dp->addServlet("/api/v1/rd/bugs/edit", XX(RdBugEditServlet));
        dp->addServlet("/api/v1/rd/bugs/detail", XX(RdBugDetailServlet));
        dp->addServlet("/api/v1/rd/bugs/delete", XX(RdBugDeleteServlet));
        dp->addServlet("/api/v1/rd/tasks", XX(RdTaskServlet));
        dp->addServlet("/api/v1/rd/tasks/edit", XX(RdTaskEditServlet));
        dp->addServlet("/api/v1/rd/tasks/detail", XX(RdTaskDetailServlet));
        dp->addServlet("/api/v1/rd/tasks/delete", XX(RdTaskDeleteServlet));
        dp->addServlet("/api/v1/rd/stats", XX(RdStatsServlet));
        dp->addServlet("/api/v1/rd/my-tickets", XX(RdMyTicketsServlet));
        dp->addServlet("/api/v1/rd/organizations", XX(RdOrganizationsServlet));
        dp->addServlet("/api/v1/rd/organizations/members", XX(RdOrganizationMembersServlet));
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
        dp->addServlet("/ws/v1/notification", notify_servlet);
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