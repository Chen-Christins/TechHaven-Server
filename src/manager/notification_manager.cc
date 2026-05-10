#include "notification_manager.h"
#include <chen/log/log.h>

namespace blog {

static chen::Logger::ptr logger = LOG_NAME("system");

void NotificationManager::addConnection(int64_t user_id, chen::http::WSSession::ptr session) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_connections[user_id] = session;
    INFO(logger) << "Notification WS connected: user_id=" << user_id;
}

void NotificationManager::removeConnection(int64_t user_id) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_connections.erase(user_id);
    INFO(logger) << "Notification WS disconnected: user_id=" << user_id;
}

int32_t NotificationManager::sendToUser(int64_t user_id, const std::string& message) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_connections.find(user_id);
    if (it == m_connections.end()) {
        return -1;
    }
    return it->second->sendMessage(message);
}

void NotificationManager::broadcast(const std::string& message) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    for (auto& [user_id, session] : m_connections) {
        session->sendMessage(message);
    }
}

bool NotificationManager::isConnected(int64_t user_id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    return m_connections.find(user_id) != m_connections.end();
}

}
