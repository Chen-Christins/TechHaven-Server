#pragma once

#include <shared_mutex>
#include "blog/data/organization_user_rel_info.h"
#include <chen/ds/lru_cache.h>
#include <chen/db/query_builder.h>
#include <chen/singleton.h>

namespace blog {

class OrganizationUserRelManager {
public:
    enum Status {
        PENDING = 0,
        APPROVED = 1,
        REJECTED = 2,
        EXITED = 3
    };

    OrganizationUserRelManager();

    void add(data::OrganizationUserRelInfo::ptr info);
    data::OrganizationUserRelInfo::ptr get(int64_t id);
    data::OrganizationUserRelInfo::ptr getByOrgAndUser(int64_t o_id, int64_t u_id);
    int64_t getByPages(std::vector<data::OrganizationUserRelInfo::ptr>& results
        , int64_t o_id, uint64_t offset, uint64_t size, int32_t status, bool isValid);

    int64_t getOrgByUserId(std::vector<data::OrganizationUserRelInfo::ptr>& results
        , int64_t u_id, int32_t status, bool isValid);

    int64_t getMemberCount(int64_t o_id, int32_t status, bool isValid);

private:
    static data::OrganizationUserRelInfo::ptr parseRow(chen::ISQLData::ptr rt);

    std::shared_mutex m_mutex;
    chen::ds::LruCache<int64_t, data::OrganizationUserRelInfo::ptr> m_cache;
};

typedef chen::Singleton<OrganizationUserRelManager> OrganizationUserRelMgr;

}
