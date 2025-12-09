#include "category_query_servlet.h"
#include "../../manager/category_manager.h"
#include "../../manager/user_manager.h"
#include <chen/log/log.h>


namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

CategoryQueryServlet::CategoryQueryServlet()
    :BlogLoginedServlet("CategoryQueryServlet") {
}

int32_t CategoryQueryServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
	do {
        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }
        
        std::vector<blog::data::CategoryInfo::ptr> infos;
        CategoryMgr::GetInstance()->listAll(infos, true);

        result->set("total", infos.size());
        auto& list = result->jsondata["list"];
        for (const auto& cat : infos) {
            Json::Value item;
            item["id"] = cat->getId();
            item["name"] = cat->getName();
            item["url"] = cat->getUrl();
            item["icon"] = cat->getIcon();
            item["color"] = cat->getColor();
            item["description"] = cat->getDescription();
            item["parent_id"] = cat->getParentId();
            item["status"] = cat->getStatus();
            list.append(item);
        }
	} while (0);
	response->setBody(result->toJsonString());
    return 0;
}

}
}
