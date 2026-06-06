#include "presence_servlet.h"

#include "../../manager/user_manager.h"
#include "../../manager/notification_manager.h"

#include <chen/log/log.h>
#include <json/json.h>

namespace blog::servlet {

static chen::Logger::ptr logger = LOG_ROOT();

PresenceServlet::PresenceServlet()
    : chen::http::WSServlet("Presence") {
}

static void broadcastOnlineCount() {
    int32_t count = NotificationMgr::GetInstance()->getPresenceOnlineCount();
    Json::Value msg;
    msg["type"] = "online_count";
    msg["count"] = count;
    NotificationMgr::GetInstance()->broadcastPresence(chen::JsonUtil::ToString(msg));
}

int32_t PresenceServlet::onConnect(chen::http::HttpRequest::ptr header, chen::http::WSSession::ptr session) {
    std::string uid_str = header->getParam("uid");
    std::string token = header->getParam("token");
    std::string token_time_str = header->getParam("token_time");

    INFO(logger) << "[Presence] onConnect: uid=" << uid_str
        << " token=" << (token.empty() ? "(empty)" : "***")
        << " token_time=" << token_time_str;

    if (uid_str.empty() || token.empty() || token_time_str.empty()) {
        INFO(logger) << "[Presence] onConnect FAIL: missing params";
        return -1;
    }

    int64_t uid = std::stoll(uid_str);
    int64_t token_time = std::stoll(token_time_str);

    if (token_time <= time(0)) {
        INFO(logger) << "[Presence] onConnect FAIL: token expired, token_time="
            << token_time << " now=" << time(0);
        return -1;
    }

    data::UserInfo::ptr uinfo = UserMgr::GetInstance()->get(uid);
    if (!uinfo) {
        INFO(logger) << "[Presence] onConnect FAIL: user not found uid=" << uid;
        return -1;
    }
    if (uinfo->getState() != 1) {
        INFO(logger) << "[Presence] onConnect FAIL: user state=" << uinfo->getState()
            << " uid=" << uid;
        return -1;
    }

    bool token_valid = false;
    const std::string& stored_token = uinfo->getToken();
    if (!stored_token.empty()) {
        token_valid = (stored_token == token);
    } else {
        token_valid = (UserManager::GetToken(uinfo, token_time) == token);
    }
    if (!token_valid) {
        INFO(logger) << "[Presence] onConnect FAIL: token mismatch, stored="
            << (stored_token.empty() ? "(empty)" : "***") << " got=" << token;
        return -1;
    }

    NotificationMgr::GetInstance()->addPresenceConnection(uid, session);
    INFO(logger) << "[Presence] onConnect OK: uid=" << uid;

    broadcastOnlineCount();
    return 0;
}

int32_t PresenceServlet::onClose(chen::http::HttpRequest::ptr header, chen::http::WSSession::ptr session) {
    std::string uid_str = header->getParam("uid");
    if (!uid_str.empty()) {
        int64_t uid = std::stoll(uid_str);
        NotificationMgr::GetInstance()->removePresenceConnection(uid);
        broadcastOnlineCount();
    }

    return 0;
}

int32_t PresenceServlet::handle(chen::http::HttpRequest::ptr header
        , chen::http::WSFrameMessage::ptr msg, chen::http::WSSession::ptr session) {
    return 0;
}

} // namespace blog::servlet
