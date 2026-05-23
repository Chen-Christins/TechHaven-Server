#include "article_manager.h"
#include "../util.h"
#include <chen/log/log.h>
#include <chen/iomanager/iomanager.h>
#include <chen/db/redis.h>
#include "user_manager.h"
#include "article_label_rel_manager.h"
#include "article_category_rel_manager.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

bool ArticleManager::loadAll() {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get Sqlite3 connection fail";
        return false;
    }
    std::vector<data::ArticleInfo::ptr> results;
    if (blog::data::ArticleInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "ArticleManager loadAll fail";
        return false;
    }

    std::map<int64_t, blog::data::ArticleInfo::ptr> datas;
    std::unordered_map<int64_t, std::map<int64_t, blog::data::ArticleInfo::ptr>> users;
    std::map<int64_t, blog::data::ArticleInfo::ptr> verifys;

    for (auto& i : results) {
        datas[i->getId()] = i;
        users[i->getUserId()][i->getId()] = i;
        if (i->getState() == 1) {
            verifys[i->getId()] = i;
        }
    }
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas.swap(datas);
    m_users.swap(users);
    m_verifys.swap(verifys);
    return true;
}

void ArticleManager::add(blog::data::ArticleInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_datas[info->getId()] = info;
    m_users[info->getUserId()][info->getId()] = info;
    if (info->getState() == 1 && info->getIsDeleted() == 0) {
        m_verifys[info->getId()] = info;
    }
}

#define XX(map, key)                                   \
    std::shared_lock<std::shared_mutex> lock(m_mutex); \
    auto it = map.find(key);                           \
    return it == map.end() ? nullptr : it->second;

blog::data::ArticleInfo::ptr ArticleManager::get(int64_t id) {
    XX(m_datas, id);
}

#undef XX

bool ArticleManager::listByUserId(std::vector<data::ArticleInfo::ptr>& infos, int64_t id, bool valid) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_users.find(id);
    if (it == m_users.end()) {
        return false;
    }
    for (auto& i : it->second) {
        if (!valid || !i.second->getIsDeleted()) {
            infos.push_back(i.second);
        }
    }
    return true;
}

int64_t ArticleManager::listByUserIdPages(std::vector<data::ArticleInfo::ptr>& infos, int64_t id
        ,int32_t offset, int32_t size, bool valid, int state) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    if (id == 0) {
        if (offset >= (int32_t)m_datas.size()) {
            return m_datas.size();
        }

		int64_t sum = 0;
		for (auto i : m_datas) {
			if (!i.second->getIsDeleted()) {
				if (!state) {
                    ++sum;
                } else if (i.second->getState() == state) {
                    ++sum;
                }
			}
		}

        auto it = m_datas.rbegin();
		int oft = 0;
		while (it != m_datas.rend() && oft < offset) {
			if (!it->second->getIsDeleted()) {
				if (!state) {
                    ++oft;
                } else if (it->second->getState() == state) {
                    ++oft;
                }
			}
			++it;
		}
        for (; (int32_t)infos.size() < size && it != m_datas.rend(); ++it) {
            if (!valid || !it->second->getIsDeleted()) {
                if (!state || it->second->getState() == state) {
                    infos.push_back(it->second);
                }
            }
        }
        return sum;
    } else {
        auto uit = m_users.find(id);
        if (uit == m_users.end()) {
            return 0;
        }
        if (offset >= (int32_t)uit->second.size()) {
            return uit->second.size();
        }

		int64_t sum = 0;
		for (auto i : uit->second) {
            if (!i.second->getIsDeleted()) {
                if (!state) {
                    ++sum;
                } else if (i.second->getState() == state) {
                    ++sum;
                }
            }
		}

        auto it = uit->second.rbegin();
		int oft = 0;
		while (it != uit->second.rend() && oft < offset) {
			if (!it->second->getIsDeleted()) {
				if (!state) {
                    ++oft;
                } else if (it->second->getState() == state) {
                    ++oft;
                }
			}
			++it;
		}
        for (; (int32_t)infos.size() < size && it != uit->second.rend(); ++it) {
            if (!valid || !it->second->getIsDeleted()) {
                if (!state || it->second->getState() == state) {
                    infos.push_back(it->second);
                }
            }
        }
        return sum;
    }
}

