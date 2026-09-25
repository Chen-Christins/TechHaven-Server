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

    void onTick();
    
    data::OrganizationRepoPrsInfo::ptr get(int64_t id);
    
    data::OrganizationRepoPrsInfo::ptr getByRepoAndPrId(int64_t repo_id, int32_t github_pr_id);

    int64_t listByRepoPages(std::vector<data::OrganizationRepoPrsInfo::ptr>& prs
        , int64_t repo_id, const std::string& state, uint64_t offset, uint64_t limit);

    int64_t getCountByRepo(int64_t repo_id);

    int64_t listByOrgPages(std::vector<data::OrganizationRepoPrsInfo::ptr>& prs
        , int64_t org_id, const std::string& state, uint64_t offset, uint64_t limit);

    int64_t listByUserPages(std::vector<data::OrganizationRepoPrsInfo::ptr>& prs
        , int64_t uid, const std::string& state, uint64_t offset, uint64_t limit);

    /// 从 GitHub API 同步某个仓库的 PR
    /// @param max_prs 最多同步的 PR 条数，0 表示全量同步
    static void SyncFromGitHub(int64_t repo_id, const std::string& repo_url, const std::string& token, int max_prs = 0);

    static data::OrganizationReposInfo::ptr findRepoByOwnerAndName(const std::string& owner, const std::string& name);

private:
    static data::OrganizationRepoPrsInfo::ptr parseRow(chen::ISQLData::ptr rt);

    chen::ds::HashLruCache<int64_t, data::OrganizationRepoPrsInfo::ptr> m_cache;
};

typedef chen::Singleton<OrganizationRepoPrManager> OrganizationRepoPrMgr;

}
