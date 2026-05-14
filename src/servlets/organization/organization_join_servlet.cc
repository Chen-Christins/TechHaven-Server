#include "organization_join_servlet.h"
#include <chen/log/log.h>
#include <json/json.h>
#include "../../util.h"
#include "../../manager/organization_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/user_manager.h"
#include "../../manager/notification_manager.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

OrganizationJoinServlet::OrganizationJoinServlet()
    : BlogLoginedServlet("OrganizationJoinServlet") {
}

int32_t OrganizationJoinServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, id, "id");

        int64_t uid = getUserId(request);

        auto org = OrganizationMgr::GetInstance()->get(id);
        if (!org || org->getIsDeleted()) {
            result->setResult(404, "invalid id");
            break;
        }

        if (org->getStatus() == OrganizationManager::Status::INACTIVE) {
            result->setResult(403, "organization is disabled");
            break;
        }

        auto info = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(id, uid);
        if (!info) {
            info = std::make_shared<data::OrganizationUserRelInfo>();
            info->setOrgId(id);
            info->setUserId(uid);
            info->setCreateTime(time(0));
        } else if (info->getStatus() == OrganizationUserRelManager::Status::APPROVED) {
            result->setResult(403, "you have joined this organization");
            break;
        } else if (info->getStatus() == OrganizationUserRelManager::Status::PENDING) {
            result->setResult(403, "you have applied to join this organization");
            break;
        }
        info->setRole(OrganizationManager::Role::MEMBER); // 普通成员
        info->setStatus(OrganizationUserRelManager::Status::PENDING); // 申请中
        info->setUpdateTime(time(0));

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db error");
            break;
        }

        if (data::OrganizationUserRelInfoDao::InsertOrUpdate(info, db)) {
            result->setResult(500, "insert or update organization fail");
            ERROR(logger) << "db error, errno=" << db->getErrno()
				<< " errstr=" << db->getErrStr();
            break;
        }

        OrganizationUserRelMgr::GetInstance()->add(info);

        // 通知组织管理员及拥有者有新的加入申请
        {
            auto applicant = UserMgr::GetInstance()->get(uid);
            std::string applicant_name = applicant ? applicant->getName() : std::to_string(uid);
            std::string title = "新的加入申请";
            std::string content = "用户「" + applicant_name + "」申请加入组织「" + org->getName() + "」";

            std::vector<data::OrganizationUserRelInfo::ptr> members;
            OrganizationUserRelMgr::GetInstance()->getByPages(members, id, 0, 10000, -1, true);
            for (auto& m : members) {
                if (m->getRole() == OrganizationManager::Role::ADMIN
                        || m->getRole() == OrganizationManager::Role::OWNER) {
                    auto notifInfo = NotificationMgr::GetInstance()->addNotification(
                        m->getUserId(), title, content, "org_join_request", uid);
                    if (notifInfo) {
                        Json::Value wsMsg;
                        wsMsg["id"] = notifInfo->getId();
                        wsMsg["title"] = title;
                        wsMsg["content"] = content;
                        wsMsg["type"] = "org_join_request";
                        wsMsg["is_read"] = false;
                        wsMsg["create_time"] = notifInfo->getCreateTime();
                        NotificationMgr::GetInstance()->sendToUser(
                            m->getUserId(), chen::JsonUtil::ToString(wsMsg));
                    }
                }
            }
        }

        result->set("id", org->getId());
        result->set("name", org->getName());
        result->set("description", org->getDescription());
        result->set("type", org->getType());
        result->set("status", org->getStatus());
        result->set("user_in_org", info->getStatus());
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