int64_t ArticleManager::listByLabelPages(std::vector<data::ArticleInfo::ptr>& infos, int64_t label_id
        ,int32_t offset, int32_t size, bool valid) {
    std::vector<data::ArticleLabelRelInfo::ptr> rels;
    ArticleLabelRelMgr::GetInstance()->listByLabelId(rels, label_id, valid);

    std::shared_lock<std::shared_mutex> lock(m_mutex);

    std::vector<data::ArticleInfo::ptr> matched;
    for (auto& rel : rels) {
        auto it = m_datas.find(rel->getArticleId());
        if (it != m_datas.end()
                && (!valid || !it->second->getIsDeleted())
                && it->second->getState() == Status::PUBLISHED) {
            matched.push_back(it->second);
        }
    }
    std::sort(matched.begin(), matched.end(), [](const data::ArticleInfo::ptr& a, const data::ArticleInfo::ptr& b) {
        return a->getId() > b->getId();
    });

    int64_t total = matched.size();

    if (offset < (int32_t)matched.size()) {
        for (int32_t i = offset; i < (int32_t)matched.size() && (int32_t)infos.size() < size; ++i) {
            infos.push_back(matched[i]);
        }
    }
    return total;
}

int64_t ArticleManager::listByCategoryPages(std::vector<data::ArticleInfo::ptr>& infos, int64_t category_id
        ,int32_t offset, int32_t size, bool valid) {
    std::vector<data::ArticleCategoryRelInfo::ptr> rels;
    ArticleCategoryRelMgr::GetInstance()->listByCategoryId(rels, category_id, valid);

    std::shared_lock<std::shared_mutex> lock(m_mutex);

    std::vector<data::ArticleInfo::ptr> matched;
    for (auto& rel : rels) {
        auto it = m_datas.find(rel->getArticleId());
        if (it != m_datas.end()
                && (!valid || !it->second->getIsDeleted())
                && it->second->getState() == Status::PUBLISHED) {
            matched.push_back(it->second);
        }
    }
    std::sort(matched.begin(), matched.end(), [](const data::ArticleInfo::ptr& a, const data::ArticleInfo::ptr& b) {
        return a->getId() > b->getId();
    });

    int64_t total = matched.size();

    if (offset < (int32_t)matched.size()) {
        for (int32_t i = offset; i < (int32_t)matched.size() && (int32_t)infos.size() < size; ++i) {
            infos.push_back(matched[i]);
        }
    }
    return total;
}

int64_t ArticleManager::listByPages(std::vector<data::ArticleInfo::ptr>& infos, int32_t offset, int state
        , int category, int32_t role, int32_t days, int32_t size, bool valid) {
    // 如果按分类筛选，先获取该分类下的文章ID集合
    std::set<int64_t> categoryArticleIds;
    if (category > 0) {
        std::vector<data::ArticleCategoryRelInfo::ptr> rels;
        ArticleCategoryRelMgr::GetInstance()->listByCategoryId(rels, category, true);
        for (auto& rel : rels) {
            categoryArticleIds.insert(rel->getArticleId());
        }
    }

    std::shared_lock<std::shared_mutex> lock(m_mutex);

    auto check = [&](data::ArticleInfo::ptr info) {
        if (valid && info->getIsDeleted()) {
            return false;
        }
        if (state && info->getState() != state) {
            return false;
        }
        if (category > 0 && categoryArticleIds.find(info->getId()) == categoryArticleIds.end()) {
            return false;
        }
        if (role != -1) {
            auto user = UserMgr::GetInstance()->get(info->getUserId());
            if (!user || user->getRole() != role) {
                return false;
            }
        }
        if (days) {
            time_t now = time(0);
            if (info->getCreateTime() < now - days * 24 * 3600) {
                return false;
            }
        }
        return true;
    };

    std::vector<data::ArticleInfo::ptr> temp;
    for (auto& i : m_datas) {
        if (check(i.second)) {
            temp.emplace_back(i.second);
        }
    }

    if (offset < (int32_t)temp.size()) {
        for (size_t i = offset; i < temp.size(); ++i) {
            if (infos.size() >= (size_t)size) {
                break;
            }
            infos.emplace_back(temp[i]);
        }
    }
    return temp.size();
}

void ArticleManager::delVerify(int64_t id) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_verifys.erase(id);
}

void ArticleManager::addVerify(data::ArticleInfo::ptr info) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_verifys[info->getId()] = info;
}

int64_t ArticleManager::listVerifyPages(std::vector<data::ArticleInfo::ptr>& infos, int32_t offset, int32_t size) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    if (offset >= (int32_t)m_verifys.size()) {
        return m_verifys.size();
    }
    auto it = m_verifys.begin();
    std::advance(it, offset);
    std::vector<int64_t> invalids;
    for (; (int32_t)infos.size() < size && it != m_verifys.end(); ++it) {
        if (it->second->getIsDeleted()) {
            invalids.push_back(it->first);
            continue;
        }
        if (it->second->getIsDeleted() != 1) {
            invalids.push_back(it->first);
            continue;
        }
        infos.push_back(it->second);
    }
    int64_t total = m_verifys.size();
    lock.unlock();
    for (auto& i : invalids) {
        delVerify(i);
    }
    return total;
}

