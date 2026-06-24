#include "organization_repo_manager.h"
#include "cache_util.h"
#include <chen/log/log.h>
#include <chen/db/redis.h>
#include <chen/http/http_connection.h>
#include <chen/http/uri.h>
#include <json/json.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 500;

OrganizationRepoManager::OrganizationRepoManager()
    :m_cache(4, kCacheMaxSize, 30) {
}

data::OrganizationReposInfo::ptr OrganizationRepoManager::parseRow(chen::ISQLData::ptr rt) {
    data::OrganizationReposInfo::ptr v(new data::OrganizationReposInfo);
    v->setId(rt->getInt64(0));
    v->setOrgId(rt->getInt64(1));
    v->setName(rt->getString(2));
    v->setDescription(rt->getString(3));
    v->setUrl(rt->getString(4));
    v->setLanguage(rt->getString(5));
    v->setStarsCount(rt->getInt32(6));
    v->setSortOrder(rt->getInt32(7));
    v->setCreateTime(rt->getTime(8));
    v->setUpdateTime(rt->getTime(9));
    v->setToken(rt->getString(10));
    v->setSyncStatus(rt->getString(11));
    return v;
}

void OrganizationRepoManager::add(data::OrganizationReposInfo::ptr info) {
    m_cache.set(info->getId(), info);
    invalidateCountCache(info->getOrgId());
}

void OrganizationRepoManager::del(int64_t id) {
    auto info = m_cache.get(id);
    if (info) {
        invalidateCountCache(info->getOrgId());
    }
    m_cache.del(id);
}

data::OrganizationReposInfo::ptr OrganizationRepoManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::OrganizationReposInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

data::OrganizationReposInfo::ptr OrganizationRepoManager::getByOrgAndName(int64_t org_id, const std::string& name) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    auto info = data::OrganizationReposInfoDao::QueryByOrgIdName(org_id, name, db);
    if (info) {
        m_cache.set(info->getId(), info);
    }
    return info;
}

int64_t OrganizationRepoManager::listByOrgPages(std::vector<data::OrganizationReposInfo::ptr>& repos
        , int64_t org_id, uint64_t offset, uint64_t limit) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }

    auto qb = chen::QueryBuilder::Create("organization_repos");
    qb->where("org_id", "=", org_id);
    qb->orderBy("sort_order DESC, id", "DESC");

    std::stringstream ck;
    ck << "org_repo:list:" << org_id;
    int64_t total = executeCountCached(qb, db, ck.str());
    if (total == 0) {
        return 0;
    }

    if (limit < (uint64_t)INT32_MAX) {
        qb->limit((int32_t)limit);
        qb->offset((int32_t)offset);
    }
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
        auto info = parseRow(rt);
        repos.push_back(info);
        if (!m_cache.exists(info->getId())) {
            m_cache.set(info->getId(), info);
        }
    }
    return total;
}

int64_t OrganizationRepoManager::getCountByOrg(int64_t org_id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }

    auto qb = chen::QueryBuilder::Create("organization_repos");
    qb->where("org_id", "=", org_id);

    std::stringstream ck;
    ck << "org_repo:count:" << org_id;
    return executeCountCached(qb, db, ck.str());
}

void OrganizationRepoManager::getAllWithToken(std::vector<data::OrganizationReposInfo::ptr>& repos) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return;
    }
    auto qb = chen::QueryBuilder::Create("organization_repos");
    qb->where("token", "!=", "");
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
        repos.push_back(parseRow(rt));
    }
}

void OrganizationRepoManager::invalidateCountCache(int64_t org_id) {
    {
        std::stringstream key;
        key << "cache:count:org_repo:count:" << org_id;
        chen::RedisUtil::Cmd("blog", "del %s", key.str().c_str());
    }
    {
        std::stringstream key;
        key << "cache:count:org_repo:list:" << org_id;
        chen::RedisUtil::Cmd("blog", "del %s", key.str().c_str());
    }
}

static std::string extractGithubRepo(const std::string& url) {
    auto uri = chen::Uri::Create(url);
    if (!uri) {
        return "";
    }
    std::string path = uri->getPath();
    if (!path.empty() && path[0] == '/') {
        path = path.substr(1);
    }
    if (path.size() > 4 && path.substr(path.size() - 4) == ".git") {
        path = path.substr(0, path.size() - 4);
    }
    return path;
}

static void markSyncFailed(int64_t repo_id) {
    auto repo = OrganizationRepoMgr::GetInstance()->get(repo_id);
    if (!repo) {
        return;
    }
    auto db = GetDB();
    if (!db) {
        return;
    }
    repo->setSyncStatus("failed");
    repo->setUpdateTime(time(0));
    data::OrganizationReposInfoDao::Update(repo, db);
}

void OrganizationRepoManager::SyncFromGitHub(int64_t repo_id, const std::string& url, const std::string& token) {
    std::string repo_path = extractGithubRepo(url);
    if (repo_path.empty()) {
        ERROR(logger) << "SyncFromGitHub: invalid repo url=" << url;
        markSyncFailed(repo_id);
        return;
    }

    std::string api_url = "https://api.github.com/repos/" + repo_path;
    std::map<std::string, std::string> headers;
    headers["Accept"] = "application/vnd.github.v3+json";
    headers["Accept-Encoding"] = "identity";
    headers["User-Agent"] = "Blog-Server/1.0";
    if (!token.empty()) {
        headers["Authorization"] = "Bearer " + token;
    }

    INFO(logger) << "SyncFromGitHub: calling " << api_url;
    auto result = chen::http::HttpConnection::DoGet(api_url, 15000, headers);
    if (!result || result->result != 0 || !result->response
            || result->response->getStatus() != chen::http::HttpStatus::OK) {
        ERROR(logger) << "SyncFromGitHub: HTTP request failed"
            << " result=" << (result ? result->result : -1)
            << " status=" << (result && result->response ? (int)result->response->getStatus() : 0)
            << " body=" << (result && result->response ? result->response->getBody() : "");
        markSyncFailed(repo_id);
        return;
    }

    Json::Value json;
    Json::Reader reader;
    if (!reader.parse(result->response->getBody(), json)) {
        ERROR(logger) << "SyncFromGitHub: parse JSON failed";
        markSyncFailed(repo_id);
        return;
    }

    auto repo = OrganizationRepoMgr::GetInstance()->get(repo_id);
    if (!repo) {
        ERROR(logger) << "SyncFromGitHub: repo " << repo_id << " not found";
        return;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "SyncFromGitHub: GetDB failed";
        return;
    }

    if (json.isMember("stargazers_count")) {
        repo->setStarsCount(json["stargazers_count"].asInt());
    }
    if (json.isMember("language") && !json["language"].isNull()) {
        repo->setLanguage(json["language"].asString());
    }
    if (json.isMember("description") && !json["description"].isNull()) {
        repo->setDescription(json["description"].asString());
    }
    repo->setSyncStatus("success");
    repo->setUpdateTime(time(0));

    if (data::OrganizationReposInfoDao::Update(repo, db)) {
        ERROR(logger) << "SyncFromGitHub: DB update failed";
        repo->setSyncStatus("failed");
        data::OrganizationReposInfoDao::Update(repo, db);
        return;
    }

    INFO(logger) << "SyncFromGitHub: success for repo " << repo_id;
}

}
