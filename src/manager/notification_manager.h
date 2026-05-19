#ifndef __BLOG_MANAGER_NOTIFICATION_MANAGER_H__
#define __BLOG_MANAGER_NOTIFICATION_MANAGER_H__

#include <chen/http/ws_session.h>
#include <chen/singleton.h>
#include <unordered_map>
#include <map>
#include <shared_mutex>
#include "blog/data/notification_info.h"

namespace blog {

class NotificationManager {
public:
    // WS connection management
    void addConnection(int64_t user_id, chen::http::WSSession::ptr session);
    void removeConnection(int64_t user_id);
    void closeAllConnections();
    int32_t sendToUser(int64_t user_id, const std::string& message);
    void broadcast(const std::string& message);
    bool isConnected(int64_t user_id);
    int32_t getOnlineCount();

    // DB persistence
    bool loadAll();
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
    // WS connections
    std::unordered_map<int64_t, chen::http::WSSession::ptr> m_connections;
    std::shared_mutex m_connMutex;

    // notification data: all notifications by id
    std::map<int64_t, data::NotificationInfo::ptr> m_datas;
    // notification data: user_id -> [id -> info]
    std::unordered_map<int64_t, std::map<int64_t, data::NotificationInfo::ptr>> m_userNotifications;
    std::shared_mutex m_dataMutex;
};

typedef chen::Singleton<NotificationManager> NotificationMgr;

}

#endif // __BLOG_MANAGER_NOTIFICATION_MANAGER_H__
