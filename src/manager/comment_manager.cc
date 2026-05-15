#include "comment_manager.h"
#include <chen/log/log.h>
#include "../util.h"
#include <algorithm>

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

bool CommentManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "get db connection fail";
        return false;
    }
    std::vector<data::CommentInfo::ptr> results;
    if (data::CommentInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "CommentManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, data::CommentInfo::ptr> datas;
    std::unordered_map<int64_t, std::map<int64_t, data::CommentInfo::ptr>> articleComments;
    std::unordered_map<int64_t, std::map<int64_t, data::CommentInfo::ptr>> replies;
    for (auto& i : results) {
        datas[i->getId()] = i;
        articleComments[i->getArticleId()][i->getId()] = i;
        if (i->getParentId() > 0) {
            replies[i->getParentId()][i->getId()] = i;
        }
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_articleComments.swap(articleComments);
    m_replies.swap(replies);

    return true;
}

void CommentManager::add(data::CommentInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
    m_articleComments[info->getArticleId()][info->getId()] = info;
    if (info->getParentId() > 0) {
        m_replies[info->getParentId()][info->getId()] = info;
    }
}

data::CommentInfo::ptr CommentManager::get(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    return it == m_datas.end() ? nullptr : it->second;
}

data::CommentInfo::ptr CommentManager::create(int64_t article_id, int64_t user_id,
    const std::string& content, int64_t parent_id,
    const std::string& ip, const std::string& user_agent) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "get db connection fail";
        return nullptr;
    }

    auto info = std::make_shared<data::CommentInfo>();
    info->setArticleId(article_id);
    info->setUserId(user_id);
    info->setContent(content);
    info->setParentId(parent_id);
    info->setIp(ip);
    info->setUserAgent(user_agent);
    info->setStatus(APPROVED); // auto-approve for now
    info->setIsReported(0);
    info->setReportCount(0);
    info->setIsDeleted(0);
    info->setCreateTime(time(0));
    info->setUpdateTime(time(0));

    if (data::CommentInfoDao::Insert(info, db)) {
        ERROR(logger) << "CommentManager create Insert fail";
        return nullptr;
    }

    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_datas[info->getId()] = info;
        m_articleComments[info->getArticleId()][info->getId()] = info;
        if (parent_id > 0) {
            m_replies[parent_id][info->getId()] = info;
        }
    }

    return info;
}

bool CommentManager::update(int64_t id, const std::string& content) {
    auto info = get(id);
    if (!info || info->getIsDeleted()) {
        return false;
    }

    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "get db connection fail";
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
        ERROR(logger) << "get db connection fail";
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

void CommentManager::listByArticle(std::vector<data::CommentInfo::ptr>& results,
    int64_t article_id, uint64_t offset, uint64_t size) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_articleComments.find(article_id);
    if (it == m_articleComments.end()) {
        return;
    }
    auto& commentMap = it->second;
    uint64_t idx = 0;
    // Iterate in reverse order (latest first) for top-level comments
    for (auto rit = commentMap.rbegin(); rit != commentMap.rend(); ++rit) {
        auto& info = rit->second;
        if (info->getIsDeleted() || info->getParentId() != 0 || info->getStatus() != APPROVED) {
            continue;
        }
        if (idx >= offset && results.size() < size) {
            results.push_back(info);
        }
        idx++;
        if (results.size() >= size) {
            break;
        }
    }
}

void CommentManager::listReplies(std::vector<data::CommentInfo::ptr>& results,
    int64_t parent_id, uint64_t offset, uint64_t size) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_replies.find(parent_id);
    if (it == m_replies.end()) {
        return;
    }
    auto& replyMap = it->second;
    uint64_t idx = 0;
    // Replies in forward order (earliest first)
    for (auto& [id, info] : replyMap) {
        if (info->getIsDeleted() || info->getStatus() != APPROVED) {
            continue;
        }
        if (idx >= offset && results.size() < size) {
            results.push_back(info);
        }
        idx++;
        if (results.size() >= size) {
            break;
        }
    }
}

int64_t CommentManager::countByArticle(int64_t article_id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_articleComments.find(article_id);
    if (it == m_articleComments.end()) {
        return 0;
    }
    int64_t count = 0;
    for (auto& [id, info] : it->second) {
        if (!info->getIsDeleted() && info->getParentId() == 0 && info->getStatus() == APPROVED) {
            count++;
        }
    }
    return count;
}

int64_t CommentManager::countReplies(int64_t parent_id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_replies.find(parent_id);
    if (it == m_replies.end()) {
        return 0;
    }
    int64_t count = 0;
    for (auto& [id, info] : it->second) {
        if (!info->getIsDeleted() && info->getStatus() == APPROVED) {
            count++;
        }
    }
    return count;
}

// --- admin methods ---

int64_t CommentManager::listByAdmin(std::vector<data::CommentInfo::ptr>& results,
    int64_t page_num, int64_t page_size,
    int32_t status, const std::string& keyword,
    int64_t article_id, int32_t is_reported) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);

    std::vector<data::CommentInfo::ptr> filtered;
    for (auto& [id, info] : m_datas) {
        if (info->getIsDeleted()) {
            continue;
        }
        // filter by status
        if (status > 0 && info->getStatus() != status) {
            continue;
        }
        // filter by article_id
        if (article_id > 0 && info->getArticleId() != article_id) {
            continue;
        }
        // filter by is_reported
        if (is_reported >= 0 && info->getIsReported() != is_reported) {
            continue;
        }
        // filter by keyword (search in content)
        if (!keyword.empty()) {
            if (info->getContent().find(keyword) == std::string::npos) {
                continue;
            }
        }
        filtered.push_back(info);
    }

    // sort by create_time descending (latest first)
    std::sort(filtered.begin(), filtered.end(),
        [](const data::CommentInfo::ptr& a, const data::CommentInfo::ptr& b) {
            return a->getCreateTime() > b->getCreateTime();
        });

    int64_t total = filtered.size();
    int64_t start = (page_num - 1) * page_size;
    if (start < 0) start = 0;

    for (int64_t i = start; i < total && (int64_t)results.size() < page_size; i++) {
        results.push_back(filtered[i]);
    }

    return total;
}

int64_t CommentManager::batchUpdateStatus(const std::vector<int64_t>& ids, int32_t status) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "get db connection fail";
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
        ERROR(logger) << "get db connection fail";
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
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    CommentStats stats;
    for (auto& [id, info] : m_datas) {
        if (info->getIsDeleted()) {
            continue;
        }
        stats.total++;
        switch (info->getStatus()) {
            case PENDING: stats.pending++; break;
            case APPROVED: stats.approved++; break;
            case SPAM: stats.spam++; break;
        }
        if (info->getIsReported()) {
            stats.reported++;
        }
    }
    return stats;
}

}
