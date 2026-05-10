#ifndef __BLOG_MANAGER_NOTIFICATION_MANAGER_H__
#define __BLOG_MANAGER_NOTIFICATION_MANAGER_H__

#include <chen/http/ws_session.h>
#include <chen/singleton.h>
#include <unordered_map>
#include <shared_mutex>

namespace blog {

class NotificationManager {
public:
    void addConnection(int64_t user_id, chen::http::WSSession::ptr session);
    void removeConnection(int64_t user_id);
    int32_t sendToUser(int64_t user_id, const std::string& message);
    void broadcast(const std::string& message);
    bool isConnected(int64_t user_id);

private:
    std::unordered_map<int64_t, chen::http::WSSession::ptr> m_connections;
    std::shared_mutex m_mutex;
};

typedef chen::Singleton<NotificationManager> NotificationMgr;

}

#endif // __BLOG_MANAGER_NOTIFICATION_MANAGER_H__
