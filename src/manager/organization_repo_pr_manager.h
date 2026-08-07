/**
 * @file organization_repo_pr_manager.h
 * @brief 组织仓库 PR 管理器
 * @author Christins
 * @date 2026-06-24
 * @copyright Apache 2.0
 */
#pragma once

#include "blog/data/organization_repo_prs_info.h"
#include "blog/data/organization_repos_info.h"

#include <chen/ds/lru_cache.h>
#include <chen/db/query_builder.h>
#include <chen/util/singleton.h>

// 前置声明 RPC 协议结构体，避免在 header 中引入 protocol 头文件
struct tagGithubPRInfo;
struct tagGithubPRReviewInfo;

namespace blog {

class OrganizationRepoPrManager {
public:
    OrganizationRepoPrManager();

    void add(data::OrganizationRepoPrsInfo::ptr info);

    void del(int64_t id);
    
    data::OrganizationRepoPrsInfo::ptr get(int64_t id);
    
    data::OrganizationRepoPrsInfo::ptr getByRepoAndPrId(int64_t repo_id, int32_t github_pr_id);

    int64_t listByRepoPages(std::vector<data::OrganizationRepoPrsInfo::ptr>& prs
        , int64_t repo_id, const std::string& state, uint64_t offset, uint64_t limit);

    int64_t getCountByRepo(int64_t repo_id);

    int64_t listByOrgPages(std::vector<data::OrganizationRepoPrsInfo::ptr>& prs
        , int64_t org_id, const std::string& state, uint64_t offset, uint64_t limit);

    int64_t listByUserPages(std::vector<data::OrganizationRepoPrsInfo::ptr>& prs
        , int64_t uid, const std::string& state, uint64_t offset, uint64_t limit);

    /// 从 GitHub API 全量同步某个仓库的 PR
    static void SyncFromGitHub(int64_t repo_id, const std::string& repo_url, const std::string& token);

    // ==================== RPC Webhook 处理 ====================

    /**
     * @brief 处理来自 webhook 服务的 PR 事件（pull_request webhook）
     * @param info 通过 RPC 传入的 PR 事件数据
     * @return 0 成功，负数失败
     */
    static int32_t HandlePRWebhook(const tagGithubPRInfo& info);

    /**
     * @brief 处理来自 webhook 服务的 PR Review 事件（pull_request_review webhook）
     * @param info 通过 RPC 传入的 Review 事件数据
     * @return 0 成功，负数失败
     */
    static int32_t HandlePRReviewWebhook(const tagGithubPRReviewInfo& info);

private:
    static data::OrganizationRepoPrsInfo::ptr parseRow(chen::ISQLData::ptr rt);

    /// 根据 RepoOwner/RepoName 查找本地仓库记录
    static data::OrganizationReposInfo::ptr findRepoByOwnerAndName(const std::string& owner, const std::string& name);

    chen::ds::HashLruCache<int64_t, data::OrganizationRepoPrsInfo::ptr> m_cache;
};

typedef chen::Singleton<OrganizationRepoPrManager> OrganizationRepoPrMgr;

}
