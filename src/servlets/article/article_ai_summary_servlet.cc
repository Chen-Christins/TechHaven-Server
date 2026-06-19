#include "article_ai_summary_servlet.h"

#include <chen/log/log.h>
#include <chen/http/http_connection.h>
#include <chen/http/session_data.h>
#include <chen/ds/lru_cache.h>
#include <chen/config/config.h>

#include "../../ai/ai_provider.h"
#include "../../ai/sse_stream_parser.h"
#include "../../util.h"
#include "../../manager/article_manager.h"
#include "../../manager/user_ai_config_manager.h"
#include "../../struct.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();
static const char* SYSTEM_PROMPT = "你是一个专业的文章总结助手。";

// 系统默认 AI 配置 — 作为用户未配置时的兜底
static chen::ConfigVar<std::string>::ptr g_ai_type =
    chen::Config::Lookup("ai.type", std::string(""), "default AI provider type");
static chen::ConfigVar<std::string>::ptr g_ai_model =
    chen::Config::Lookup("ai.model", std::string(""), "default AI model");
static chen::ConfigVar<std::string>::ptr g_ai_api_key =
    chen::Config::Lookup("ai.api_key", std::string(""), "default AI API key");

// LRU 缓存：key=article_id，value=总结文本，16 桶，最多 500 条
static chen::ds::HashLruCache<int64_t, std::string> s_summary_cache(16, 500, 50);

ArticleAISummaryServlet::ArticleAISummaryServlet()
    : chen::http::SSEServlet("ArticleAISummaryServlet") {
}

int32_t ArticleAISummaryServlet::onConnect(chen::http::HttpRequest::ptr request, chen::http::SSESession::ptr session) {
    int64_t uid = 0;
    std::string ai_type, ai_url, ai_key, ai_model;
    int32_t ai_max_tokens = 4096;

    {
        std::string sid = request->getCookie(CookieKey::SESSION_KEY);
        if (!sid.empty()) {
            auto data = chen::http::SessionDataMgr::GetInstance()->get(sid);
            if (data) uid = data->getData<int64_t>(CookieKey::USER_ID);
        }
        if (!uid) {
            session->sendEvent(R"({"type":"error","message":"not login"})");
            return -1;
        }
    }

    int64_t article_id = 0;
    std::string article_title, article_content;
    {
        auto article_id_str = request->getParam("article_id");
        if (article_id_str.empty()) {
            session->sendEvent(R"({"type":"error","message":"param article_id is required"})");
            return -1;
        }
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

    {
        auto config = UserAIConfigMgr::GetInstance()->getByUserId(uid);
        if (config) {
            ai_type = config->getType();
            ai_url  = config->getUrl();
            ai_key  = DecryptApiKey(config->getApiKey());
            ai_model = config->getModel();
            ai_max_tokens = config->getMaxTokens();
        } else {
            // 用户未配置 AI，使用系统默认值
            ai_type = g_ai_type->getValue();
            ai_model = g_ai_model->getValue();
            ai_key  = g_ai_api_key->getValue();
            if (ai_type.empty() || ai_key.empty()) {
                session->sendEvent(R"({"type":"error","message":"AI config not found"})");
                return -1;
            }
            // 根据类型推导默认 base URL
            if (ai_type == "claude") {
                ai_url = "https://api.anthropic.com";
            } else if (ai_type == "glm") {
                ai_url = "https://open.bigmodel.cn";
            } else {
                ai_url = "https://api.openai.com";
            }
        }
        if (ai_max_tokens <= 0) {
            ai_max_tokens = 4096;
        }
        if (ai_model.empty()) {
            if (ai_type == "claude") {
                ai_model = "claude-sonnet-4-6";
            } else if (ai_type == "glm") {
                ai_model = "glm-4.7-flash";
            } else {
                ai_model = "gpt-4o";
            }
        }
    }

    auto provider = ai::createProvider(ai_type);

    std::string prompt =
        "请用简洁的中文总结以下文章的核心内容，控制在200字以内：\n\n"
        "标题：" + article_title + "\n\n" + article_content;

    std::string api_body_str = provider->buildRequest(ai_model, ai_max_tokens, SYSTEM_PROMPT, prompt);

    auto uri = chen::Uri::Create(ai_url + provider->endpointSuffix());
    {
        std::string cached;
        if (s_summary_cache.get(article_id, cached)) {
            INFO(logger) << "summary cache hit for article " << article_id;
            SendSSEJson(session, "start", "message", "开始生成总结...");
            SendSSEJson(session, "chunk", "content", cached);
            SendSSEJson(session, "done", "message", "总结生成完成");
            return 0;
        }

        std::map<std::string, std::string> headers;
        headers["Content-Type"] = "application/json";
        headers["Authorization"] = "Bearer " + ai_key;

        SendSSEJson(session, "start", "message", "开始生成总结...");

        const int MAX_RETRIES = 5;
        chen::http::HttpResult::ptr http_result;
        ai::SSEStreamParser parser(session, provider);
        std::string raw_body;

        for (int attempt = 1; attempt <= MAX_RETRIES; ++attempt) {
            if (attempt > 1) {
                parser = ai::SSEStreamParser(session, provider);
                raw_body.clear();
            }
            auto callback = [&parser, &raw_body](const char* data, size_t len) {
                raw_body.append(data, len);
                return parser(data, len);
            };

            http_result = chen::http::HttpConnection::DoRequestStreaming(
                chen::http::HttpMethod::POST, uri, 60000, callback, headers, api_body_str);

            if (http_result && http_result->result == 0) {
                break;
            }
            ERROR(logger) << "AI API call failed (attempt " << attempt << "/" << MAX_RETRIES << "): "
                << (http_result ? http_result->error : "null result");
        }

        if (!http_result || http_result->result != 0) {
            ERROR(logger) << "AI API call failed after " << MAX_RETRIES << " attempts";
            SendSSEJson(session, "error", "message", "AI service call failed");
            return -1;
        }

        auto ai_response = http_result->response;
        if (!ai_response) {
            SendSSEJson(session, "error", "message", "no response from AI service");
            return -1;
        }

        int status = static_cast<int>(ai_response->getStatus());
        if (status != 200) {
            ERROR(logger) << "AI service returned status " << status << " body=" << raw_body;
            SendSSEJson(session, "error", "message", "AI service returned error");
            return -1;
        }

        SendSSEJson(session, "done", "message", "总结生成完成");

        if (!parser.full_text.empty()) {
            s_summary_cache.set(article_id, parser.full_text);
            INFO(logger) << "summary cached for article " << article_id;
        }
    }

    return 0;
}

int32_t ArticleAISummaryServlet::onClose(chen::http::HttpRequest::ptr request, chen::http::SSESession::ptr session) {
    return 0;
}

}  // namespace servlet
}  // namespace blog
