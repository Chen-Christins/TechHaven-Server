#ifndef BLOG_DATANOTIFICATION_INFO_H
#define BLOG_DATANOTIFICATION_INFO_H

#include <json/json.h>
#include <vector>
#include "chen/db/db.h"
#include "chen/util/util.h"


namespace blog {
namespace data {

class NotificationInfoDao;
class NotificationInfo {
friend class NotificationInfoDao;
public:
    typedef std::shared_ptr<NotificationInfo> ptr;

    NotificationInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const int64_t& getUserId() { return m_userId; }
    void setUserId(const int64_t& v);

    const std::string& getTitle() { return m_title; }
    void setTitle(const std::string& v);

    const std::string& getContent() { return m_content; }
    void setContent(const std::string& v);

    const std::string& getType() { return m_type; }
    void setType(const std::string& v);

    const int64_t& getSenderId() { return m_senderId; }
    void setSenderId(const int64_t& v);

    const int32_t& getIsRead() { return m_isRead; }
    void setIsRead(const int32_t& v);

    const int64_t& getReadTime() { return m_readTime; }
    void setReadTime(const int64_t& v);

    const int32_t& getIsDeleted() { return m_isDeleted; }
    void setIsDeleted(const int32_t& v);

    const int64_t& getCreateTime() { return m_createTime; }
    void setCreateTime(const int64_t& v);

    const int64_t& getUpdateTime() { return m_updateTime; }
    void setUpdateTime(const int64_t& v);

    std::string toJsonString() const;

private:
    int32_t m_isRead;
    int32_t m_isDeleted;
    int64_t m_id;
    int64_t m_userId;
    int64_t m_senderId;
    std::string m_title;
    std::string m_type;
    std::string m_content;
    int64_t m_readTime;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class NotificationInfoDao {
public:
    typedef std::shared_ptr<NotificationInfoDao> ptr;
    static int Update(NotificationInfo::ptr info, chen::IDB::ptr conn);
    static int Insert(NotificationInfo::ptr info, chen::IDB::ptr conn);
    static int InsertOrUpdate(NotificationInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(NotificationInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(const int64_t& id, chen::IDB::ptr conn);
    static int DeleteById( const int64_t& id, chen::IDB::ptr conn);
    static int DeleteByUserId( const int64_t& user_id, chen::IDB::ptr conn);
    static int DeleteByUserIdIsRead( const int64_t& user_id,  const int32_t& is_read, chen::IDB::ptr conn);
    static int QueryAll(std::vector<NotificationInfo::ptr>& results, chen::IDB::ptr conn);
    static NotificationInfo::ptr Query( const int64_t& id, chen::IDB::ptr conn);
    static int QueryByUserId(std::vector<NotificationInfo::ptr>& results,  const int64_t& user_id, chen::IDB::ptr conn);
    static int QueryByUserIdIsRead(std::vector<NotificationInfo::ptr>& results,  const int64_t& user_id,  const int32_t& is_read, chen::IDB::ptr conn);
    static int CreateTableSQLite3(chen::IDB::ptr info);
    static int CreateTableMySQL(chen::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //BLOG_DATANOTIFICATION_INFO_H
