#include "user_follow_rel_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

bool UserFollowRelManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "get db connection fail";
        return false;
    }
    std::vector<data::UserFollowRelInfo::ptr> results;
    if (data::UserFollowRelInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "UserFollowRelManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, data::UserFollowRelInfo::ptr> datas;
    std::unordered_map<int64_t, std::map<int64_t, data::UserFollowRelInfo::ptr>> followings;
    std::unordered_map<int64_t, std::map<int64_t, data::UserFollowRelInfo::ptr>> followers;
    for (auto& i : results) {
        datas[i->getId()] = i;
        followings[i->getFollowerId()][i->getFollowingId()] = i;
        followers[i->getFollowingId()][i->getFollowerId()] = i;
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_followings.swap(followings);
    m_followers.swap(followers);

    return true;
}

void UserFollowRelManager::add(data::UserFollowRelInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
    m_followings[info->getFollowerId()][info->getFollowingId()] = info;
    m_followers[info->getFollowingId()][info->getFollowerId()] = info;
}

data::UserFollowRelInfo::ptr UserFollowRelManager::get(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    return it == m_datas.end() ? nullptr : it->second;
}

data::UserFollowRelInfo::ptr UserFollowRelManager::getByFollowerAndFollowing(
    int64_t follower_id, int64_t following_id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_followings.find(follower_id);
    if (it != m_followings.end()) {
        auto iit = it->second.find(following_id);
        return iit == it->second.end() ? nullptr : iit->second;
    }
    return nullptr;
}

data::UserFollowRelInfo::ptr UserFollowRelManager::follow(int64_t follower_id, int64_t following_id) {
    // check if already following
    auto existing = getByFollowerAndFollowing(follower_id, following_id);
    if (existing) {
        if (!existing->getIsDeleted()) {
            return existing; // already following
        }
        // re-follow: update existing record
        auto db = GetDB();
        if (!db) {
            ERROR(logger) << "get db connection fail";
            return nullptr;
        }
        existing->setIsDeleted(0);
        existing->setUpdateTime(time(0));
        if (data::UserFollowRelInfoDao::Update(existing, db)) {
            ERROR(logger) << "UserFollowRelManager follow Update fail";
            return nullptr;
        }
        // update in-memory maps
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        existing->setIsDeleted(0);
        return existing;
    }

    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "get db connection fail";
        return nullptr;
    }

    auto info = std::make_shared<data::UserFollowRelInfo>();
    info->setFollowerId(follower_id);
    info->setFollowingId(following_id);
    info->setIsDeleted(0);
    info->setCreateTime(time(0));
    info->setUpdateTime(time(0));

    if (data::UserFollowRelInfoDao::Insert(info, db)) {
        ERROR(logger) << "UserFollowRelManager follow Insert fail";
        return nullptr;
    }

    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_datas[info->getId()] = info;
        m_followings[info->getFollowerId()][info->getFollowingId()] = info;
        m_followers[info->getFollowingId()][info->getFollowerId()] = info;
    }

    return info;
}

bool UserFollowRelManager::unfollow(int64_t follower_id, int64_t following_id) {
    auto info = getByFollowerAndFollowing(follower_id, following_id);
    if (!info || info->getIsDeleted()) {
        return false;
    }

    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "get db connection fail";
        return false;
    }

    info->setIsDeleted(1);
    info->setUpdateTime(time(0));
    if (data::UserFollowRelInfoDao::Update(info, db)) {
        ERROR(logger) << "UserFollowRelManager unfollow Update fail";
        return false;
    }
    return true;
}

bool UserFollowRelManager::isFollowing(int64_t follower_id, int64_t following_id) {
    auto info = getByFollowerAndFollowing(follower_id, following_id);
    return info && !info->getIsDeleted();
}

void UserFollowRelManager::listFollowing(std::vector<data::UserFollowRelInfo::ptr>& results,
    int64_t follower_id, uint64_t offset, uint64_t size) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_followings.find(follower_id);
    if (it == m_followings.end()) {
        return;
    }
    auto& followMap = it->second;
    uint64_t idx = 0;
    for (auto rit = followMap.rbegin(); rit != followMap.rend(); ++rit) {
        if (rit->second->getIsDeleted()) {
            continue;
        }
        if (idx >= offset && results.size() < size) {
            results.push_back(rit->second);
        }
        idx++;
        if (results.size() >= size) {
            break;
        }
    }
}

void UserFollowRelManager::listFollowers(std::vector<data::UserFollowRelInfo::ptr>& results,
    int64_t following_id, uint64_t offset, uint64_t size) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_followers.find(following_id);
    if (it == m_followers.end()) {
        return;
    }
    auto& followMap = it->second;
    uint64_t idx = 0;
    for (auto rit = followMap.rbegin(); rit != followMap.rend(); ++rit) {
        if (rit->second->getIsDeleted()) {
            continue;
        }
        if (idx >= offset && results.size() < size) {
            results.push_back(rit->second);
        }
        idx++;
        if (results.size() >= size) {
            break;
        }
    }
}

int64_t UserFollowRelManager::countFollowing(int64_t follower_id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_followings.find(follower_id);
    if (it == m_followings.end()) {
        return 0;
    }
    int64_t count = 0;
    for (auto& [id, info] : it->second) {
        if (!info->getIsDeleted()) {
            count++;
        }
    }
    return count;
}

int64_t UserFollowRelManager::countFollowers(int64_t following_id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_followers.find(following_id);
    if (it == m_followers.end()) {
        return 0;
    }
    int64_t count = 0;
    for (auto& [id, info] : it->second) {
        if (!info->getIsDeleted()) {
            count++;
        }
    }
    return count;
}

}
