#include "user_admin_lists_servlet.h"
#include "../../util.h"
#include "../../manager/user_manager.h"
#include <chen/util/json_util.h>
#include <chen/db/query_builder.h>

namespace blog {
namespace servlet {

UserAdminListsServlet::UserAdminListsServlet()
    : BlogLoginedServlet("UserAdminListsServlet") {
}

int32_t UserAdminListsServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, uint64_t, page_size, "page_size");
        DEFINE_AND_CHECK_TYPE(result, uint64_t, page_num, "page_num");
        int32_t rrole = request->getParamAs<int32_t>("role", -1);
        int32_t state = request->getParamAs<int32_t>("state", -1);
        int32_t days = request->getParamAs<int32_t>("days", 0);

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }
        int32_t role = UserMgr::GetInstance()->get(uid)->getRole();

        if (role != UserManager::Role::ADMIN) {
            result->setResult(403, "Access Denied");
            break;
        }

        uint64_t offset = (page_num - 1) * page_size;
        std::vector<data::UserInfo::ptr> users;
        uint64_t total = UserMgr::GetInstance()->listByPages(users, offset, page_size, rrole, state, days, true);

        // 批量查询文章数和评论数，避免逐用户查询
        std::map<int64_t, int64_t> article_counts, comment_counts;
        auto db = getDB();
        if (db && !users.empty()) {
            std::vector<int64_t> user_ids;
            for (auto& u : users) {
                user_ids.push_back(u->getId());
            }

            auto aqb = chen::QueryBuilder::Create("article");
            aqb->select("user_id, COUNT(*) as cnt");
            aqb->whereIn("user_id", user_ids);
            aqb->where("is_deleted", "=", (int64_t)0);
            aqb->groupBy("user_id");
            auto stmt = db->prepare(aqb->buildQuerySQL());
            if (stmt) {
                aqb->bindParams(stmt);
                auto rt = stmt->query();
                if (rt) {
                    while (rt->next()) {
                        article_counts[rt->getInt64(0)] = rt->getInt64(1);
                    }
                }
            }

            auto cqb = chen::QueryBuilder::Create("comment");
            cqb->select("user_id, COUNT(*) as cnt");
            cqb->whereIn("user_id", user_ids);
            cqb->where("is_deleted", "=", (int64_t)0);
            cqb->groupBy("user_id");
            stmt = db->prepare(cqb->buildQuerySQL());
            if (stmt) {
                cqb->bindParams(stmt);
                auto rt = stmt->query();
                if (rt) {
                    while (rt->next()) {
                        comment_counts[rt->getInt64(0)] = rt->getInt64(1);
                    }
                }
            }
        }

        result->set("total", total);
        auto& list = result->jsondata["list"];
        for (const auto& user : users) {
            Json::Value item;
            item["id"] = user->getId();
            item["name"] = user->getName();
            item["email"] = user->getEmail();
            item["avatar"] = user->getAvatar();
            item["role"] = user->getRole();
            item["state"] = user->getState();
            item["create_time"] = user->getCreateTime();
            item["login_time"] = user->getLoginTime();
            item["article_count"] = article_counts[user->getId()];
            item["comment_count"] = comment_counts[user->getId()];
            list.append(item);
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}