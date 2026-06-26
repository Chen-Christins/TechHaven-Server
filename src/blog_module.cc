#include "blog_module.h"

#include <chen/http/http_server.h>
#include <chen/log/log.h>
#include <chen/db/query_builder.h>
#include <chen/db/sqlite3.h>
#include <chen/db/mysql.h>
#include <chen/config/config.h>
#include <chen/application.h>
#include <chen/http/ws_server.h>
#include <chen/http/ws_servlet.h>
#include <chen/env.h>
#include <chen/worker.h>

#include <ranges>

#include "./include/tables.h"
#include "./include/managers.h"
#include "./include/servlets.h"
#include "./chunk_upload.h"
#include "protocol_ss_github.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();
static chen::ConfigVar<std::map<std::string, std::map<std::string, std::string>>>::ptr g_mysql_dbs =
    chen::Config::Lookup("mysql.dbs", std::map<std::string, std::map<std::string, std::string>>(), "mysql dbs");

BlogModule::BlogModule()
    :chen::Module("Blog", "1.0", "blog_module") {
}

bool BlogModule::onLoad() {
    INFO(logger) << "onLoad";
    return true;
}

bool BlogModule::onUnload() {
    INFO(logger) << "onUnload";
    ArticleMgr::GetInstance()->stop();
    NotificationMgr::GetInstance()->closeAllConnections();
    unregisterWSServlets();
    unregisterServlets();
    unregisterRPCMethods();
    return true;
}

bool BlogModule::onDrain() {
    INFO(logger) << "onDrain";
    ArticleMgr::GetInstance()->stop();
    NotificationMgr::GetInstance()->closeAllConnections();
    // 注意：不调用 unregisterServlets/unregisterWSServlets
    // dispatch 已被框架 clearServlets() 清理，新模块已重新注册
    return true;
}

bool BlogModule::onGracefulUnload() {
    INFO(logger) << "onGracefulUnload";
    // dispatch 已被 clearServlets() 清理，无需再 clear
    return true;
}

void BlogModule::onTick() {
    // 1. 定时发布已到发布时间的文章
    ArticleMgr::GetInstance()->onTimer();

    // 2. 定时 flush 脏数据（浏览/点赞/收藏数）到数据库
    ArticleMgr::GetInstance()->onUpdateTimer();

    // 3. 清理过期的分块上传会话及临时文件
    ::ChunkUploadMgr::GetInstance()->cleanupExpiredSessions();

    // 4. 定时同步有 token 的仓库 PR（每 30 分钟）
    static int s_pr_sync_tick = 0;
    if (++s_pr_sync_tick >= 30) {
        s_pr_sync_tick = 0;
        std::vector<data::OrganizationReposInfo::ptr> repos;
        OrganizationRepoMgr::GetInstance()->getAllWithToken(repos);
        for (auto& repo : repos) {
            int64_t repo_id = repo->getId();
            std::string url = repo->getUrl();
            std::string token = repo->getToken();
            chen::Scheduler::GetThis()->schedule([repo_id, url, token]() {
                OrganizationRepoPrManager::SyncFromGitHub(repo_id, url, token);
            });
        }
    }
}

uint64_t BlogModule::getTickIntervalMs() {
    // 设置定时器间隔为1分钟
    return 60 * 1000;
}

bool BlogModule::onServerReady() {
    INFO(logger) << "onServerReady";

    if (!initMySQL()) {
        ERROR(logger) << "initDB failed";
        return false;
    }

    // 启动时从 DB 同步统计计数到 Redis，覆盖旧实例可能残留的数据
    ArticleMgr::GetInstance()->syncStatsFromDB();

    ArticleMgr::GetInstance()->start();

    // 初始化错误码管理器
    {
        std::string workPath = chen::Config::Lookup<std::string>("server.work_path")->getValue();
        std::string errorsPath = workPath + "/errors.json";
        if (!ErrorCodeMgr::GetInstance()->load(errorsPath)) {
            ERROR(logger) << "Failed to load error codes from " << errorsPath;
            // 不阻止启动，使用空错误码表（兜底）
        }
    }

    getAllHttpServer(m_httpServers);
    if (m_httpServers.empty()) {
        ERROR(logger) << "no http server, cannot register servlets";
        return false;
    }
    
    registerServlets();

    getAllWSServer(m_wsServers);
    if (m_wsServers.empty()) {
        ERROR(logger) << "no ws server, cannot register ws servlets";
        return false;
    }

    registerWSServlets();

    // 注册 RPC 方法
    {
        std::vector<chen::rpc::RpcServer::ptr> rpc_servers;
        getAllRpcServer(rpc_servers);
        for (auto& s : rpc_servers) {
            if (!s) continue;
            s->registerMethod("GithubPRWebhook", OrganizationRepoPrManager::HandlePRWebhook);
            s->registerMethod("GithubPRReviewWebhook", OrganizationRepoPrManager::HandlePRReviewWebhook);
            INFO(logger) << "registered RPC methods on " << s->getName();
        }
    }

    return true;
}

