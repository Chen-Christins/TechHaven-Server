#include "article_ai_summary_servlet.h"

#include <chen/log/log.h>
#include <chen/http/http_connection.h>
#include <chen/http/session_data.h>

#include "../../util.h"
#include "../../manager/article_manager.h"
#include "../../manager/user_ai_config_manager.h"
#include "../../struct.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

ArticleAISummaryServlet::ArticleAISummaryServlet()
    : chen::http::SSEServlet("ArticleAISummaryServlet") {
}

static void sendSSEJson(chen::http::SSESession::ptr session, const std::string& type
        , const std::string& key, const std::string& value) {
    Json::Value obj;
    obj["type"] = type;
    obj[key] = value;
    session->sendEvent(chen::JsonUtil::ToString(obj));
}

// ---------- SSE 流解析器：逐行解析 AI 返回的 SSE，提取文本推给前端 ----------
class SSEStreamParser {
public:
    SSEStreamParser(chen::http::SSESession::ptr s) : session(s) {}

    bool operator()(const char* data, size_t len) {
        for (size_t i = 0; i < len; i++) {
            char c = data[i];
            if (c == '\n') {
                if (!line_buf.empty() && line_buf.back() == '\r') {
                    line_buf.pop_back();
                }

                if (line_buf.empty()) {
                    if (!current_event.empty()) {
                        processEvent();
                        current_event.clear();
                    }
                } else if (line_buf.size() >= 6 && line_buf.substr(0, 6) == "data: ") {
                    current_event = line_buf.substr(6);
                }
                line_buf.clear();
            } else {
                line_buf += c;
            }
        }
        return true;
    }

private:
    void processEvent() {
        Json::Value parsed;
        Json::Reader reader;
        if (reader.parse(current_event, parsed)) {
            std::string evType = parsed["type"].asString();

            // OpenAI: choices[0].delta.content
            auto& choices = parsed["choices"];
            if (choices.isArray() && choices.size() > 0) {
                auto content = choices[0]["delta"]["content"].asString();
                if (!content.empty()) {
                    sendSSEJson(session, "chunk", "content", content);
                }
                auto finish = choices[0]["finish_reason"].asString();
                if (!finish.empty() && finish != "null") {
                    sendSSEJson(session, "done", "message", "总结生成完成");
                }
            }

            // Anthropic: content_block_delta + text_delta
            if (evType == "content_block_delta") {
                auto deltaType = parsed["delta"]["type"].asString();
                if (deltaType == "text_delta") {
                    auto text = parsed["delta"]["text"].asString();
                    if (!text.empty()) {
                        sendSSEJson(session, "chunk", "content", text);
                    }
                }
            } else if (evType == "message_stop") {
                sendSSEJson(session, "done", "message", "总结生成完成");
            } else if (evType == "error") {
                auto msg = parsed["error"]["message"].asString();
                sendSSEJson(session, "error", "message", msg.empty() ? "AI服务返回错误" : msg);
            }
        } else if (current_event == "[DONE]") {
            sendSSEJson(session, "done", "message", "总结生成完成");
        }
    }
private:
    chen::http::SSESession::ptr session;
    std::string line_buf;
    std::string current_event;
};

