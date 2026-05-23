#ifndef BLOG_DATABUG_INFO_H
#define BLOG_DATABUG_INFO_H

#include <json/json.h>
#include <vector>
#include "chen/db/db.h"
#include "chen/util/util.h"


namespace blog {
namespace data {

class BugInfoDao;
class BugInfo {
friend class BugInfoDao;
public:
    typedef std::shared_ptr<BugInfo> ptr;

    BugInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const int64_t& getOrgId() { return m_orgId; }
    void setOrgId(const int64_t& v);

    const std::string& getTitle() { return m_title; }
    void setTitle(const std::string& v);

    const std::string& getDescription() { return m_description; }
    void setDescription(const std::string& v);

    const int32_t& getSeverity() { return m_severity; }
    void setSeverity(const int32_t& v);

    const int32_t& getPriority() { return m_priority; }
    void setPriority(const int32_t& v);

    const int32_t& getStatus() { return m_status; }
    void setStatus(const int32_t& v);

    const int64_t& getCreatorId() { return m_creatorId; }
    void setCreatorId(const int64_t& v);

    const int64_t& getAssigneeId() { return m_assigneeId; }
    void setAssigneeId(const int64_t& v);

    const int64_t& getRequirementId() { return m_requirementId; }
    void setRequirementId(const int64_t& v);

    const std::string& getModule() { return m_module; }
    void setModule(const std::string& v);

    const std::string& getStepsToReproduce() { return m_stepsToReproduce; }
    void setStepsToReproduce(const std::string& v);

    const std::string& getEnvironment() { return m_environment; }
    void setEnvironment(const std::string& v);

    const int32_t& getIsDeleted() { return m_isDeleted; }
    void setIsDeleted(const int32_t& v);

    const int64_t& getCreateTime() { return m_createTime; }
    void setCreateTime(const int64_t& v);

    const int64_t& getUpdateTime() { return m_updateTime; }
    void setUpdateTime(const int64_t& v);

    std::string toJsonString() const;

private:
    int32_t m_severity;
    int32_t m_priority;
    int32_t m_status;
    int32_t m_isDeleted;
    int64_t m_id;
    int64_t m_orgId;
    int64_t m_creatorId;
    int64_t m_assigneeId;
    int64_t m_requirementId;
    std::string m_title;
    std::string m_description;
    std::string m_module;
    std::string m_stepsToReproduce;
    std::string m_environment;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class BugInfoDao {
public:
    typedef std::shared_ptr<BugInfoDao> ptr;
    static int Update(BugInfo::ptr info, chen::IDB::ptr conn);
    static int Insert(BugInfo::ptr info, chen::IDB::ptr conn);
    static int InsertOrUpdate(BugInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(BugInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(const int64_t& id, chen::IDB::ptr conn);
    static int DeleteById( const int64_t& id, chen::IDB::ptr conn);
    static int DeleteByOrgId( const int64_t& org_id, chen::IDB::ptr conn);
    static int DeleteByCreatorId( const int64_t& creator_id, chen::IDB::ptr conn);
    static int DeleteByAssigneeId( const int64_t& assignee_id, chen::IDB::ptr conn);
    static int QueryAll(std::vector<BugInfo::ptr>& results, chen::IDB::ptr conn);
    static BugInfo::ptr Query( const int64_t& id, chen::IDB::ptr conn);
    static int QueryByOrgId(std::vector<BugInfo::ptr>& results,  const int64_t& org_id, chen::IDB::ptr conn);
    static int QueryByCreatorId(std::vector<BugInfo::ptr>& results,  const int64_t& creator_id, chen::IDB::ptr conn);
    static int QueryByAssigneeId(std::vector<BugInfo::ptr>& results,  const int64_t& assignee_id, chen::IDB::ptr conn);
    static int CreateTableSQLite3(chen::IDB::ptr info);
    static int CreateTableMySQL(chen::IDB::ptr info);
    static int MigrateTableSQLite3(chen::IDB::ptr info);
    static int MigrateTableMySQL(chen::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //BLOG_DATABUG_INFO_H