std::pair<data::ArticleInfo::ptr, data::ArticleInfo::ptr> ArticleManager::nearby(int64_t id) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    if (it == m_datas.end()) {
        return std::pair(nullptr, nullptr);
    }
    data::ArticleInfo::ptr next;
    auto iit = it;
    ++iit;
    for (; iit != m_datas.end(); ++iit) {
        if (iit->second->getIsDeleted()) {
            continue;
        }
        if (iit->second->getState() == ArticleManager::Status::PUBLISHED) {
            next = iit->second;
            break;
        }
    }
    data::ArticleInfo::ptr prev;
    ASSERT(id == it->first);
    while (it != m_datas.begin()) {
        --it;
        if (it->second->getIsDeleted()) {
            continue;
        }
        if (it->second->getState() == ArticleManager::Status::PUBLISHED) {
            prev = it->second;
            break;
        }
    }
    return std::pair(prev, next);
}

ArticleManager::ArticleStats ArticleManager::getStats(int32_t category, int32_t role, int32_t days, const std::string& keyword) {
    ArticleStats stats;

    // 如果按分类筛选，先获取该分类下的文章ID集合
    std::set<int64_t> categoryArticleIds;
    if (category > 0) {
        std::vector<data::ArticleCategoryRelInfo::ptr> rels;
        ArticleCategoryRelMgr::GetInstance()->listByCategoryId(rels, category, true);
        for (auto& rel : rels) {
            categoryArticleIds.insert(rel->getArticleId());
        }
    }

    time_t now = time(0);
    std::shared_lock<std::shared_mutex> lock(m_mutex);

    for (auto& i : m_datas) {
        auto& info = i.second;
        if (info->getIsDeleted()) {
            continue;
        }

        // 分类筛选
        if (category > 0 && categoryArticleIds.find(info->getId()) == categoryArticleIds.end()) {
            continue;
        }

        // 角色筛选
        if (role != -1) {
            auto user = UserMgr::GetInstance()->get(info->getUserId());
            if (!user || user->getRole() != role) {
                continue;
            }
        }

        // 时间筛选
        if (days > 0) {
            if (info->getCreateTime() < now - days * 24 * 3600) {
                continue;
            }
        }

        // 关键词筛选
        if (!keyword.empty()) {
            if (info->getTitle().find(keyword) == std::string::npos) {
                continue;
            }
        }

        stats.total++;
        switch (info->getState()) {
        case Status::CHECKING:
            stats.pending++;
            break;
        case Status::PUBLISHED:
            stats.published++;
            break;
        case Status::REJECTED:
            stats.rejected++;
            break;
        }
    }

    return stats;
}

std::string ArticleManager::statusString() {
    std::stringstream ss;
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    ss << "ArticleManager total=" << m_datas.size()
       << " verify=" << m_verifys.size()
       << std::endl;
    for (auto& i : m_users) {
        ss << "    user(" << i.first << ") size=" << i.second.size() << std::endl;
    }
    lock.unlock();
    return ss.str();
}

void ArticleManager::start() {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    if (m_timer) {
        return;
    }
    m_timer = chen::IOManager::GetThis()->addTimer(60 * 1000
            ,std::bind(&ArticleManager::onTimer, this), true);
    m_updateTimer = chen::IOManager::GetThis()->addTimer(60 * 1000
            ,std::bind(&ArticleManager::onUpdateTimer, this), true);
}

void ArticleManager::stop() {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    if (!m_timer) {
        return;
    }
    m_timer->cancel();
    m_timer = nullptr;

    m_updateTimer->cancel();
    m_updateTimer = nullptr;
}

bool ArticleManager::incViews(uint64_t id, const std::string& cookie_id, uint64_t user_id) {
    auto info = get(id);
    if (!info) {
        return false;
    }
    bool v = addViews(id, cookie_id);
    if (v) {
        info->setViews(info->getViews() + 1);
        addUpdate(id);
        // Redis: cumulative PV (never expires)
        chen::RedisUtil::Cmd("blog", "incr blog:total_visits");
        // Redis: today PV (expires at midnight)
        auto rpy = chen::RedisUtil::Cmd("blog", "incr blog:today_visits");
        if (rpy && rpy->integer == 1) {
            int64_t now = time(0);
            int64_t tomorrow_midnight = now - (now % 86400) + 86400;
            chen::RedisUtil::Cmd("blog", "expireat blog:today_visits %lld", tomorrow_midnight);
        }
        // Redis: unique visitor tracking via HyperLogLog
        chen::RedisUtil::Cmd("blog", "pfadd blog:visitors %lld", user_id);
    }
    return true;
}

