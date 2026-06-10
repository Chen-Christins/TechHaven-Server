#include "openai_provider.h"

#include <chen/util/json_util.h>

namespace blog {
namespace ai {

std::string OpenAIProvider::buildRequest(const std::string& model, int32_t maxTokens, const std::string& systemPrompt
        , const std::string& prompt) {
    Json::Value body;
    body["model"] = model;
    body["stream"] = true;
    body["max_tokens"] = maxTokens;

    Json::Value messages(Json::arrayValue);
    Json::Value sys;
    sys["role"] = "system";
    sys["content"] = systemPrompt;
    messages.append(sys);

    Json::Value usr;
    usr["role"] = "user";
    usr["content"] = prompt;
    messages.append(usr);

    body["messages"] = messages;
    return chen::JsonUtil::ToString(body);
}

std::string OpenAIProvider::endpointSuffix() const {
    return "/v1/chat/completions";
}

std::string OpenAIProvider::extractContent(const Json::Value& parsed) {
    auto& choices = parsed["choices"];
    if (choices.isArray() && choices.size() > 0) {
        return choices[0]["delta"]["content"].asString();
    }
    return "";
}

bool OpenAIProvider::isTerminal(const Json::Value& parsed) {
    auto& choices = parsed["choices"];
    if (choices.isArray() && choices.size() > 0) {
        auto finish = choices[0]["finish_reason"].asString();
        return !finish.empty() && finish != "null";
    }
    return false;
}

std::string OpenAIProvider::getError(const Json::Value& /*parsed*/) {
    return "";
}

bool OpenAIProvider::isDoneMarker(const std::string& rawData) {
    return rawData == "[DONE]";
}

}  // namespace ai
}  // namespace blog
