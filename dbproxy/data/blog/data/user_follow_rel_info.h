#ifndef BLOG_DATAUSER_FOLLOW_REL_INFO_H
#define BLOG_DATAUSER_FOLLOW_REL_INFO_H

#include <json/json.h>
#include <vector>
#include "chen/db/db.h"
#include "chen/util/util.h"


namespace blog {
namespace data {

class UserFollowRelInfoDao;
class UserFollowRelInfo {
friend class UserFollowRelInfoDao;
public:
    typedef std::shared_ptr<UserFollowRelInfo> ptr;

    UserFollowRelInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const int64_t& getFollowerId() { return m_followerId; }
    void setFollowerId(const int64_t& v);

    const int64_t& getFollowingId() { return m_followingId; }
    void setFollowingId(const int64_t& v);

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
    int64_t m_followerId;
    int64_t m_followingId;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class UserFollowRelInfoDao {
public:
    typedef std::shared_ptr<UserFollowRelInfoDao> ptr;
    static int Update(UserFollowRelInfo::ptr info, chen::IDB::ptr conn);
    static int Insert(UserFollowRelInfo::ptr info, chen::IDB::ptr conn);
    static int InsertOrUpdate(UserFollowRelInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(UserFollowRelInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(const int64_t& id, chen::IDB::ptr conn);
    static int DeleteById( const int64_t& id, chen::IDB::ptr conn);
    static int DeleteByFollowerIdFollowingId( const int64_t& follower_id,  const int64_t& following_id, chen::IDB::ptr conn);
    static int DeleteByFollowerId( const int64_t& follower_id, chen::IDB::ptr conn);
    static int DeleteByFollowingId( const int64_t& following_id, chen::IDB::ptr conn);
    static int QueryAll(std::vector<UserFollowRelInfo::ptr>& results, chen::IDB::ptr conn);
    static UserFollowRelInfo::ptr Query( const int64_t& id, chen::IDB::ptr conn);
    static UserFollowRelInfo::ptr QueryByFollowerIdFollowingId( const int64_t& follower_id,  const int64_t& following_id, chen::IDB::ptr conn);
    static int QueryByFollowerId(std::vector<UserFollowRelInfo::ptr>& results,  const int64_t& follower_id, chen::IDB::ptr conn);
    static int QueryByFollowingId(std::vector<UserFollowRelInfo::ptr>& results,  const int64_t& following_id, chen::IDB::ptr conn);
    static int CreateTableSQLite3(chen::IDB::ptr info);
    static int CreateTableMySQL(chen::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //BLOG_DATAUSER_FOLLOW_REL_INFO_H
