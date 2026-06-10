/**
 * @file ai_provider.h
 * @brief AI 厂商适配器 — 抽象接口 + 工厂
 * @author Christins
 * @date 2026-06-10
 * @copyright Apache 2.0
 */
#ifndef __BLOG_AI_PROVIDER_H__
#define __BLOG_AI_PROVIDER_H__

#include <json/json.h>
#include <memory>
#include <string>

namespace blog {
namespace ai {

/// 抽象接口：封装不同 AI 厂商的请求体构建与 SSE 响应解析差异
class AIProvider {
public:
    typedef std::shared_ptr<AIProvider> ptr;
    virtual ~AIProvider() = default;

    /// 构建 API 请求体 JSON
    virtual std::string buildRequest(const std::string& model, int32_t maxTokens, const std::string& systemPrompt,
                                     const std::string& prompt) = 0;
    /// API endpoint 路径后缀
    virtual std::string endpointSuffix() const = 0;
    /// 从一条 data: 事件的 JSON 中提取文本（无内容则返回空串）
    virtual std::string extractContent(const Json::Value& parsed) = 0;
    /// 该事件是否表示流结束
    virtual bool isTerminal(const Json::Value& parsed) = 0;
    /// 该事件是否表示错误，返回错误消息（无错误返回空串）
    virtual std::string getError(const Json::Value& parsed) = 0;
    /// 检查非 JSON 的结束标记（如 OpenAI 的 [DONE]），命中返回 true
    virtual bool isDoneMarker(const std::string& rawData) { return false; }
};

/// 工厂：根据 type 创建对应适配器（"openai" 或 "claude"）
AIProvider::ptr createProvider(const std::string& type);

} // namespace ai
} // namespace blog

#endif // __BLOG_AI_PROVIDER_H__