bool ArticleManager::incPraise(uint64_t id, const std::string& cookie_id, uint64_t user_id) {
    auto info = get(id);
    if (!info) {
        return false;
    }
    auto rpy = chen::RedisUtil::Cmd("blog", "hexist pra_a2u:%lld %lld", id, user_id);
    if (!rpy) {
        ERROR(logger) << "hexists fail";
        return false;
    }
    if (rpy->integer == 1) {
        return true;
    }
    rpy = chen::RedisUtil::Cmd("blog", "hset pra_a2u:%lld %lld %lld", id, user_id, time(0));
    if (!rpy) {
        ERROR(logger) << "hset fail";
        return false;
    }
    rpy = chen::RedisUtil::Cmd("blog", "hset pra_u2a:%lld %lld %lld", user_id, id, time(0));
    if (!rpy) {
        ERROR(logger) << "hset fail";
        return false;
    }
    info->setPraise(info->getPraise() + 1);
    addUpdate(id);

    return true;
}

bool ArticleManager::incFavorites(uint64_t id, const std::string& cookie_id, uint64_t user_id) {
    auto info = get(id);
    if (!info) {
        return false;
    }
    auto rpy = chen::RedisUtil::Cmd("blog", "hexist fav_a2u:%lld %lld", id, user_id);
    if (!rpy) {
        ERROR(logger) << "hexists fail";
        return false;
    }
    if (rpy->integer == 1) {
        return true;
    }
    rpy = chen::RedisUtil::Cmd("blog", "hset fav_a2u:%lld %lld %lld", id, user_id, time(0));
    if (!rpy) {
        ERROR(logger) << "hset fail";
        return false;
    }
    rpy = chen::RedisUtil::Cmd("blog", "hset fav_u2a:%lld %lld %lld", user_id, id, time(0));
    if (!rpy) {
        ERROR(logger) << "hset fail";
        return false;
    }
    info->setFavorites(info->getFavorites() + 1);
    addUpdate(id);
    
    return true;
}

bool ArticleManager::decPraise(uint64_t id, const std::string& cookie_id, uint64_t user_id) {
    auto info = get(id);
    if (!info) {
        return false;
    }
    bool v = false;
    auto rpy = chen::RedisUtil::Cmd("blog", "hdel pra_a2u:%lld %lld", id, user_id);
    if (!rpy) {
        ERROR(logger) << "hdel fail";
        return false;
    }
    if (rpy->integer == 1) {
        v = true;
    }
    rpy = chen::RedisUtil::Cmd("blog", "hdel pra_u2a:%lld %lld", user_id, id);
    if (!rpy) {
        ERROR(logger) << "hdel fail";
        return false;
    }
    if (rpy->integer == 1) {
        v = true;
    }
    if (v) {
        info->setPraise(info->getPraise() - 1);
        addUpdate(id);
    }

    return true;
}

bool ArticleManager::decFavorites(uint64_t id, const std::string& cookie_id, uint64_t user_id) {
    auto info = get(id);
    if (!info) {
        return false;
    }
    bool v = false;
    auto rpy = chen::RedisUtil::Cmd("blog", "hdel fav_a2u:%lld %lld", id, user_id);
    if (!rpy) {
        ERROR(logger) << "hdel fail";
        return false;
    }
    if (rpy->integer == 1) {
        v = true;
    }
    rpy = chen::RedisUtil::Cmd("blog", "hset fav_u2a:%lld %lld", user_id, id);
    if (!rpy) {
        ERROR(logger) << "hset fail";
        return false;
    }
    if (rpy->integer == 1) {
        v = true;
    }
    if (v) {
        info->setFavorites(info->getFavorites() - 1);
        addUpdate(id);
    }

    return true;
}

bool ArticleManager::listUserFav(int64_t id, std::map<int64_t, int64_t>& articles) {
#define PROC(id, mask, articles)                               \
    auto rpy = chen::RedisUtil::Cmd("blog", mask, id);        \
    if (!rpy) {                                                \
        ERROR(logger) << "hgetall fail";                       \
        return false;                                          \
    }                                                          \
    for (size_t i = 0; i < rpy->elements; i += 2) {            \
        articles[chen::TypeUtil::Atoi(rpy->element[i]->str)]  \
            = chen::TypeUtil::Atoi(rpy->element[i + 1]->str); \
    }                                                          \
    return true;
    
    PROC(id, "hgetall fav_u2a:%lld", articles);
}

