#include "label_create_servlet.h"
#include "../../manager/label_manager.h"
#include <chen/log/log.h>
#include "../../util.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

LabelCreateServlet::LabelCreateServlet()
    : BlogLoginedServlet("LabelCreateServlet") {
}

int32_t LabelCreateServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        ,chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, name, "name");
        DEFINE_AND_CHECK_STRING(result, color, "color");
        std::string desc = request->getParamAs<std::string>("desc");
        int64_t lid = request->getParamAs<int64_t>("id", 0);

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }

        bool new_label = false;
        data::LabelInfo::ptr info;
        if (lid) {
            info = LabelMgr::GetInstance()->get(lid);
            if (!info) {
                result->setResult(401, "invalid lid");
                break;
            }
            if (info->getUserId() != uid) {
                result->setResult(402, "no permission");
                break;
            }
            info->setName(name);
        } else {
            info = LabelMgr::GetInstance()->getByUserIdName(uid, name);
            if (!info) {
                info.reset(new data::LabelInfo);
                info->setUserId(uid);
                info->setName(name);
                info->setCreateTime(time(0));
                new_label = true;
            } else if (info->getIsDeleted()) {
                info->setIsDeleted(0);
            }
        }
        info->setDescription(desc);
        info->setColor(color);
        info->setIsDeleted(0);
        info->setUpdateTime(time(0));

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db error");
            break;
        }

        if (data::LabelInfoDao::InsertOrUpdate(info, db)) {
            result->setResult(500, "insert or update label fail");
            ERROR(logger) << "db error, errno=" << db->getErrno()
                << " errstr=" << db->getErrStr();
            break;
        }

        if (new_label) {
            LabelMgr::GetInstance()->add(info);
        }

        result->set("id", info->getId());
        result->set("name", info->getName());
        result->set("color", info->getColor());
        result->set("desc", info->getDescription());
        result->set("create_time", info->getCreateTime());
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
