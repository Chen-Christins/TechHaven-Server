#include "comment_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 1000;

CommentManager::CommentManager()
    :m_cache(kCacheMaxSize, 0, nullptr) {
}

data::CommentInfo::ptr CommentManager::parseRow(chen::ISQLData::ptr rt) {
    data::CommentInfo::ptr v(new data::CommentInfo);
    v->setId(rt->getInt64(0));
    v->setArticleId(rt->getInt64(1));
    v->setUserId(rt->getInt64(2));
    v->setParentId(rt->getInt64(3));
    v->setContent(rt->getString(4));
    v->setIp(rt->getString(5));
    v->setUserAgent(rt->getString(6));
    v->setStatus(rt->getInt32(7));
    v->setIsReported(rt->getInt32(8));
    v->setReportCount(rt->getInt32(9));
    v->setIsDeleted(rt->getInt32(10));
    v->setCreateTime(rt->getTime(11));
    v->setUpdateTime(rt->getTime(12));
    return v;
}

bool CommentManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }
    INFO(logger) << "CommentManager loadAll: DB connection verified, no preloading needed";
    return true;
}

void CommentManager::add(data::CommentInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

data::CommentInfo::ptr CommentManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::CommentInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

data::CommentInfo::ptr CommentManager::create(int64_t article_id, int64_t user_id,
    const std::string& content, int64_t parent_id,
    const std::string& ip, const std::string& user_agent) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }

    auto info = std::make_shared<data::CommentInfo>();
    info->setArticleId(article_id);
    info->setUserId(user_id);
    info->setContent(content);
    info->setParentId(parent_id);
    info->setIp(ip);
    info->setUserAgent(user_agent);
    info->setStatus(APPROVED);
    info->setIsReported(0);
    info->setReportCount(0);
    info->setIsDeleted(0);
    info->setCreateTime(time(0));
    info->setUpdateTime(time(0));

    if (data::CommentInfoDao::Insert(info, db)) {
        ERROR(logger) << "CommentManager create Insert fail";
        return nullptr;
    }

    m_cache.set(info->getId(), info);
    return info;
}

bool CommentManager::update(int64_t id, const std::string& content) {
    auto info = get(id);
    if (!info || info->getIsDeleted()) {
        return false;
    }

    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }

    info->setContent(content);
    info->setUpdateTime(time(0));
    if (data::CommentInfoDao::Update(info, db)) {
        ERROR(logger) << "CommentManager update fail";
        return false;
    }
    return true;
}

bool CommentManager::del(int64_t id) {
    auto info = get(id);
    if (!info || info->getIsDeleted()) {
        return false;
    }

    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }

    info->setIsDeleted(1);
    info->setUpdateTime(time(0));
    if (data::CommentInfoDao::Update(info, db)) {
        ERROR(logger) << "CommentManager del Update fail";
        return false;
    }
    return true;
}

void CommentManager::listAllByArticle(std::vector<data::CommentInfo::ptr>& results, int64_t article_id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return;
    }
    auto qb = chen::QueryBuilder::Create("comment");
    qb->where("article_id", "=", article_id);
    qb->where("is_deleted", "=", (int64_t)0);
    qb->where("status", "=", (int64_t)APPROVED);
    qb->orderBy("id", "ASC");
    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (!rt) {
        return;
    }
    while (rt->next()) {
        results.push_back(parseRow(rt));
    }
}

void CommentManager::listByArticle(std::vector<data::CommentInfo::ptr>& results,
    int64_t article_id, uint64_t offset, uint64_t size) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return;
    }
    auto qb = chen::QueryBuilder::Create("comment");
    qb->where("article_id", "=", article_id);
    qb->where("parent_id", "=", (int64_t)0);
    qb->where("is_deleted", "=", (int64_t)0);
    qb->where("status", "=", (int64_t)APPROVED);
    qb->orderBy("id", "DESC");
    qb->limit((int32_t)size);
    qb->offset((int32_t)offset);
    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (!rt) {
        return;
    }
    while (rt->next()) {
        results.push_back(parseRow(rt));
    }
}

void CommentManager::listReplies(std::vector<data::CommentInfo::ptr>& results,
    int64_t parent_id, uint64_t offset, uint64_t size) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return;
    }
    auto qb = chen::QueryBuilder::Create("comment");
    qb->where("parent_id", "=", parent_id);
    qb->where("is_deleted", "=", (int64_t)0);
    qb->where("status", "=", (int64_t)APPROVED);
    qb->orderBy("id", "ASC");
    qb->limit((int32_t)size);
    qb->offset((int32_t)offset);
    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (!rt) {
        return;
    }
    while (rt->next()) {
        results.push_back(parseRow(rt));
    }
}