bool ArticleManager::listUserPra(int64_t id, std::map<int64_t, int64_t>& articles) {
    PROC(id, "hgetall pra_u2a:%lld", articles);
}

bool ArticleManager::listArticleFav(int64_t id, std::map<int64_t, int64_t>& users) {
    PROC(id, "hgetall fav_a2u:%lld", users);
}

bool ArticleManager::listArticlePra(int64_t id, std::map<int64_t, int64_t>& users) {
    PROC(id, "hgetall pra_a2u:%lld", users);
}
#undef PROC

void ArticleManager::onTimer() {
    time_t now = time(0);
    std::vector<data::ArticleInfo::ptr> infos;
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    for (auto& i : m_datas) {
        if (i.second->getState() != ArticleManager::Status::PUBLISHED) {
            continue;
        }
        if (i.second->getPublishTime() < now) {
            i.second->setState(ArticleManager::Status::PUBLISHED);
            i.second->setUpdateTime(now);
            infos.push_back(i.second);
        }
    }
    lock.unlock();

    if (infos.empty()) {
        return;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "getDB error";
        return;
    }
    for (auto& i : infos) {
        if (data::ArticleInfoDao::Update(i, db)) {
            ERROR(logger) << "Update error errno=" << errno
                << db->getErrno() << " errstr=" << db->getErrStr()
                << " data=" << i->toJsonString();
        }
    }
}

void ArticleManager::onUpdateTimer() {
    std::set<int64_t> updates;
    {
        std::unique_lock<std::shared_mutex> lock(m_viewsMutex);
        updates.swap(m_updates);
    }

    if (updates.empty()) {
        return;
    }
    auto conn = GetDB();
    if (!conn) {
        ERROR(logger) << "get db connect fail";

        std::unique_lock<std::shared_mutex> lock(m_viewsMutex);
        for (auto& i : updates) {
            m_updates.insert(i);
        }
        return;
    }
    for (auto& i : updates) {
        auto info = get(i);
        if (info) {
            if (data::ArticleInfoDao::Update(info, conn)) {
                addUpdate(i);
            }
        }
    }
}

bool ArticleManager::addViews(uint64_t id, const std::string& cookie_id) {
    time_t now = time(0);
    std::shared_lock<std::shared_mutex> lock(m_viewsMutex);
    auto it = m_viewsCache.find(id);
    if (it != m_viewsCache.end()) {
        auto iit = it->second.find(cookie_id);
        if (iit != it->second.end() && (now - iit->second) < 10 * 60) {
            return false;
        }
    }
    lock.unlock();

    std::unique_lock<std::shared_mutex> lock2(m_viewsMutex);
    m_viewsCache[id][cookie_id] = now;
    return true;
}

void ArticleManager::addUpdate(int64_t id) {
    std::unique_lock<std::shared_mutex> lock(m_viewsMutex);
    m_updates.insert(id);
}

void ArticleManager::incPraiseCount(int64_t id) {
    auto info = get(id);
    if (!info) {
        return;
    }
    info->setPraise(info->getPraise() + 1);
    addUpdate(id);
}

void ArticleManager::decPraiseCount(int64_t id) {
    auto info = get(id);
    if (!info) {
        return;
    }
    if (info->getPraise() > 0) {
        info->setPraise(info->getPraise() - 1);
        addUpdate(id);
    }
}

int64_t ArticleManager::getTodayViews() {
    auto rpy = chen::RedisUtil::Cmd("blog", "get blog:today_visits");
    if (rpy && rpy->str) {
        return chen::TypeUtil::Atoi(rpy->str);
    }
    return 0;
}

int64_t ArticleManager::getTotalViews() {
    auto rpy = chen::RedisUtil::Cmd("blog", "get blog:total_visits");
    if (rpy && rpy->str) {
        return chen::TypeUtil::Atoi(rpy->str);
    }
    // Redis key not yet seeded — compute from articles and initialise
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    int64_t total = 0;
    for (auto& i : m_datas) {
        if (!i.second->getIsDeleted()) {
            total += i.second->getViews();
        }
    }
    lock.unlock();
    chen::RedisUtil::Cmd("blog", "set blog:total_visits %lld", total);
    return total;
}

int64_t ArticleManager::getTotalVisitors() {
    auto rpy = chen::RedisUtil::Cmd("blog", "pfcount blog:visitors");
    if (rpy) {
        return rpy->integer;
    }
    return 0;
}

}