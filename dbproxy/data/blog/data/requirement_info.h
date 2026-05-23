#ifndef BLOG_DATAREQUIREMENT_INFO_H
#define BLOG_DATAREQUIREMENT_INFO_H

#include <json/json.h>
#include <vector>
#include "chen/db/db.h"
#include "chen/util/util.h"


namespace blog {
namespace data {

class RequirementInfoDao;
class RequirementInfo {
friend class RequirementInfoDao;
public:
    typedef std::shared_ptr<RequirementInfo> ptr;

    RequirementInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const int64_t& getOrgId() { return m_orgId; }
    void setOrgId(const int64_t& v);

    const std::string& getTitle() { return m_title; }
    void setTitle(const std::string& v);

    const std::string& getDescription() { return m_description; }
    void setDescription(const std::string& v);

    const int32_t& getPriority() { return m_priority; }
    void setPriority(const int32_t& v);

    const int32_t& getStatus() { return m_status; }
    void setStatus(const int32_t& v);

    const int64_t& getCreatorId() { return m_creatorId; }
    void setCreatorId(const int64_t& v);

    const int64_t& getAssigneeId() { return m_assigneeId; }
    void setAssigneeId(const int64_t& v);

    const std::string& getIteration() { return m_iteration; }
    void setIteration(const std::string& v);

    const std::string& getCategory() { return m_category; }
    void setCategory(const std::string& v);

    const std::string& getSource() { return m_source; }
    void setSource(const std::string& v);

    const int64_t& getDeadline() { return m_deadline; }
    void setDeadline(const int64_t& v);

    const int32_t& getIsDeleted() { return m_isDeleted; }
    void setIsDeleted(const int32_t& v);

    const int64_t& getCreateTime() { return m_createTime; }
    void setCreateTime(const int64_t& v);

    const int64_t& getUpdateTime() { return m_updateTime; }
    void setUpdateTime(const int64_t& v);

    std::string toJsonString() const;

private:
    int32_t m_priority;
    int32_t m_status;
    int32_t m_isDeleted;
    int64_t m_id;
    int64_t m_orgId;
    int64_t m_creatorId;
    int64_t m_assigneeId;
    std::string m_title;
    std::string m_description;
    std::string m_iteration;
    std::string m_category;
    std::string m_source;
    int64_t m_deadline;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class RequirementInfoDao {
public:
    typedef std::shared_ptr<RequirementInfoDao> ptr;
    static int Update(RequirementInfo::ptr info, chen::IDB::ptr conn);
    static int Insert(RequirementInfo::ptr info, chen::IDB::ptr conn);
    static int InsertOrUpdate(RequirementInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(RequirementInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(const int64_t& id, chen::IDB::ptr conn);
    static int DeleteById( const int64_t& id, chen::IDB::ptr conn);
    static int DeleteByOrgId( const int64_t& org_id, chen::IDB::ptr conn);
    static int DeleteByCreatorId( const int64_t& creator_id, chen::IDB::ptr conn);
    static int DeleteByAssigneeId( const int64_t& assignee_id, chen::IDB::ptr conn);
    static int QueryAll(std::vector<RequirementInfo::ptr>& results, chen::IDB::ptr conn);
    static RequirementInfo::ptr Query( const int64_t& id, chen::IDB::ptr conn);
    static int QueryByOrgId(std::vector<RequirementInfo::ptr>& results,  const int64_t& org_id, chen::IDB::ptr conn);
    static int QueryByCreatorId(std::vector<RequirementInfo::ptr>& results,  const int64_t& creator_id, chen::IDB::ptr conn);
    static int QueryByAssigneeId(std::vector<RequirementInfo::ptr>& results,  const int64_t& assignee_id, chen::IDB::ptr conn);
    static int CreateTableSQLite3(chen::IDB::ptr info);
    static int CreateTableMySQL(chen::IDB::ptr info);
    static int MigrateTableSQLite3(chen::IDB::ptr info);
    static int MigrateTableMySQL(chen::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //BLOG_DATAREQUIREMENT_INFO_H
