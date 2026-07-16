#include "feedback_manager.h"

#include "../util.h"

#include <chen/log/log.h>

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

FeedbackManager::FeedbackManager() {
}

bool FeedbackManager::list(std::vector<data::UserFeedbackInfo::ptr>& infos,
        const std::string& type, int32_t page, int32_t page_size, int64_t* total) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }

    auto qb = chen::QueryBuilder::Create("user_feedback");
    qb->where("is_deleted", "=", (int64_t)0);

    if (!type.empty()) {
        qb->where("type", "=", type);
    }

    // 如果需要总数，先查 count
    if (total) {
        std::string countSql = qb->buildCountSQL();
        auto countStmt = db->prepare(countSql);
        if (!countStmt) {
            ERROR(logger) << "count stmt=" << countSql
                << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
            return false;
        }
        qb->bindQueryParams(countStmt);
        auto countRt = countStmt->query();
        if (!countRt) {
            ERROR(logger) << "count query failed";
            return false;
        }
        if (countRt->next()) {
            *total = countRt->getInt64(0);
        }
        if (*total == 0) {
            return true;
        }
    }

    qb->orderBy("id", "DESC");

    int32_t offset = (page - 1) * page_size;
    std::string sql = qb->buildQuerySQL("id, type, content, contact, user_id, is_deleted, create_time, update_time", false);
    sql += " limit ? offset ?";

    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql
            << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return false;
    }

    qb->bindQueryParams(stmt);
    int idx = qb->getQueryParamCount() + 1;
    stmt->bindInt32(idx++, page_size);
    stmt->bindInt32(idx++, offset);

    auto rt = stmt->query();
    if (!rt) {
        ERROR(logger) << "query failed";
        return false;
    }

    while (rt->next()) {
        auto info = data::UserFeedbackInfoDao::ParseRow(rt);
        if (info) {
            infos.push_back(info);
        }
    }

    return true;
}

data::UserFeedbackInfo::ptr FeedbackManager::get(int64_t id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }

    auto qb = chen::QueryBuilder::Create("user_feedback");
    qb->where("id", "=", id);
    qb->where("is_deleted", "=", (int64_t)0);

    std::string sql = qb->buildQuerySQL("id, type, content, contact, user_id, is_deleted, create_time, update_time");
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql
            << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return nullptr;
    }
    qb->bindParams(stmt);

    auto rt = stmt->query();
    if (!rt) {
        return nullptr;
    }

    if (!rt->next()) {
        return nullptr;
    }

    return data::UserFeedbackInfoDao::ParseRow(rt);
}

bool FeedbackManager::remove(int64_t id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }

    auto info = get(id);
    if (!info) {
        ERROR(logger) << "feedback not found: " << id;
        return false;
    }

    info->setIsDeleted(1);
    info->setUpdateTime(time(0));

    if (data::UserFeedbackInfoDao::Update(info, db)) {
        ERROR(logger) << "soft delete feedback failed, errno=" << db->getErrno()
            << " errstr=" << db->getErrStr();
        return false;
    }

    return true;
}

}
