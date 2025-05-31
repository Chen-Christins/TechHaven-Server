#include "user_query_servlet.h"
#include "chen/log/log.h"
#include "blog/data/user_info.h"
#include "../../manager/user_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

static sylar::Logger::ptr logger = LOG_ROOT();

UserQueryServlet::UserQueryServlet()
    :BlogServlet("UserQueryServlet") {
}

int32_t UserQueryServlet::handle(sylar::http::HttpRequest::ptr request, sylar::http::HttpResponse::ptr response
        ,sylar::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, user_ids, "user_ids");

        auto ids = sylar::split(user_ids, ',');
        std::vector<data::UserInfo::ptr> infos;

        for (auto& i : ids) {
            data::UserInfo::ptr info = UserMgr::GetInstance()->get(sylar::TypeUtil::Atoi(i));
            if (info) {
                infos.push_back(info);
            }
        }

        for (auto& i : infos) {
            Json::Value v;
            v["id"] = i->getId();
            v["name"] = i->getName();
            result->jsondata.append(v);
        }
        INFO(logger) << "infos.size=" << infos.size()
            << " - " << sylar::JsonUtil::ToString(result->jsondata);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
};

}
}
