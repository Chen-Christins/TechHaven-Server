#include "notify_servlet.h"

#include "../../manager/user_manager.h"
#include "../../manager/notification_manager.h"

#include <chen/log/log.h>

namespace blog::servlet {

static chen::Logger::ptr logger = LOG_ROOT();

NotifyServlet::NotifyServlet()
    : chen::http::WSServlet("Notify") {
}

int32_t NotifyServlet::onConnect(chen::http::HttpRequest::ptr header, chen::http::WSSession::ptr session) {
    std::string uid_str = header->getParam("uid");
    std::string token = header->getParam("token");
    std::string token_time_str = header->getParam("token_time");

    INFO(logger) << "[WS] onConnect: uid=" << uid_str
        << " token=" << (token.empty() ? "(empty)" : "***")
        << " token_time=" << token_time_str;

    if (uid_str.empty() || token.empty() || token_time_str.empty()) {
        INFO(logger) << "[WS] onConnect FAIL: missing params";
        return -1;
    }

    int64_t uid = std::stoll(uid_str);
    int64_t token_time = std::stoll(token_time_str);

    if (token_time <= time(0)) {
        INFO(logger) << "[WS] onConnect FAIL: token expired, token_time="
            << token_time << " now=" << time(0);
        return -1;
    }

    data::UserInfo::ptr uinfo = UserMgr::GetInstance()->get(uid);
    if (!uinfo) {
        INFO(logger) << "[WS] onConnect FAIL: user not found uid=" << uid;
        return -1;
    }
    if (uinfo->getState() != 1) {
        INFO(logger) << "[WS] onConnect FAIL: user state=" << uinfo->getState()
            << " uid=" << uid;
        return -1;
    }

    // 优先用数据库存储的随机 token（单设备登录），
    // 若为空则回退到旧的 MD5 计算方式（兼容旧账号）
    bool token_valid = false;
    const std::string& stored_token = uinfo->getToken();
    if (!stored_token.empty()) {
        token_valid = (stored_token == token);
    } else {
        token_valid = (UserManager::GetToken(uinfo, token_time) == token);
    }
    if (!token_valid) {
        INFO(logger) << "[WS] onConnect FAIL: token mismatch, stored="
            << (stored_token.empty() ? "(empty)" : "***") << " got=" << token;
        return -1;
    }

    NotificationMgr::GetInstance()->addConnection(uid, session);
    INFO(logger) << "[WS] onConnect OK: uid=" << uid;

    return 0;
}

int32_t NotifyServlet::onClose(chen::http::HttpRequest::ptr header, chen::http::WSSession::ptr session) {
    std::string uid_str = header->getParam("uid");
    if (!uid_str.empty()) {
        int64_t uid = std::stoll(uid_str);
        NotificationMgr::GetInstance()->removeConnection(uid);
    }

    return 0;
}

int32_t NotifyServlet::handle(chen::http::HttpRequest::ptr header
        , chen::http::WSFrameMessage::ptr msg, chen::http::WSSession::ptr session) {
    INFO(logger) << "[WS] handle: opcode=" << msg->getOpcode()
        << " data=" << msg->getData();

    return 0;
}

} // namespace blog::servlet