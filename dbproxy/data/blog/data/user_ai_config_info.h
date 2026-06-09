#ifndef BLOG_DATAUSER_AI_CONFIG_INFO_H
#define BLOG_DATAUSER_AI_CONFIG_INFO_H

#include <json/json.h>
#include <vector>
#include "chen/db/db.h"
#include "chen/util/util.h"


namespace blog {
namespace data {

class UserAiConfigInfoDao;
class UserAiConfigInfo {
friend class UserAiConfigInfoDao;
public:
    typedef std::shared_ptr<UserAiConfigInfo> ptr;

    UserAiConfigInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const int64_t& getUserId() { return m_userId; }
    void setUserId(const int64_t& v);

    const std::string& getType() { return m_type; }
    void setType(const std::string& v);

    const std::string& getUrl() { return m_url; }
    void setUrl(const std::string& v);

    const std::string& getApiKey() { return m_apiKey; }
    void setApiKey(const std::string& v);

    const std::string& getModel() { return m_model; }
    void setModel(const std::string& v);

    const int32_t& getMaxTokens() { return m_maxTokens; }
    void setMaxTokens(const int32_t& v);

    const int64_t& getCreateTime() { return m_createTime; }
    void setCreateTime(const int64_t& v);

    const int64_t& getUpdateTime() { return m_updateTime; }
    void setUpdateTime(const int64_t& v);

    std::string toJsonString() const;

private:
    int32_t m_maxTokens;
    int64_t m_id;
    int64_t m_userId;
    std::string m_type;
    std::string m_url;
    std::string m_apiKey;
    std::string m_model;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class UserAiConfigInfoDao {
public:
    typedef std::shared_ptr<UserAiConfigInfoDao> ptr;
    static int Update(UserAiConfigInfo::ptr info, chen::IDB::ptr conn);
    static int Insert(UserAiConfigInfo::ptr info, chen::IDB::ptr conn);
    static int InsertOrUpdate(UserAiConfigInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(UserAiConfigInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(const int64_t& id, chen::IDB::ptr conn);
    static int DeleteById( const int64_t& id, chen::IDB::ptr conn);
    static int DeleteByUserId( const int64_t& user_id, chen::IDB::ptr conn);
    static int QueryAll(std::vector<UserAiConfigInfo::ptr>& results, chen::IDB::ptr conn);
    static UserAiConfigInfo::ptr Query( const int64_t& id, chen::IDB::ptr conn);
    static UserAiConfigInfo::ptr QueryByUserId( const int64_t& user_id, chen::IDB::ptr conn);
    static int CreateTableSQLite3(chen::IDB::ptr info);
    static int CreateTableMySQL(chen::IDB::ptr info);
    static int MigrateTableSQLite3(chen::IDB::ptr info);
    static int MigrateTableMySQL(chen::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //BLOG_DATAUSER_AI_CONFIG_INFO_H
