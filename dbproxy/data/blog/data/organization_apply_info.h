#ifndef BLOG_DATAORGANIZATION_APPLY_INFO_H
#define BLOG_DATAORGANIZATION_APPLY_INFO_H

#include <json/json.h>
#include <vector>
#include "chen/db/db.h"
#include "chen/util/util.h"


namespace blog {
namespace data {

class OrganizationApplyInfoDao;
class OrganizationApplyInfo {
friend class OrganizationApplyInfoDao;
public:
    typedef std::shared_ptr<OrganizationApplyInfo> ptr;

    OrganizationApplyInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const int64_t& getUserId() { return m_userId; }
    void setUserId(const int64_t& v);

    const std::string& getOrgName() { return m_orgName; }
    void setOrgName(const std::string& v);

    const std::string& getOrgType() { return m_orgType; }
    void setOrgType(const std::string& v);

    const std::string& getOrgDescription() { return m_orgDescription; }
    void setOrgDescription(const std::string& v);

    const int32_t& getStatus() { return m_status; }
    void setStatus(const int32_t& v);

    const std::string& getReviewReason() { return m_reviewReason; }
    void setReviewReason(const std::string& v);

    const int64_t& getCreatedAt() { return m_createdAt; }
    void setCreatedAt(const int64_t& v);

    const int64_t& getReviewedAt() { return m_reviewedAt; }
    void setReviewedAt(const int64_t& v);

    const int32_t& getIsDeleted() { return m_isDeleted; }
    void setIsDeleted(const int32_t& v);

    std::string toJsonString() const;

private:
    int32_t m_status;
    int32_t m_isDeleted;
    int64_t m_id;
    int64_t m_userId;
    int64_t m_createdAt;
    int64_t m_reviewedAt;
    std::string m_orgName;
    std::string m_orgType;
    std::string m_orgDescription;
    std::string m_reviewReason;
};


class OrganizationApplyInfoDao {
public:
    typedef std::shared_ptr<OrganizationApplyInfoDao> ptr;
    static int Update(OrganizationApplyInfo::ptr info, chen::IDB::ptr conn);
    static int Insert(OrganizationApplyInfo::ptr info, chen::IDB::ptr conn);
    static int InsertOrUpdate(OrganizationApplyInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(OrganizationApplyInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(const int64_t& id, chen::IDB::ptr conn);
    static int DeleteById( const int64_t& id, chen::IDB::ptr conn);
    static int DeleteByUserId( const int64_t& user_id, chen::IDB::ptr conn);
    static int DeleteByStatus( const int32_t& status, chen::IDB::ptr conn);
    static int QueryAll(std::vector<OrganizationApplyInfo::ptr>& results, chen::IDB::ptr conn);
    static OrganizationApplyInfo::ptr Query( const int64_t& id, chen::IDB::ptr conn);
    static int QueryByUserId(std::vector<OrganizationApplyInfo::ptr>& results,  const int64_t& user_id, chen::IDB::ptr conn);
    static int QueryByStatus(std::vector<OrganizationApplyInfo::ptr>& results,  const int32_t& status, chen::IDB::ptr conn);
    static int CreateTableSQLite3(chen::IDB::ptr info);
    static int CreateTableMySQL(chen::IDB::ptr info);
    static int MigrateTableSQLite3(chen::IDB::ptr info);
    static int MigrateTableMySQL(chen::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //BLOG_DATAORGANIZATION_APPLY_INFO_H
