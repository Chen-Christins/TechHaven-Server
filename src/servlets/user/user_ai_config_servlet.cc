#include "user_ai_config_servlet.h"

#include <chen/log/log.h>

#include "../../manager/user_ai_config_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

UserAIConfigServlet::UserAIConfigServlet()
    : BlogLoginedServlet("UserAIConfigServlet") {
}

int32_t UserAIConfigServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(410, "not login");
            break;
        }

        auto method = request->getMethod();

        if (method == chen::http::HttpMethod::GET) {
            // 获取AI配置
            auto info = UserAIConfigMgr::GetInstance()->getByUserId(uid);
            if (!info) {
                result->setResult(200, "ok");
                result->set("data", Json::Value::null);
                break;
            }

            // 解密并脱敏api_key
            std::string raw_key = DecryptApiKey(info->getApiKey());
            std::string masked_key = MaskApiKey(raw_key);

            result->setResult(200, "ok");
            result->set("type", info->getType());
            result->set("url", info->getUrl());
            result->set("api_key", masked_key);
            result->set("model", info->getModel());
            result->set("max_tokens", info->getMaxTokens());
        } else if (method == chen::http::HttpMethod::PUT || method == chen::http::HttpMethod::POST) {
            // 保存AI配置
            DEFINE_AND_CHECK_STRING(result, type, "type");
            DEFINE_AND_CHECK_STRING(result, url, "url");
            DEFINE_AND_CHECK_STRING(result, api_key, "api_key");

            // 校验type值
            if (type != "openai" && type != "claude") {
                result->setResult(400, "type must be 'openai' or 'claude'");
                break;
            }

            // claude类型必须提供max_tokens
            int32_t max_tokens = request->getParamAs<int32_t>("max_tokens", 0);
            if (type == "claude" && max_tokens <= 0) {
                result->setResult(400, "param max_tokens is required for claude type");
                break;
            }

            std::string model = request->getParam("model");

            // 查询已有配置
            auto info = UserAIConfigMgr::GetInstance()->getByUserId(uid);
            if (!info) {
                info.reset(new data::UserAiConfigInfo);
                info->setUserId(uid);
            }

            info->setType(type);
            info->setUrl(url);
            info->setApiKey(EncryptApiKey(api_key));
            if (!model.empty()) {
                info->setModel(model);
            }
            if (max_tokens > 0) {
                info->setMaxTokens(max_tokens);
            }

            if (!UserAIConfigMgr::GetInstance()->save(info)) {
                result->setResult(500, "save config fail");
                break;
            }
        } else {
            result->setResult(405, "method not allowed");
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