bool BlogModule::onServerUp() {
    INFO(logger) << "onServerUp";
    return true;
}

bool BlogModule::initMySQL() {
    INFO(logger) << "initMySQL";

    const auto& mysql_dbs = g_mysql_dbs->getValue();
    for (const auto& params : mysql_dbs | std::views::values) {
        chen::MySQL::ptr mysql(new chen::MySQL(params));
        if (!mysql->connect()) {
            ERROR(logger) << "connect mysql failed";
            return false;
        }

#define XX(class_name, table_name)                             \
    if (data::class_name::CreateTableMySQL(mysql)) {           \
        ERROR(logger) << "create " table_name " table failed"; \
        return false;                                          \
    }
    XX(EmailVerificationInfoDao, "email_verification")
    XX(UserInfoDao, "user")
    XX(ArticleInfoDao, "article")
    XX(CategoryInfoDao, "category")
    XX(LabelInfoDao, "label")
    XX(ArticleCategoryRelInfoDao, "article_category_rel")
    XX(ArticleLabelRelInfoDao, "article_label_rel")
    XX(AssignmentInfoDao, "assignment")
    XX(OrganizationApplyInfoDao, "organization_apply")
    XX(OrganizationInfoDao, "organization")
    XX(OrganizationRepoPrsInfoDao, "organization_repo_prs")
    XX(OrganizationReposInfoDao, "organization_repos")
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
    XX(UserAiConfigInfoDao, "user_ai_config")
#undef XX

    // 数据库迁移：为已有表补充新增列
    {
        INFO(logger) << "migrate database begin";
#define XX(clazz) blog::data::clazz::MigrateTableMySQL(mysql);
        XX(EmailVerificationInfoDao)
        XX(UserInfoDao)
        XX(ArticleInfoDao)
        XX(CategoryInfoDao)
        XX(LabelInfoDao)
        XX(ArticleCategoryRelInfoDao)
        XX(ArticleLabelRelInfoDao)
        XX(AssignmentInfoDao)
        XX(OrganizationApplyInfoDao)
        XX(OrganizationInfoDao)
        XX(OrganizationRepoPrsInfoDao)
        XX(OrganizationReposInfoDao)
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
        XX(UserAiConfigInfoDao)
#undef XX
        INFO(logger) << "migrate database end";
    }

    }

    return true;
}

