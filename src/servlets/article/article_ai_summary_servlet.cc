#include "article_ai_summary_servlet.h"

#include <chen/log/log.h>
#include <chen/http/http_connection.h>
#include <chen/http/session_data.h>
#include <chen/ds/lru_cache.h>

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
        if (!config) {
            session->sendEvent(R"({"type":"error","message":"AI config not found"})");
            return -1;
        }
        ai_type = config->getType();
        ai_url  = config->getUrl();
        ai_key  = DecryptApiKey(config->getApiKey());
        ai_model = config->getModel();
        ai_max_tokens = config->getMaxTokens();
        if (ai_max_tokens <= 0) {
            ai_max_tokens = 4096;
        }
        if (ai_model.empty()) {
            ai_model = (ai_type == "claude") ? "claude-sonnet-4-6" : "gpt-4o";
        }
    }

    auto provider = ai::createProvider(ai_type);

    std::string prompt =
        "请用简洁的中文总结以下文章的核心内容，控制在200字以内：\n\n"
        "标题：" + article_title + "\n\n" + article_content;

    std::string api_body_str = provider->buildRequest(ai_model, ai_max_tokens, SYSTEM_PROMPT, prompt);

    {
        auto uri = chen::Uri::Create(ai_url);
        std::string scheme = uri->getScheme();
        std::string host   = uri->getHost();
        int32_t port = uri->getPort();
        std::string path = uri->getPath();
        if (path.empty()) {
            path = "/";
        }

        std::string suffix = provider->endpointSuffix();
        if (path.size() < suffix.size() ||
            path.compare(path.size() - suffix.size(), suffix.size(), suffix) != 0) {
            if (path == "/") {
                path = suffix;
            } else {
                if (path.back() == '/') {
                    path.pop_back();
                }
                path += suffix;
            }
        }

        if (port == 0) {
            port = (scheme == "https") ? 443 : 80;
        }
        ai_url = scheme + "://" + host;
        if ((scheme == "https" && port != 443) || (scheme == "http" && port != 80)) {
            ai_url += ":" + std::to_string(port);
        }
        ai_url += path;
    }

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

        ai::SSEStreamParser parser(session, provider);
        auto callback = [&parser](const char* data, size_t len) {
            return parser(data, len);
        };

        auto http_result = chen::http::HttpConnection::DoRequestStreaming(
            chen::http::HttpMethod::POST, ai_url, 60000, callback, headers, api_body_str);

        if (!http_result || http_result->result != 0) {
            ERROR(logger) << "AI API call failed: " << (http_result ? http_result->error : "null result");
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
            ERROR(logger) << "AI service returned status " << status;
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
