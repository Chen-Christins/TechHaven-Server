#include "message_manager.h"

#include "../util.h"

#include <chen/log/log.h>

#include <algorithm>

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 2000;

MessageManager::MessageManager()
    :m_cache(16, kCacheMaxSize, 0) {
}

// ========== 会话相关 ==========

data::ConversationInfo::ptr MessageManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::ConversationInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

data::ConversationInfo::ptr MessageManager::getByUsers(int64_t uid_a, int64_t uid_b) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    auto qb = data::ConversationInfoDao::newQuery();
    qb->where("user_a_id", "=", NormalizeUserA(uid_a, uid_b));
    qb->where("user_b_id", "=", NormalizeUserB(uid_a, uid_b));
    qb->where("is_deleted", "=", (int64_t)0);
    std::vector<data::ConversationInfo::ptr> results;
    if (data::ConversationInfoDao::QueryByBuilder(results, qb, db)) {
        ERROR(logger) << "QueryByBuilder failed";
        return nullptr;
    }
    if (results.empty()) {
        return nullptr;
    }
    m_cache.set(results[0]->getId(), results[0]);
    return results[0];
}

data::ConversationInfo::ptr MessageManager::getOrCreate(int64_t uid_a, int64_t uid_b) {
    auto conv = getByUsers(uid_a, uid_b);
    if (conv) {
        return conv;
    }

    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }

    auto info = std::make_shared<data::ConversationInfo>();
    info->setUserAId(NormalizeUserA(uid_a, uid_b));
    info->setUserBId(NormalizeUserB(uid_a, uid_b));
    info->setLastMessageId(0);
    info->setLastSenderId(0);
    info->setLastMessage("");
    info->setLastMessageTime(0);
    info->setUnreadA(0);
    info->setUnreadB(0);
    info->setIsDeleted(0);
    info->setCreateTime(time(0));
    info->setUpdateTime(time(0));

    if (data::ConversationInfoDao::Insert(info, db)) {
        // 并发下唯一索引冲突时回查
        conv = getByUsers(uid_a, uid_b);
        if (conv) {
            return conv;
        }
        ERROR(logger) << "MessageManager getOrCreate Insert fail";
        return nullptr;
    }

    m_cache.set(info->getId(), info);
    return info;
}

void MessageManager::listConversations(std::vector<data::ConversationInfo::ptr>& results, int64_t uid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return;
    }

    // 分两次查询（user_a_id / user_b_id），避免 OR 优先级问题
    auto qb = data::ConversationInfoDao::newQuery();
    qb->where("user_a_id", "=", uid);
    qb->where("is_deleted", "=", (int64_t)0);
    if (data::ConversationInfoDao::QueryByBuilder(results, qb, db)) {
        ERROR(logger) << "QueryByBuilder failed";
        return;
    }

    auto qb2 = data::ConversationInfoDao::newQuery();
    qb2->where("user_b_id", "=", uid);
    qb2->where("is_deleted", "=", (int64_t)0);
    std::vector<data::ConversationInfo::ptr> part;
    if (data::ConversationInfoDao::QueryByBuilder(part, qb2, db)) {
        ERROR(logger) << "QueryByBuilder failed";
        return;
    }
    results.insert(results.end(), part.begin(), part.end());

    std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
        return a->getLastMessageTime() > b->getLastMessageTime();
    });

    for (auto& c : results) {
        if (!m_cache.exists(c->getId())) {
            m_cache.set(c->getId(), c);
        }
    }
}

// ========== 消息相关 ==========

void MessageManager::listMessages(std::vector<data::ConversationMessageInfo::ptr>& results
        , int64_t conversation_id, int32_t offset, int32_t limit) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return;
    }

    auto qb = data::ConversationMessageInfoDao::newQuery();
    qb->where("conversation_id", "=", conversation_id);
    qb->where("is_deleted", "=", (int64_t)0);
    qb->orderBy("id", "DESC");
    qb->limit(limit);
    qb->offset(offset);

    std::vector<data::ConversationMessageInfo::ptr> tmp;
    if (data::ConversationMessageInfoDao::QueryByBuilder(tmp, qb, db)) {
        ERROR(logger) << "QueryByBuilder failed";
        return;
    }
    std::reverse(tmp.begin(), tmp.end());
    results.swap(tmp);
}

