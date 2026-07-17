#include "assignment_delete_servlet.h"

#include "../../manager/assignment_manager.h"
#include "../../manager/user_manager.h"
#include "../../manager/notification_manager.h"
#include "../../util.h"
#include "blog/data/assignment_info.h"
#include "blog/data/assignment_user_rel_info.h"

#include <chen/log/log.h>
#include <chen/db/query_builder.h>

#include <set>

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

AssignmentDeleteServlet::AssignmentDeleteServlet()
    : BlogLoginedServlet("AssignmentDeleteServlet") {
}

int32_t AssignmentDeleteServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_STRING(result, ids, "ids");
        std::set<int64_t> assignment_ids;
        auto tmp = chen::StringUtil::Split(ids, ",");
        for (auto& i : tmp) {
            assignment_ids.insert(chen::TypeUtil::Atoi(i));
        }

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setErrno(errcode::NOT_LOGIN);
            break;
        }
        auto current_user = UserMgr::GetInstance()->get(uid);
        if (!current_user) {
            result->setErrno(errcode::USER_NOT_FOUND);
            break;
        }
        if (current_user->getRole() != UserManager::Role::ADMIN) {
            result->setErrno(errcode::ACCESS_DENIED);
            break;
        }
        std::vector<data::AssignmentInfo::ptr> infos;
        if (!AssignmentMgr::GetInstance()->listByPages(infos, 0, INT32_MAX, -1, true)) {
            break;
        }

        std::vector<data::AssignmentInfo::ptr> del_assignments;
        for (auto& i : infos) {
            if (assignment_ids.count(i->getId())) {
                del_assignments.push_back(i);
            }
        }

        auto db = getDB();
        if (!db) {
            result->setErrno(errcode::DB_OPERATION_FAILED);
            break;
        }

        auto trans = db->openTransaction();
        if (!trans) {
            result->setErrno(errcode::DB_TRANSACTION_FAILED);
            break;
        }
        time_t now = time(0);
        for (auto& i : del_assignments) {
            i->setIsDeleted(1);
            i->setUpdateTime(now);
            data::AssignmentInfoDao::Update(i, db);
        }
        if (!trans->commit()) {
            ERROR(logger) << "commit fail";
            result->setErrno(errcode::DB_COMMIT_FAILED);

            for (auto& i : del_assignments) {
                i->setIsDeleted(0);
            }
            break;
        }
        if (!del_assignments.empty()) {
            auto& jids = result->jsondata["ids"];
            for (auto& i : del_assignments) {
                jids.append(i->getId());
            }

            // Notify submitters
            for (auto& assign : del_assignments) {
                std::string assign_name = assign->getName();
                std::set<int64_t> notified;
                auto db2 = getDB();
                if (db2) {
                    auto qb = data::AssignmentUserRelInfoDao::newQuery();
                    qb->where("assignment_id", "=", assign->getId());
                    qb->where("is_deleted", "=", (int64_t)0);
                    auto stmt = db2->prepare(qb->buildQuerySQL());
                    if (stmt) {
                        qb->bindParams(stmt);
                        auto rt = stmt->query();
                        if (rt) {
                            while (rt->next()) {
                                auto info = data::AssignmentUserRelInfoDao::ParseRow(rt);
                                if (!info) continue;
                                int64_t submitter_id = info->getUserId();
                                if (notified.count(submitter_id)) continue;
                                notified.insert(submitter_id);
                                chen::IOManager::GetThis()->schedule([submitter_id, assign_name]() {
                                    std::string title = "作业已删除";
                                    std::string content = "作业「" + assign_name + "」已被管理员删除";
                                    auto notif = NotificationMgr::GetInstance()->addNotification(
                                        submitter_id, title, content, "assignment_deleted", 0);
                                    if (notif) {
                                        Json::Value wsMsg;
                                        wsMsg["id"] = notif->getId();
                                        wsMsg["title"] = title;
                                        wsMsg["content"] = content;
                                        wsMsg["type"] = "assignment_deleted";
                                        wsMsg["is_read"] = false;
                                        wsMsg["create_time"] = notif->getCreateTime();
                                        NotificationMgr::GetInstance()->sendToUser(submitter_id, chen::JsonUtil::ToString(wsMsg));
                                    }
                                });
                            }
                        }
                    }
                }
            }
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
