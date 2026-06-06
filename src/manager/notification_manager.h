#pragma once

#include <chen/http/ws_session.h>
#include <chen/singleton.h>
#include <chen/ds/lru_cache.h>
#include <chen/db/query_builder.h>
#include <unordered_map>
#include <shared_mutex>
#include "blog/data/notification_info.h"

namespace blog {

class NotificationManager {
public:
    NotificationManager();

    // WS connection management
    void addConnection(int64_t user_id, chen::http::WSSession::ptr session);
    void removeConnection(int64_t user_id);
    void closeAllConnections();
    int32_t sendToUser(int64_t user_id, const std::string& message);
    void broadcast(const std::string& message);
    bool isConnected(int64_t user_id);
    int32_t getOnlineCount();

    // Presence WS connection management
    void addPresenceConnection(int64_t user_id, chen::http::WSSession::ptr session);
    void removePresenceConnection(int64_t user_id);
    void broadcastPresence(const std::string& message);
    int32_t getPresenceOnlineCount();

    // DB persistence
    data::NotificationInfo::ptr get(int64_t id);
    data::NotificationInfo::ptr addNotification(int64_t user_id, const std::string& title,
        const std::string& content, const std::string& type, int64_t sender_id,
        int64_t article_id = 0, int64_t comment_id = 0);
    void listByUser(std::vector<data::NotificationInfo::ptr>& results, int64_t user_id,
        uint64_t offset, uint64_t size, const std::string& type = "");
    int64_t countByUser(int64_t user_id, const std::string& type = "");
    int64_t unreadCount(int64_t user_id);
    bool markRead(int64_t notification_id);
    bool markRead(const std::vector<int64_t>& ids);
    int64_t markAllRead(int64_t user_id);
    int64_t markReadByType(int64_t user_id, const std::string& type);

private:
    static data::NotificationInfo::ptr parseRow(chen::ISQLData::ptr rt);

    // WS connections (notification)
    std::unordered_map<int64_t, chen::http::WSSession::ptr> m_connections;
    std::shared_mutex m_connMutex;

    // WS connections (presence)
    std::unordered_map<int64_t, chen::http::WSSession::ptr> m_presenceConnections;
    std::shared_mutex m_presenceMutex;

    // notification cache
    chen::ds::HashLruCache<int64_t, data::NotificationInfo::ptr> m_cache;
};

typedef chen::Singleton<NotificationManager> NotificationMgr;

}
