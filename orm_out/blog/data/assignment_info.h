#ifndef BLOG_DATAASSIGNMENT_INFO_H
#define BLOG_DATAASSIGNMENT_INFO_H

#include <json/json.h>
#include <vector>
#include "chen/db/db.h"
#include "chen/util/util.h"


namespace blog {
namespace data {

class AssignmentInfoDao;
class AssignmentInfo {
friend class AssignmentInfoDao;
public:
    typedef std::shared_ptr<AssignmentInfo> ptr;

    AssignmentInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const int64_t& getSubjectId() { return m_subjectId; }
    void setSubjectId(const int64_t& v);

    const std::string& getName() { return m_name; }
    void setName(const std::string& v);

    const std::string& getColor() { return m_color; }
    void setColor(const std::string& v);

    const std::string& getContent() { return m_content; }
    void setContent(const std::string& v);

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
    int32_t m_isDeleted;
    int64_t m_id;
    int64_t m_subjectId;
    std::string m_name;
    std::string m_color;
    std::string m_content;
    int64_t m_deadline;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class AssignmentInfoDao {
public:
    typedef std::shared_ptr<AssignmentInfoDao> ptr;
    static int Update(AssignmentInfo::ptr info, chen::IDB::ptr conn);
    static int Insert(AssignmentInfo::ptr info, chen::IDB::ptr conn);
    static int InsertOrUpdate(AssignmentInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(AssignmentInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(const int64_t& id, chen::IDB::ptr conn);
    static int DeleteById( const int64_t& id, chen::IDB::ptr conn);
    static int DeleteBySubjectId( const int64_t& subject_id, chen::IDB::ptr conn);
    static int DeleteBySubjectIdName( const int64_t& subject_id,  const std::string& name, chen::IDB::ptr conn);
    static int QueryAll(std::vector<AssignmentInfo::ptr>& results, chen::IDB::ptr conn);
    static AssignmentInfo::ptr Query( const int64_t& id, chen::IDB::ptr conn);
    static int QueryBySubjectId(std::vector<AssignmentInfo::ptr>& results,  const int64_t& subject_id, chen::IDB::ptr conn);
    static AssignmentInfo::ptr QueryBySubjectIdName( const int64_t& subject_id,  const std::string& name, chen::IDB::ptr conn);
    static int CreateTableSQLite3(chen::IDB::ptr info);
    static int CreateTableMySQL(chen::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //BLOG_DATAASSIGNMENT_INFO_H
