#ifndef BLOG_DATAORGANIZATION_USER_REL_INFO_H
#define BLOG_DATAORGANIZATION_USER_REL_INFO_H

#include <json/json.h>
#include <vector>
#include "chen/db/db.h"
#include "chen/util/util.h"


namespace blog {
namespace data {

class OrganizationUserRelInfoDao;
class OrganizationUserRelInfo {
friend class OrganizationUserRelInfoDao;
public:
    typedef std::shared_ptr<OrganizationUserRelInfo> ptr;

    OrganizationUserRelInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const int64_t& getOrgId() { return m_orgId; }
    void setOrgId(const int64_t& v);

    const int64_t& getUserId() { return m_userId; }
    void setUserId(const int64_t& v);

    const int32_t& getRole() { return m_role; }
    void setRole(const int32_t& v);

    const int32_t& getStatus() { return m_status; }
    void setStatus(const int32_t& v);

    const int32_t& getIsDeleted() { return m_isDeleted; }
    void setIsDeleted(const int32_t& v);

    const int64_t& getCreateTime() { return m_createTime; }
    void setCreateTime(const int64_t& v);

    const int64_t& getUpdateTime() { return m_updateTime; }
    void setUpdateTime(const int64_t& v);

    std::string toJsonString() const;

private:
    int32_t m_role;
    int32_t m_status;
    int32_t m_isDeleted;
    int64_t m_id;
    int64_t m_orgId;
    int64_t m_userId;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class OrganizationUserRelInfoDao {
public:
    typedef std::shared_ptr<OrganizationUserRelInfoDao> ptr;
    static int Update(OrganizationUserRelInfo::ptr info, chen::IDB::ptr conn);
    static int Insert(OrganizationUserRelInfo::ptr info, chen::IDB::ptr conn);
    static int InsertOrUpdate(OrganizationUserRelInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(OrganizationUserRelInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(const int64_t& id, chen::IDB::ptr conn);
    static int DeleteById( const int64_t& id, chen::IDB::ptr conn);
    static int DeleteByOrgIdUserId( const int64_t& org_id,  const int64_t& user_id, chen::IDB::ptr conn);
    static int DeleteByUserId( const int64_t& user_id, chen::IDB::ptr conn);
    static int QueryAll(std::vector<OrganizationUserRelInfo::ptr>& results, chen::IDB::ptr conn);
    static OrganizationUserRelInfo::ptr Query( const int64_t& id, chen::IDB::ptr conn);
    static OrganizationUserRelInfo::ptr QueryByOrgIdUserId( const int64_t& org_id,  const int64_t& user_id, chen::IDB::ptr conn);
    static int QueryByUserId(std::vector<OrganizationUserRelInfo::ptr>& results,  const int64_t& user_id, chen::IDB::ptr conn);
    static int QueryByUserIdPages(std::vector<OrganizationUserRelInfo::ptr>& results, int64_t& total,  const int64_t& user_id, int32_t offset, int32_t limit, chen::IDB::ptr conn);
    static int CreateTableSQLite3(chen::IDB::ptr info);
    static int CreateTableMySQL(chen::IDB::ptr info);
    static int MigrateTableSQLite3(chen::IDB::ptr info);
    static int MigrateTableMySQL(chen::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //BLOG_DATAORGANIZATION_USER_REL_INFO_H