data::ConversationMessageInfo::ptr MessageManager::sendMessage(int64_t conversation_id
        , int64_t sender_id, const std::string& text) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }

    auto msg = std::make_shared<data::ConversationMessageInfo>();
    msg->setConversationId(conversation_id);
    msg->setSenderId(sender_id);
    msg->setContent(text);
    msg->setIsRead(0);
    msg->setReadTime(0);
    msg->setIsDeleted(0);
    msg->setCreateTime(time(0));
    msg->setUpdateTime(time(0));

    if (data::ConversationMessageInfoDao::Insert(msg, db)) {
        ERROR(logger) << "MessageManager sendMessage Insert fail";
        return nullptr;
    }

    // 更新会话冗余信息与接收方未读数
    auto conv = get(conversation_id);
    if (conv) {
        conv->setLastMessageId(msg->getId());
        conv->setLastSenderId(sender_id);
        conv->setLastMessage(text);
        conv->setLastMessageTime(msg->getCreateTime());
        conv->setUpdateTime(msg->getCreateTime());
        if (sender_id == conv->getUserAId()) {
            conv->setUnreadB(conv->getUnreadB() + 1);
        } else {
            conv->setUnreadA(conv->getUnreadA() + 1);
        }
        if (data::ConversationInfoDao::Update(conv, db) == 0) {
            m_cache.set(conv->getId(), conv);
        }
    }

    return msg;
}

bool MessageManager::markRead(int64_t conversation_id, int64_t uid) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }

    auto conv = get(conversation_id);
    if (!conv) {
        return false;
    }
    if (uid == conv->getUserAId()) {
        if (conv->getUnreadA() != 0) {
            conv->setUnreadA(0);
        }
    } else if (uid == conv->getUserBId()) {
        if (conv->getUnreadB() != 0) {
            conv->setUnreadB(0);
        }
    } else {
        return false;
    }
    conv->setUpdateTime(time(0));
    if (data::ConversationInfoDao::Update(conv, db)) {
        return false;
    }
    m_cache.set(conv->getId(), conv);

    // 将该会话内对方发来的未读消息置为已读
    auto qb = data::ConversationMessageInfoDao::newQuery();
    qb->set("is_read", (int64_t)1);
    qb->set("read_time", (int64_t)time(0));
    qb->set("update_time", (int64_t)time(0));
    qb->where("conversation_id", "=", conversation_id);
    qb->where("sender_id", "!=", uid);
    qb->where("is_read", "=", (int64_t)0);
    qb->where("is_deleted", "=", (int64_t)0);
    auto stmt = db->prepare(qb->buildUpdateSQL());
    if (!stmt) {
        return true;
    }
    qb->bindUpdateParams(stmt);
    stmt->execute();
    return true;
}

// ========== 聊天 WS 连接管理 ==========

void MessageManager::addChatConnection(int64_t user_id, chen::http::WSSession::ptr session) {
    std::unique_lock lock(m_chatMutex);
    m_chatConnections[user_id] = session;
    INFO(logger) << "Chat WS connected: user_id=" << user_id;
}

void MessageManager::removeChatConnection(int64_t user_id) {
    std::unique_lock lock(m_chatMutex);
    m_chatConnections.erase(user_id);
    INFO(logger) << "Chat WS disconnected: user_id=" << user_id;
}

bool MessageManager::isChatConnected(int64_t user_id) {
    std::shared_lock lock(m_chatMutex);
    return m_chatConnections.find(user_id) != m_chatConnections.end();
}

int32_t MessageManager::sendToUser(int64_t user_id, const std::string& message) {
    std::shared_lock lock(m_chatMutex);
    auto it = m_chatConnections.find(user_id);
    if (it == m_chatConnections.end()) {
        return -1;
    }
    return it->second->sendMessage(message);
}

void MessageManager::closeAllChatConnections() {
    std::unique_lock lock(m_chatMutex);
    for (auto& [user_id, session] : m_chatConnections) {
        session->close();
        INFO(logger) << "Chat WS closed: user_id=" << user_id;
    }
    m_chatConnections.clear();
    INFO(logger) << "All Chat WS connections closed";
}

int64_t MessageManager::getUidBySession(chen::http::WSSession::ptr session) {
    std::shared_lock lock(m_chatMutex);
    for (auto& [user_id, conn] : m_chatConnections) {
        if (conn == session) {
            return user_id;
        }
    }
    return 0;
}

}
