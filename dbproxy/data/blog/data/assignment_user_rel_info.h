#ifndef BLOG_DATAASSIGNMENT_USER_REL_INFO_H
#define BLOG_DATAASSIGNMENT_USER_REL_INFO_H

#include <json/json.h>
#include <vector>
#include "chen/db/db.h"
#include "chen/util/util.h"


namespace blog {
namespace data {

class AssignmentUserRelInfoDao;
class AssignmentUserRelInfo {
friend class AssignmentUserRelInfoDao;
public:
    typedef std::shared_ptr<AssignmentUserRelInfo> ptr;

    AssignmentUserRelInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const int64_t& getAssignmentId() { return m_assignmentId; }
    void setAssignmentId(const int64_t& v);

    const int64_t& getUserId() { return m_userId; }
    void setUserId(const int64_t& v);

    const int32_t& getStatus() { return m_status; }
    void setStatus(const int32_t& v);

    const int32_t& getScore() { return m_score; }
    void setScore(const int32_t& v);

    const int64_t& getSubmitTime() { return m_submitTime; }
    void setSubmitTime(const int64_t& v);

    const int32_t& getIsDeleted() { return m_isDeleted; }
    void setIsDeleted(const int32_t& v);

    const int64_t& getCreateTime() { return m_createTime; }
    void setCreateTime(const int64_t& v);

    const int64_t& getUpdateTime() { return m_updateTime; }
    void setUpdateTime(const int64_t& v);

    std::string toJsonString() const;

private:
    int32_t m_status;
    int32_t m_score;
    int32_t m_isDeleted;
    int64_t m_id;
    int64_t m_assignmentId;
    int64_t m_userId;
    int64_t m_submitTime;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class AssignmentUserRelInfoDao {
public:
    typedef std::shared_ptr<AssignmentUserRelInfoDao> ptr;
    static int Update(AssignmentUserRelInfo::ptr info, chen::IDB::ptr conn);
    static int Insert(AssignmentUserRelInfo::ptr info, chen::IDB::ptr conn);
    static int InsertOrUpdate(AssignmentUserRelInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(AssignmentUserRelInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(const int64_t& id, chen::IDB::ptr conn);
    static int DeleteById( const int64_t& id, chen::IDB::ptr conn);
    static int DeleteByAssignmentIdUserId( const int64_t& assignment_id,  const int64_t& user_id, chen::IDB::ptr conn);
    static int DeleteByAssignmentId( const int64_t& assignment_id, chen::IDB::ptr conn);
    static int DeleteByUserId( const int64_t& user_id, chen::IDB::ptr conn);
    static int QueryAll(std::vector<AssignmentUserRelInfo::ptr>& results, chen::IDB::ptr conn);
    static AssignmentUserRelInfo::ptr Query( const int64_t& id, chen::IDB::ptr conn);
    static AssignmentUserRelInfo::ptr QueryByAssignmentIdUserId( const int64_t& assignment_id,  const int64_t& user_id, chen::IDB::ptr conn);
    static int QueryByAssignmentId(std::vector<AssignmentUserRelInfo::ptr>& results,  const int64_t& assignment_id, chen::IDB::ptr conn);
    static int QueryByAssignmentIdPages(std::vector<AssignmentUserRelInfo::ptr>& results, int64_t& total,  const int64_t& assignment_id, int32_t offset, int32_t limit, chen::IDB::ptr conn);
    static int QueryByUserId(std::vector<AssignmentUserRelInfo::ptr>& results,  const int64_t& user_id, chen::IDB::ptr conn);
    static int QueryByUserIdPages(std::vector<AssignmentUserRelInfo::ptr>& results, int64_t& total,  const int64_t& user_id, int32_t offset, int32_t limit, chen::IDB::ptr conn);
    static int CreateTableSQLite3(chen::IDB::ptr info);
    static int CreateTableMySQL(chen::IDB::ptr info);
    static int MigrateTableSQLite3(chen::IDB::ptr info);
    static int MigrateTableMySQL(chen::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //BLOG_DATAASSIGNMENT_USER_REL_INFO_H
