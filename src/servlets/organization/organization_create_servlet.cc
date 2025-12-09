#include "organization_create_servlet.h"
#include <chen/log/log.h>
#include "../../util.h"
#include "../../manager/user_manager.h"
#include "../../manager/organization_manager.h"
#include "../../types.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

OrganizationCreateServlet::OrganizationCreateServlet()
    : BlogLoginedServlet("OrganizationCreateServlet") {
}

int32_t OrganizationCreateServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, name, "name");
        DEFINE_AND_CHECK_STRING(result, type, "type");
        DEFINE_AND_CHECK_TYPE(result, int32_t, status, "status");
        std::string desc = request->getParamAs<std::string>("desc");
        int64_t oid = request->getParamAs<int64_t>("id", 0);

        int64_t user_id = getUserId(request);
        if (!user_id) {
            result->setResult(410, "not login");
            break;
        }

        auto uinfo = UserMgr::GetInstance()->get(user_id);
        if (!uinfo) {
            result->setResult(420, "user not exist");
            break;
        }

        if (uinfo->getRole() != (int32_t)types::Role::System::ADMIN) {
            result->setResult(403, "Access Denied");
            break;
        }

        bool new_org = false;
        data::OrganizationInfo::ptr info;
        if (oid) {
            info = OrganizationMgr::GetInstance()->get(oid);
            if (!info) {
                result->setResult(404, "organization not exist");
                break;
            }

            info->setName(name);
        } else {
            info = OrganizationMgr::GetInstance()->getByName(name);
            if (!info) {
                info = std::make_shared<data::OrganizationInfo>();
                info->setName(name);
                info->setCreateTime(time(0));
                new_org = true;
            } else if (info->getIsDeleted()) {
                info->setCreateTime(time(0));
            }
        }
        info->setType(type);
        info->setStatus(status);
        info->setOwnerId(user_id);
        info->setDescription(desc);
        info->setIsDeleted(0);
        info->setUpdateTime(time(0));

        auto db = getDB();
		if (!db) {
			result->setResult(500, "get db error");
			break;
		}

        if (data::OrganizationInfoDao::InsertOrUpdate(info, db)) {
			result->setResult(500, "insert or update organization fail");
			ERROR(logger) << "db error, errno=" << db->getErrno()
				<< " errstr=" << db->getErrStr();
			break;
		}

        if (new_org) {
            OrganizationMgr::GetInstance()->add(info);
        }

        result->set("id", info->getId());
        result->set("name", info->getName());
        result->set("type", info->getType());
        result->set("status", info->getStatus());
        result->set("description", info->getDescription());
        result->set("create_time", info->getCreateTime());
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