int64_t CommentManager::countByArticle(int64_t article_id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("comment");
    qb->where("article_id", "=", article_id);
    qb->where("parent_id", "=", (int64_t)0);
    qb->where("is_deleted", "=", (int64_t)0);
    qb->where("status", "=", (int64_t)APPROVED);
    int64_t total = 0;
    if (qb->executeCount(total, db)) {
        ERROR(logger) << "countByArticle executeCount fail errno=" << db->getErrno();
        return 0;
    }
    return total;
}

int64_t CommentManager::countReplies(int64_t parent_id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("comment");
    qb->where("parent_id", "=", parent_id);
    qb->where("is_deleted", "=", (int64_t)0);
    qb->where("status", "=", (int64_t)APPROVED);
    int64_t total = 0;
    if (qb->executeCount(total, db)) {
        ERROR(logger) << "countReplies executeCount fail errno=" << db->getErrno();
        return 0;
    }
    return total;
}

int64_t CommentManager::listByAdmin(std::vector<data::CommentInfo::ptr>& results,
    int64_t page_num, int64_t page_size,
    int32_t status, const std::string& keyword,
    int64_t article_id, int32_t is_reported) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("comment");
    qb->where("is_deleted", "=", (int64_t)0);
    qb->whereIf(status > 0, "status", "=", (int64_t)status);
    qb->whereIf(article_id > 0, "article_id", "=", article_id);
    qb->whereIf(is_reported >= 0, "is_reported", "=", (int64_t)is_reported);
    qb->whereIf(!keyword.empty(), "content", "LIKE", "%" + keyword + "%");
    qb->orderBy("id", "DESC");

    int64_t total = 0;
    if (qb->executeCount(total, db)) {
        ERROR(logger) << "listByAdmin executeCount fail errno=" << db->getErrno();
        return 0;
    }

    qb->limit((int32_t)page_size);
    qb->offset((int32_t)((page_num - 1) * page_size));
    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return 0;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (!rt) {
        return 0;
    }
    while (rt->next()) {
        results.push_back(parseRow(rt));
    }
    return total;
}

int64_t CommentManager::batchUpdateStatus(const std::vector<int64_t>& ids, int32_t status) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }

    int64_t affected = 0;
    for (auto& id : ids) {
        auto info = get(id);
        if (!info || info->getIsDeleted()) {
            continue;
        }
        info->setStatus(status);
        info->setUpdateTime(time(0));
        if (data::CommentInfoDao::Update(info, db) == 0) {
            affected++;
        }
    }
    return affected;
}

int64_t CommentManager::batchDelete(const std::vector<int64_t>& ids) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }

    int64_t affected = 0;
    for (auto& id : ids) {
        auto info = get(id);
        if (!info || info->getIsDeleted()) {
            continue;
        }
        info->setIsDeleted(1);
        info->setUpdateTime(time(0));
        if (data::CommentInfoDao::Update(info, db) == 0) {
            affected++;
        }
    }
    return affected;
}

CommentManager::CommentStats CommentManager::getStats() {
    CommentStats stats;
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return stats;
    }

    // Query status counts with GROUP BY
    {
        auto qb = chen::QueryBuilder::Create("comment");
        qb->select("status, COUNT(*) AS cnt");
        qb->where("is_deleted", "=", (int64_t)0);
        qb->groupBy("status");
        std::string sql = qb->buildQuerySQL();
        auto stmt = db->prepare(sql);
        if (stmt) {
            qb->bindParams(stmt);
            auto rt = stmt->query();
            if (rt) {
                while (rt->next()) {
                    int32_t s = rt->getInt32(0);
                    int64_t cnt = rt->getInt64(1);
                    stats.total += cnt;
                    switch (s) {
                    case PENDING:
                        stats.pending = cnt;
                        break;
                    case APPROVED:
                        stats.approved = cnt;
                        break;
                    case SPAM:
                        stats.spam = cnt;
                        break;
                    }
                }
            }
        }
    }

    // Query reported count
    {
        auto qb = chen::QueryBuilder::Create("comment");
        qb->where("is_deleted", "=", (int64_t)0);
        qb->where("is_reported", "=", (int64_t)1);
        int64_t reported = 0;
        if (qb->executeCount(reported, db) == 0) {
            stats.reported = reported;
        }
    }

    return stats;
}

}