int32_t ArticleAISummaryServlet::onConnect(chen::http::HttpRequest::ptr request, chen::http::SSESession::ptr session) {
    int64_t uid = 0;
    std::string ai_type, ai_url, ai_key, ai_model;
    int32_t ai_max_tokens = 4096;
    std::string api_body_str;

    // ======================== 1. 用户认证 ========================
    {
        std::string sid = request->getCookie(CookieKey::SESSION_KEY);
        if (!sid.empty()) {
            auto data = chen::http::SessionDataMgr::GetInstance()->get(sid);
            if (data) {
                uid = data->getData<int64_t>(CookieKey::USER_ID);
            }
        }
        if (!uid) {
            session->sendEvent(R"({"type":"error","message":"not login"})");
            return -1;
        }
    }

    // ======================== 2. 获取文章 ========================
    std::string article_title, article_content;
    {
        auto article_id_str = request->getParam("article_id");
        if (article_id_str.empty()) {
            session->sendEvent(R"({"type":"error","message":"param article_id is required"})");
            return -1;
        }
        int64_t article_id = 0;
        try {
            article_id = std::stoll(article_id_str);
        } catch (...) {
            session->sendEvent(R"({"type":"error","message":"invalid article_id"})");
            return -1;
        }

        auto article = ArticleMgr::GetInstance()->get(article_id);
        if (!article) {
            session->sendEvent(R"({"type":"error","message":"article not found"})");
            return -1;
        }
        article_title = article->getTitle();
        article_content = article->getContent();

        const size_t MAX_CONTENT_LEN = 12000;
        if (article_content.size() > MAX_CONTENT_LEN) {
            article_content = article_content.substr(0, MAX_CONTENT_LEN) + "...";
        }
    }

    // ======================== 3. 获取 AI 配置 ========================
    {
        auto config = UserAIConfigMgr::GetInstance()->getByUserId(uid);
        if (!config) {
            session->sendEvent(R"({"type":"error","message":"AI config not found"})");
            return -1;
        }
        ai_type = config->getType();
        ai_url  = config->getUrl();
        ai_key  = DecryptApiKey(config->getApiKey());
        ai_model = config->getModel();
        ai_max_tokens = config->getMaxTokens();
        if (ai_max_tokens <= 0) ai_max_tokens = 4096;
        if (ai_model.empty()) {
            ai_model = (ai_type == "claude") ? "claude-sonnet-4-6" : "gpt-4o";
        }
    }

    // ======================== 4. 构建 Prompt ========================
    std::string prompt =
        "请用简洁的中文总结以下文章的核心内容，控制在200字以内：\n\n"
        "标题：" + article_title + "\n\n" +
        article_content;

    // ======================== 5. 构建 AI API 请求体 ========================
    {
        Json::Value body;
        body["model"] = ai_model;
        body["stream"] = true;
        body["max_tokens"] = ai_max_tokens;

        Json::Value messages(Json::arrayValue);
        if (ai_type == "openai") {
            Json::Value sys;
            sys["role"] = "system";
            sys["content"] = "你是一个专业的文章总结助手。";
            messages.append(sys);
        } else {
            body["system"] = "你是一个专业的文章总结助手。";
        }

        Json::Value usr;
        usr["role"] = "user";
        usr["content"] = prompt;
        messages.append(usr);
        body["messages"] = messages;
        api_body_str = chen::JsonUtil::ToString(body);
    }

    // ======================== 6. 构造完整 API URL ========================
    {
        auto uri = chen::Uri::Create(ai_url);
        std::string scheme = uri->getScheme();
        std::string host   = uri->getHost();
        int32_t port = uri->getPort();
        std::string path = uri->getPath();
        if (path.empty()) path = "/";

        std::string suffix = (ai_type == "openai") ? "/v1/chat/completions" : "/v1/messages";
        if (path.size() < suffix.size() ||
            path.compare(path.size() - suffix.size(), suffix.size(), suffix) != 0) {
            if (path == "/") {
                path = suffix;
            } else {
                if (path.back() == '/') path.pop_back();
                path += suffix;
            }
        }

        if (port == 0) port = (scheme == "https") ? 443 : 80;
        ai_url = scheme + "://" + host;
        if ((scheme == "https" && port != 443) || (scheme == "http" && port != 80)) {
            ai_url += ":" + std::to_string(port);
        }
        ai_url += path;
    }

    // ======================== 7. 流式调用 AI API ========================
    {
        std::map<std::string, std::string> headers;
        headers["Content-Type"] = "application/json";
        headers["Authorization"] = "Bearer " + ai_key;

        // 发送 start 事件
        sendSSEJson(session, "start", "message", "开始生成总结...");

        SSEStreamParser parser(session);
        auto http_result = chen::http::HttpConnection::DoRequestStreaming(
            chen::http::HttpMethod::POST,
            ai_url, 60000, parser, headers, api_body_str);

        if (!http_result || http_result->result != 0) {
            ERROR(logger) << "AI API call failed: " << (http_result ? http_result->error : "null result");
            sendSSEJson(session, "error", "message", "AI service call failed");
            return -1;
        }

        auto ai_response = http_result->response;
        if (!ai_response) {
            sendSSEJson(session, "error", "message", "no response from AI service");
            return -1;
        }

        int status = static_cast<int>(ai_response->getStatus());
        if (status != 200) {
            ERROR(logger) << "AI service returned status " << status;
            sendSSEJson(session, "error", "message", "AI service returned error");
            return -1;
        }

        // 确保以 done 收尾（如果 AI 没有触发 message_stop）
        sendSSEJson(session, "done", "message", "总结生成完成");
    }

    return 0;
}

int32_t ArticleAISummaryServlet::onClose(chen::http::HttpRequest::ptr request, chen::http::SSESession::ptr session) {
    return 0;
}

}  // namespace servlet
}  // namespace blog
