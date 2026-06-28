#include "label_query_servlet.h"

#include "../../manager/label_manager.h"

#include <chen/log/log.h>

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

LabelQueryServlet::LabelQueryServlet()
    : BlogLoginedServlet("LabelQueryServlet") {
}

int32_t LabelQueryServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t user_id = request->getParamAs<int64_t>("user_id");
        std::string ids = request->getParam("ids");
        if (user_id == 0 && ids.empty()) {
            result->setErrno(errcode::PARAM_MISSING);
            break;
        }

        std::vector<data::LabelInfo::ptr> infos;
        if (user_id) {
            LabelMgr::GetInstance()->listByUserId(infos, user_id, true);
        } else {
            auto tmp = chen::StringUtil::Split(ids, ",");
            for (auto& i : tmp) {
                auto id = chen::TypeUtil::Atoi(i);
                if (id) {
                    auto info = LabelMgr::GetInstance()->get(id);
                    if (info) {
                        infos.push_back(info);
                    }
                }
            }
        }
        for (auto& i : infos) {
            Json::Value v;
            v["id"] = i->getId();
            v["name"] = i->getName();
            v["color"] = i->getColor();
            v["desc"] = i->getDescription();
            v["create_time"] = i->getCreateTime();
            result->jsondata.append(v);
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
