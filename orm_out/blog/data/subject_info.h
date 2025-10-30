#ifndef BLOG_DATASUBJECT_INFO_H
#define BLOG_DATASUBJECT_INFO_H

#include <json/json.h>
#include <vector>
#include "chen/db/db.h"
#include "chen/util/util.h"


namespace blog {
namespace data {

class SubjectInfoDao;
class SubjectInfo {
friend class SubjectInfoDao;
public:
    typedef std::shared_ptr<SubjectInfo> ptr;

    SubjectInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const std::string& getName() { return m_name; }
    void setName(const std::string& v);

    const std::string& getColor() { return m_color; }
    void setColor(const std::string& v);

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
    std::string m_name;
    std::string m_color;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class SubjectInfoDao {
public:
    typedef std::shared_ptr<SubjectInfoDao> ptr;
    static int Update(SubjectInfo::ptr info, chen::IDB::ptr conn);
    static int Insert(SubjectInfo::ptr info, chen::IDB::ptr conn);
    static int InsertOrUpdate(SubjectInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(SubjectInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(const int64_t& id, chen::IDB::ptr conn);
    static int DeleteById( const int64_t& id, chen::IDB::ptr conn);
    static int DeleteByName( const std::string& name, chen::IDB::ptr conn);
    static int QueryAll(std::vector<SubjectInfo::ptr>& results, chen::IDB::ptr conn);
    static SubjectInfo::ptr Query( const int64_t& id, chen::IDB::ptr conn);
    static SubjectInfo::ptr QueryByName( const std::string& name, chen::IDB::ptr conn);
    static int CreateTableSQLite3(chen::IDB::ptr info);
    static int CreateTableMySQL(chen::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //BLOG_DATASUBJECT_INFO_H
