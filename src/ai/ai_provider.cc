#include "ai_provider.h"
#include "openai_provider.h"
#include "claude_provider.h"

namespace blog {
namespace ai {

AIProvider::ptr createProvider(const std::string& type) {
    if (type == "claude") {
        return std::make_shared<ClaudeProvider>();
    }
    return std::make_shared<OpenAIProvider>();  // 默认 OpenAI
}

}  // namespace ai
}  // namespace blog
