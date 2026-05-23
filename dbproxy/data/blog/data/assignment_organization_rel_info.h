#ifndef BLOG_DATAASSIGNMENT_ORGANIZATION_REL_INFO_H
#define BLOG_DATAASSIGNMENT_ORGANIZATION_REL_INFO_H

#include <json/json.h>
#include <vector>
#include "chen/db/db.h"
#include "chen/util/util.h"


namespace blog {
namespace data {

class AssignmentOrganizationRelInfoDao;
class AssignmentOrganizationRelInfo {
friend class AssignmentOrganizationRelInfoDao;
public:
    typedef std::shared_ptr<AssignmentOrganizationRelInfo> ptr;

    AssignmentOrganizationRelInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const int64_t& getAssignmentId() { return m_assignmentId; }
    void setAssignmentId(const int64_t& v);

    const int64_t& getOrganizationId() { return m_organizationId; }
    void setOrganizationId(const int64_t& v);

    const std::string& getAssignedBy() { return m_assignedBy; }
    void setAssignedBy(const std::string& v);

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
    int32_t m_status;
    int32_t m_isDeleted;
    int64_t m_id;
    int64_t m_assignmentId;
    int64_t m_organizationId;
    std::string m_assignedBy;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class AssignmentOrganizationRelInfoDao {
public:
    typedef std::shared_ptr<AssignmentOrganizationRelInfoDao> ptr;
    static int Update(AssignmentOrganizationRelInfo::ptr info, chen::IDB::ptr conn);
    static int Insert(AssignmentOrganizationRelInfo::ptr info, chen::IDB::ptr conn);
    static int InsertOrUpdate(AssignmentOrganizationRelInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(AssignmentOrganizationRelInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(const int64_t& id, chen::IDB::ptr conn);
    static int DeleteById( const int64_t& id, chen::IDB::ptr conn);
    static int DeleteByAssignmentIdOrganizationId( const int64_t& assignment_id,  const int64_t& organization_id, chen::IDB::ptr conn);
    static int DeleteByAssignmentId( const int64_t& assignment_id, chen::IDB::ptr conn);
    static int DeleteByOrganizationId( const int64_t& organization_id, chen::IDB::ptr conn);
    static int QueryAll(std::vector<AssignmentOrganizationRelInfo::ptr>& results, chen::IDB::ptr conn);
    static AssignmentOrganizationRelInfo::ptr Query( const int64_t& id, chen::IDB::ptr conn);
    static AssignmentOrganizationRelInfo::ptr QueryByAssignmentIdOrganizationId( const int64_t& assignment_id,  const int64_t& organization_id, chen::IDB::ptr conn);
    static int QueryByAssignmentId(std::vector<AssignmentOrganizationRelInfo::ptr>& results,  const int64_t& assignment_id, chen::IDB::ptr conn);
    static int QueryByOrganizationId(std::vector<AssignmentOrganizationRelInfo::ptr>& results,  const int64_t& organization_id, chen::IDB::ptr conn);
    static int CreateTableSQLite3(chen::IDB::ptr info);
    static int CreateTableMySQL(chen::IDB::ptr info);
    static int MigrateTableSQLite3(chen::IDB::ptr info);
    static int MigrateTableMySQL(chen::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //BLOG_DATAASSIGNMENT_ORGANIZATION_REL_INFO_H
