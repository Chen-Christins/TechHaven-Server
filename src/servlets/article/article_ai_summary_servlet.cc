#include "article_ai_summary_servlet.h"

#include <chen/log/log.h>
#include <chen/http/http_connection.h>

#include "../../util.h"
#include "../../manager/article_manager.h"
#include "../../manager/user_ai_config_manager.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

ArticleAISummaryServlet::ArticleAISummaryServlet()
    : BlogLoginedServlet("ArticleAISummaryServlet") {
}

// 从 AI 返回的 SSE body 中提取总结文本
static std::string extractSummaryFromBody(const std::string& body) {
    std::string summary;
    std::string line;
    std::string current_event;

    for (char c : body) {
        if (c == '\n') {
            if (!line.empty() && line.back() == '\r') line.pop_back();

            if (line.empty()) {
                // SSE 事件结束
                if (!current_event.empty()) {
                    Json::Value parsed;
                    Json::Reader reader;
                    if (reader.parse(current_event, parsed)) {
                        std::string evType = parsed["type"].asString();

                        // Anthropic 格式: content_block_delta + text_delta
                        if (evType == "content_block_delta") {
                            std::string deltaType = parsed["delta"]["type"].asString();
                            if (deltaType == "text_delta") {
                                summary += parsed["delta"]["text"].asString();
                            }
                        }

                        // OpenAI 格式: choices[0].delta.content
                        auto& choices = parsed["choices"];
                        if (choices.isArray() && choices.size() > 0) {
                            summary += choices[0]["delta"]["content"].asString();
                        }
                    }
                    current_event.clear();
                }
            } else if (line.size() >= 6 && line.substr(0, 6) == "data: ") {
                current_event = line.substr(6);
            }

            line.clear();
        } else {
            line += c;
        }
    }
    return summary;
}

int32_t ArticleAISummaryServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);

        // ======================== 1. 获取文章 ========================
        DEFINE_AND_CHECK_TYPE(result, int64_t, article_id, "article_id");

        auto article = ArticleMgr::GetInstance()->get(article_id);
        if (!article) {
            result->setResult(404, "article not found");
            break;
        }

        std::string article_title = article->getTitle();
        std::string article_content = article->getContent();

        // ======================== 2. 获取 AI 配置 ========================
        auto config = UserAIConfigMgr::GetInstance()->getByUserId(uid);
        if (!config) {
            result->setResult(400, "AI config not found, please configure in settings");
            break;
        }

        std::string ai_type = config->getType();
        std::string ai_url  = config->getUrl();
        std::string ai_key  = DecryptApiKey(config->getApiKey());
        std::string ai_model = config->getModel();
        int32_t ai_max_tokens = config->getMaxTokens();
        if (ai_max_tokens <= 0) ai_max_tokens = 4096;
        if (ai_model.empty()) {
            ai_model = (ai_type == "claude") ? "claude-sonnet-4-6" : "gpt-4o";
        }

        // ======================== 3. 构建 Prompt ========================
        std::string prompt =
            "请用简洁的中文总结以下文章的核心内容，控制在200字以内：\n\n"
            "标题：" + article_title + "\n\n" +
            article_content;

        // ======================== 4. 构建 AI API 请求体 ========================
        Json::Value api_body;
        api_body["model"] = ai_model;
        api_body["stream"] = true;
        api_body["max_tokens"] = ai_max_tokens;

        Json::Value messages(Json::arrayValue);
        if (ai_type == "openai") {
            Json::Value sys;
            sys["role"] = "system";
            sys["content"] = "你是一个专业的文章总结助手。";
            messages.append(sys);
        } else {
            api_body["system"] = "你是一个专业的文章总结助手。";
        }

        Json::Value usr;
        usr["role"] = "user";
        usr["content"] = prompt;
        messages.append(usr);
        api_body["messages"] = messages;

        std::string api_body_str = chen::JsonUtil::ToString(api_body);

        // 拼接完整的 API endpoint URL（已含则跳过）
        {
            auto uri = chen::Uri::Create(ai_url);
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
            uri->setPath(path);
            ai_url = uri->toString();
        }

        // ======================== 5. 调用 AI API（协程异步，不阻塞 IO 线程） ========================
        std::map<std::string, std::string> headers;
        headers["Content-Type"] = "application/json";
        headers["Authorization"] = "Bearer " + ai_key;
        headers["Accept"] = "text/event-stream";

        // 解析 host/port，用 HttpConnectionPool 做异步请求
        auto uri = chen::Uri::Create(ai_url);
        std::string scheme = uri->getScheme();
        std::string host   = uri->getHost();
        int32_t port = uri->getPort();
        if (port == 0) port = (scheme == "https") ? 443 : 80;

        chen::http::HttpResult::ptr http_result;
        if (scheme == "https") {
            // HTTPS: pool 不支持 SSL，走静态方法（协程层面仍是异步的）
            http_result = chen::http::HttpConnection::DoPost(ai_url, 60000, headers, api_body_str);
        } else {
            // HTTP: 走连接池，复用连接，协程异步等待
            chen::http::HttpConnectionPool pool(host, host, port, 5, 30000, 100);
            http_result = pool.doPost(ai_url, 60000, headers, api_body_str);
        }

        if (!http_result || http_result->result != 0) {
            ERROR(logger) << "AI API call failed: " << (http_result ? http_result->error : "null result");
            result->setResult(500, "AI service call failed");
            break;
        }

        auto ai_response = http_result->response;
        if (!ai_response) {
            result->setResult(500, "no response from AI service");
            break;
        }

        int status = static_cast<int>(ai_response->getStatus());
        if (status != 200) {
            ERROR(logger) << "AI service returned status " << status;
            result->setResult(502, "AI service returned error");
            break;
        }

        // ======================== 6. 解析 SSE body 提取总结 ========================
        std::string summary = extractSummaryFromBody(ai_response->getBody());

        if (summary.empty()) {
            result->setResult(500, "failed to extract summary from AI response");
            break;
        }

        result->set("summary", summary);
    } while (0);

    response->setBody(result->toJsonString());
    return 0;
}

}  // namespace servlet
}  // namespace blog
