#include "notification_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_NAME("system");

// ========== WS connection management ==========

void NotificationManager::addConnection(int64_t user_id, chen::http::WSSession::ptr session) {
    std::unique_lock<std::shared_mutex> lock(m_connMutex);
    m_connections[user_id] = session;
    INFO(logger) << "Notification WS connected: user_id=" << user_id;
}

void NotificationManager::removeConnection(int64_t user_id) {
    std::unique_lock<std::shared_mutex> lock(m_connMutex);
    m_connections.erase(user_id);
    INFO(logger) << "Notification WS disconnected: user_id=" << user_id;
}

int32_t NotificationManager::sendToUser(int64_t user_id, const std::string& message) {
    std::shared_lock<std::shared_mutex> lock(m_connMutex);
    auto it = m_connections.find(user_id);
    if (it == m_connections.end()) {
        return -1;
    }
    return it->second->sendMessage(message);
}

void NotificationManager::broadcast(const std::string& message) {
    std::shared_lock<std::shared_mutex> lock(m_connMutex);
    for (auto& [user_id, session] : m_connections) {
        session->sendMessage(message);
    }
}

bool NotificationManager::isConnected(int64_t user_id) {
    std::shared_lock<std::shared_mutex> lock(m_connMutex);
    return m_connections.find(user_id) != m_connections.end();
}

// ========== DB persistence ==========

bool NotificationManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<data::NotificationInfo::ptr> results;
    if (data::NotificationInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "NotificationManager loadAll fail";
        return false;
    }

    std::map<int64_t, data::NotificationInfo::ptr> datas;
    std::unordered_map<int64_t, std::map<int64_t, data::NotificationInfo::ptr>> userNotifications;

    for (auto& i : results) {
        datas[i->getId()] = i;
        userNotifications[i->getUserId()][i->getId()] = i;
    }

    std::unique_lock<std::shared_mutex> lock(m_dataMutex);
    m_datas.swap(datas);
    m_userNotifications.swap(userNotifications);
    return true;
}

data::NotificationInfo::ptr NotificationManager::addNotification(
    int64_t user_id, const std::string& title,
    const std::string& content, const std::string& type, int64_t sender_id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get SQLite3 connection fail";
        return nullptr;
    }

    auto info = std::make_shared<data::NotificationInfo>();
    info->setUserId(user_id);
    info->setTitle(title);
    info->setContent(content);
    info->setType(type);
    info->setSenderId(sender_id);
    info->setIsRead(0);
    info->setReadTime(0);
    info->setIsDeleted(0);
    info->setCreateTime(time(0));
    info->setUpdateTime(time(0));

    if (data::NotificationInfoDao::Insert(info, db)) {
        ERROR(logger) << "NotificationManager addNotification Insert fail";
        return nullptr;
    }

    {
        std::unique_lock<std::shared_mutex> lock(m_dataMutex);
        m_datas[info->getId()] = info;
        m_userNotifications[user_id][info->getId()] = info;
    }

    return info;
}

void NotificationManager::listByUser(std::vector<data::NotificationInfo::ptr>& results,
    int64_t user_id, uint64_t offset, uint64_t size) {
    std::shared_lock<std::shared_mutex> lock(m_dataMutex);
    auto it = m_userNotifications.find(user_id);
    if (it == m_userNotifications.end()) {
        return;
    }
    // m_userNotifications[user_id] is std::map<int64_t, ptr> ordered by id DESC
    auto& userMap = it->second;
    uint64_t idx = 0;
    for (auto rit = userMap.rbegin(); rit != userMap.rend(); ++rit) {
        if (idx >= offset && results.size() < size) {
            results.push_back(rit->second);
        }
        idx++;
        if (results.size() >= size) {
            break;
        }
    }
}

int64_t NotificationManager::unreadCount(int64_t user_id) {
    std::shared_lock<std::shared_mutex> lock(m_dataMutex);
    auto it = m_userNotifications.find(user_id);
    if (it == m_userNotifications.end()) {
        return 0;
    }
    int64_t count = 0;
    for (auto& [id, info] : it->second) {
        if (info->getIsRead() == 0 && info->getIsDeleted() == 0) {
            count++;
        }
    }
    return count;
}

bool NotificationManager::markRead(int64_t notification_id) {
    auto db = GetDB();
    if (!db) {
        return false;
    }

    data::NotificationInfo::ptr info;
    {
        std::shared_lock<std::shared_mutex> lock(m_dataMutex);
        auto it = m_datas.find(notification_id);
        if (it == m_datas.end()) {
            return false;
        }
        info = it->second;
    }

    info->setIsRead(1);
    info->setReadTime(time(0));
    if (data::NotificationInfoDao::Update(info, db)) {
        return false;
    }
    return true;
}

bool NotificationManager::markRead(const std::vector<int64_t>& ids) {
    auto db = GetDB();
    if (!db) {
        return false;
    }

    for (auto id : ids) {
        data::NotificationInfo::ptr info;
        {
            std::shared_lock<std::shared_mutex> lock(m_dataMutex);
            auto it = m_datas.find(id);
            if (it == m_datas.end()) {
                continue;
            }
            info = it->second;
        }
        info->setIsRead(1);
        info->setReadTime(time(0));
        data::NotificationInfoDao::Update(info, db);
    }
    return true;
}

}