void BlogModule::registerServlets() {
    INFO(logger) << "registerServlets";

    for (auto& i : m_httpServers) {
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
        dp->addServlet("/api/v1/user/ai-config", XX(UserAIConfigServlet));
        dp->addServlet("/api/v1/user/admin/create", XX(UserAdminCreateServlet));
        dp->addServlet("/api/v1/user/admin/delete", XX(UserAdminDeleteServlet));
        dp->addServlet("/api/v1/user/admin/recover", XX(UserAdminRecoverServlet));
        dp->addServlet("/api/v1/user/admin/reset_passwd", XX(UserAdminResetPasswdServlet));
        dp->addServlet("/api/v1/user/admin/lists", XX(UserAdminListsServlet));
        dp->addServlet("/api/v1/user/admin/stats", XX(UserAdminStatsServlet));
        dp->addServlet("/api/v1/user/admin/update", XX(UserAdminUpdateServlet));
        dp->addServlet("/api/v1/user/admin/detail", XX(UserAdminDetailServlet));
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
        dp->addServlet("/api/v1/article/calendar", XX(ArticleCalendarServlet));
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
        dp->addServlet("/api/v1/article/ai-summary", XX(ArticleAISummaryServlet));
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
        // 错误码下发（公开接口）
        dp->addServlet("/api/v1/error-codes", XX(ErrorCodesServlet));
        // 站点公开配置
        dp->addServlet("/api/v1/site/settings", XX(SiteSettingsServlet));
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
        dp->addServlet("/api/v1/organization/apply-create", XX(OrganizationApplyCreateServlet));
        dp->addServlet("/api/v1/organization/apply-list", XX(OrganizationApplyListServlet));
        dp->addServlet("/api/v1/organization/apply-review", XX(OrganizationApplyReviewServlet));
        dp->addServlet("/api/v1/organization/my-applies", XX(OrganizationMyAppliesServlet));
        dp->addServlet("/api/v1/organization/stats", XX(OrganizationStatsServlet));
        dp->addServlet("/api/v1/organization/repos/prs", XX(OrganizationRepoPrsListServlet));
        dp->addServlet("/api/v1/organization/repos/prs/delete", XX(OrganizationRepoPrsDeleteServlet));
        dp->addServlet("/api/v1/organization/repos/prs/sync", XX(OrganizationRepoPrsSyncServlet));
        dp->addServlet("/api/v1/organization/repos/stats", XX(OrganizationReposStatsServlet));
        dp->addServlet("/api/v1/organization/repos/token", XX(OrganizationReposTokenServlet));
        dp->addServlet("/api/v1/organization/repos", XX(OrganizationReposListServlet));
        dp->addServlet("/api/v1/organization/repos/add", XX(OrganizationReposAddServlet));
        dp->addServlet("/api/v1/organization/repos/sync", XX(OrganizationReposSyncServlet));
        dp->addServlet("/api/v1/organization/repos/delete", XX(OrganizationReposDeleteServlet));
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
        dp->addServlet("/api/v1/rd/trends", XX(RdTrendServlet));
        dp->addServlet("/api/v1/rd/stats", XX(RdStatsServlet));
        dp->addServlet("/api/v1/rd/my-tickets", XX(RdMyTicketsServlet));
        dp->addServlet("/api/v1/rd/organizations", XX(RdOrganizationsServlet));
        dp->addServlet("/api/v1/rd/organizations/members", XX(RdOrganizationMembersServlet));
#undef XX
    }

}

void BlogModule::registerWSServlets() {
    INFO(logger) << "registerWSServlets";

    for (auto& i : m_wsServers) {
        auto ws = std::dynamic_pointer_cast<chen::http::WSServer>(i);
        ASSERT(ws);

        chen::http::ServletDispatch::ptr dp = ws->getWSServletDispatch();
        ASSERT(dp);

        servlet::NotifyServlet::ptr notify_servlet(std::make_shared<servlet::NotifyServlet>());
        dp->addServlet("/ws/v1/notification", notify_servlet);

        servlet::PresenceServlet::ptr presence_servlet(std::make_shared<servlet::PresenceServlet>());
        dp->addServlet("/ws/v1/presence", presence_servlet);
    }
}

void BlogModule::unregisterServlets() {
    for (auto& s : m_httpServers) {
        auto hs = std::dynamic_pointer_cast<chen::http::HttpServer>(s);
        if (hs) {
            hs->getServletDispatch()->clear();
        }
    }
    m_httpServers.clear();
}

void BlogModule::unregisterWSServlets() {
    for (auto& s : m_wsServers) {
        auto ws = std::dynamic_pointer_cast<chen::http::WSServer>(s);
        if (ws) {
            ws->getWSServletDispatch()->clear();
        }
    }
    m_wsServers.clear();
}

void BlogModule::unregisterRPCMethods() {
    std::vector<chen::rpc::RpcServer::ptr> rpc_servers;
    getAllRpcServer(rpc_servers);
    for (auto& s : rpc_servers) {
        if (!s) {
            continue;
        }
        s->unregisterMethod("GithubPRWebhook");
        s->unregisterMethod("GithubPRReviewWebhook");
        s->clearRegistrations();
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