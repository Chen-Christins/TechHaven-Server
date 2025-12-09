#ifndef BLOG_DATAORGANIZATION_INFO_H
#define BLOG_DATAORGANIZATION_INFO_H

#include <json/json.h>
#include <vector>
#include "chen/db/db.h"
#include "chen/util/util.h"


namespace blog {
namespace data {

class OrganizationInfoDao;
class OrganizationInfo {
friend class OrganizationInfoDao;
public:
    typedef std::shared_ptr<OrganizationInfo> ptr;

    OrganizationInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const std::string& getName() { return m_name; }
    void setName(const std::string& v);

    const std::string& getType() { return m_type; }
    void setType(const std::string& v);

    const std::string& getDescription() { return m_description; }
    void setDescription(const std::string& v);

    const int64_t& getOwnerId() { return m_ownerId; }
    void setOwnerId(const int64_t& v);

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
    int64_t m_ownerId;
    std::string m_name;
    std::string m_type;
    std::string m_description;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class OrganizationInfoDao {
public:
    typedef std::shared_ptr<OrganizationInfoDao> ptr;
    static int Update(OrganizationInfo::ptr info, chen::IDB::ptr conn);
    static int Insert(OrganizationInfo::ptr info, chen::IDB::ptr conn);
    static int InsertOrUpdate(OrganizationInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(OrganizationInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(const int64_t& id, chen::IDB::ptr conn);
    static int DeleteById( const int64_t& id, chen::IDB::ptr conn);
    static int DeleteByOwnerId( const int64_t& owner_id, chen::IDB::ptr conn);
    static int DeleteByName( const std::string& name, chen::IDB::ptr conn);
    static int QueryAll(std::vector<OrganizationInfo::ptr>& results, chen::IDB::ptr conn);
    static OrganizationInfo::ptr Query( const int64_t& id, chen::IDB::ptr conn);
    static int QueryByOwnerId(std::vector<OrganizationInfo::ptr>& results,  const int64_t& owner_id, chen::IDB::ptr conn);
    static OrganizationInfo::ptr QueryByName( const std::string& name, chen::IDB::ptr conn);
    static int CreateTableSQLite3(chen::IDB::ptr info);
    static int CreateTableMySQL(chen::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //BLOG_DATAORGANIZATION_INFO_H
