#ifndef BLOG_DATACATEGORY_INFO_H
#define BLOG_DATACATEGORY_INFO_H

#include <json/json.h>
#include <vector>
#include "chen/db/db.h"
#include "chen/util/util.h"


namespace blog {
namespace data {

class CategoryInfoDao;
class CategoryInfo {
friend class CategoryInfoDao;
public:
    typedef std::shared_ptr<CategoryInfo> ptr;

    CategoryInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const std::string& getName() { return m_name; }
    void setName(const std::string& v);

    const std::string& getColor() { return m_color; }
    void setColor(const std::string& v);

    const std::string& getDescription() { return m_description; }
    void setDescription(const std::string& v);

    const std::string& getUrl() { return m_url; }
    void setUrl(const std::string& v);

    const std::string& getIcon() { return m_icon; }
    void setIcon(const std::string& v);

    const int64_t& getParentId() { return m_parentId; }
    void setParentId(const int64_t& v);

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
    int64_t m_parentId;
    std::string m_name;
    std::string m_color;
    std::string m_description;
    std::string m_url;
    std::string m_icon;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class CategoryInfoDao {
public:
    typedef std::shared_ptr<CategoryInfoDao> ptr;
    static int Update(CategoryInfo::ptr info, chen::IDB::ptr conn);
    static int Insert(CategoryInfo::ptr info, chen::IDB::ptr conn);
    static int InsertOrUpdate(CategoryInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(CategoryInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(const int64_t& id, chen::IDB::ptr conn);
    static int DeleteById( const int64_t& id, chen::IDB::ptr conn);
    static int DeleteByName( const std::string& name, chen::IDB::ptr conn);
    static int QueryAll(std::vector<CategoryInfo::ptr>& results, chen::IDB::ptr conn);
    static CategoryInfo::ptr Query( const int64_t& id, chen::IDB::ptr conn);
    static CategoryInfo::ptr QueryByName( const std::string& name, chen::IDB::ptr conn);
    static int CreateTableSQLite3(chen::IDB::ptr info);
    static int CreateTableMySQL(chen::IDB::ptr info);
    static int MigrateTableSQLite3(chen::IDB::ptr info);
    static int MigrateTableMySQL(chen::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //BLOG_DATACATEGORY_INFO_H
