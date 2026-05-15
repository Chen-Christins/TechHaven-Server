#ifndef __BLOG_MANAGER_USER_FOLLOW_REL_MANAGER_H__
#define __BLOG_MANAGER_USER_FOLLOW_REL_MANAGER_H__

#include <shared_mutex>
#include <unordered_map>
#include <map>
#include "blog/data/user_follow_rel_info.h"
#include <chen/singleton.h>

namespace blog {

class UserFollowRelManager {
public:
    bool loadAll();
    void add(data::UserFollowRelInfo::ptr info);
    data::UserFollowRelInfo::ptr get(int64_t id);
    data::UserFollowRelInfo::ptr getByFollowerAndFollowing(int64_t follower_id, int64_t following_id);

    // follow/unfollow
    data::UserFollowRelInfo::ptr follow(int64_t follower_id, int64_t following_id);
    bool unfollow(int64_t follower_id, int64_t following_id);
    bool isFollowing(int64_t follower_id, int64_t following_id);

    // queries with pagination
    void listFollowing(std::vector<data::UserFollowRelInfo::ptr>& results,
        int64_t follower_id, uint64_t offset, uint64_t size);
    void listFollowers(std::vector<data::UserFollowRelInfo::ptr>& results,
        int64_t following_id, uint64_t offset, uint64_t size);

    int64_t countFollowing(int64_t follower_id);
    int64_t countFollowers(int64_t following_id);

private:
    std::shared_mutex m_mutex;
    std::unordered_map<int64_t, data::UserFollowRelInfo::ptr> m_datas;
    // follower_id -> (following_id -> info)
    std::unordered_map<int64_t, std::map<int64_t, data::UserFollowRelInfo::ptr>> m_followings;
    // following_id -> (follower_id -> info)
    std::unordered_map<int64_t, std::map<int64_t, data::UserFollowRelInfo::ptr>> m_followers;
};

typedef chen::Singleton<UserFollowRelManager> UserFollowRelMgr;

}

#endif // __BLOG_MANAGER_USER_FOLLOW_REL_MANAGER_H__
