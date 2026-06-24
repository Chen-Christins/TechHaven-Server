#pragma once

#include "blog/data/organization_repos_info.h"
#include <chen/ds/lru_cache.h>
#include <chen/db/query_builder.h>
#include <chen/singleton.h>

namespace blog {

class OrganizationRepoManager {
public:
    OrganizationRepoManager();

    void add(data::OrganizationReposInfo::ptr info);
    void del(int64_t id);
    data::OrganizationReposInfo::ptr get(int64_t id);
    data::OrganizationReposInfo::ptr getByOrgAndName(int64_t org_id, const std::string& name);
    int64_t listByOrgPages(std::vector<data::OrganizationReposInfo::ptr>& repos
        , int64_t org_id, uint64_t offset, uint64_t limit);
    int64_t getCountByOrg(int64_t org_id);

    /// 从 GitHub API 异步同步仓库数据（stars_count、language、description）
    static void SyncFromGitHub(int64_t repo_id, const std::string& url, const std::string& token);

private:
    static data::OrganizationReposInfo::ptr parseRow(chen::ISQLData::ptr rt);
    void invalidateCountCache(int64_t org_id);

    chen::ds::HashLruCache<int64_t, data::OrganizationReposInfo::ptr> m_cache;
};

typedef chen::Singleton<OrganizationRepoManager> OrganizationRepoMgr;

}
