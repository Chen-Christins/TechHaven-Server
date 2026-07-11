#include "database_stats_servlet.h"

#include "../../manager/article_manager.h"
#include "../../manager/comment_manager.h"
#include "../../manager/user_manager.h"
#include "../../manager/category_manager.h"
#include "../../manager/backup_record_manager.h"
#include "../../util.h"

namespace blog {
namespace servlet {

DatabaseStatsServlet::DatabaseStatsServlet()
    : BlogLoginedServlet("DatabaseStatsServlet") {
}

int32_t DatabaseStatsServlet::handle(chen::http::HttpRequest::ptr request,
        chen::http::HttpResponse::ptr response, chen::http::HttpSession::ptr session,
        Result::ptr result) {
    do {
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

        auto astats = ArticleMgr::GetInstance()->getStats(0, -1, 0, "");
        auto cstats = CommentMgr::GetInstance()->getStats();

        int64_t total_users = 0;
        std::vector<int64_t> uids;
        UserMgr::GetInstance()->getAllIds(uids, true);
        total_users = uids.size();

        int64_t total_categories = 0;
        std::vector<data::CategoryInfo::ptr> categories;
        CategoryMgr::GetInstance()->listAll(categories, true);
        total_categories = categories.size();

        int64_t total_labels = 0;
        auto db = GetDB();
        if (db) {
            auto stmt = db->prepare("SELECT COUNT(*) FROM label WHERE is_deleted = 0");
            if (stmt) {
                auto rt = stmt->query();
                if (rt && rt->next()) {
                    total_labels = rt->getInt64(0);
                }
            }
        }

        int64_t total_backups = 0;
        std::vector<data::BackupRecordInfo::ptr> backups;
        BackupRecordMgr::GetInstance()->list(backups, total_backups, "", "", "", 0, 1);

        result->set("totalSize", (int64_t)0);
        result->set("usedSize", (int64_t)0);
        result->set("availableSize", (int64_t)0);
        result->set("totalRecords", astats.total + total_users + cstats.total + total_categories + total_labels);
        result->set("articles", astats.total);
        result->set("users", total_users);
        result->set("comments", cstats.total);
        result->set("categories", total_categories);
        result->set("tags", total_labels);
        result->set("backups", total_backups);

        result->setErrno(errcode::SUCCESS);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
