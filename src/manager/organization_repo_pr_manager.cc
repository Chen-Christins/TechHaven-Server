/**
 * @file organization_repo_pr_manager.cc
 * @brief 组织仓库 PR 管理器实现
 * @author Christins
 * @date 2026-06-24
 * @copyright Apache 2.0
 */
#include "organization_repo_pr_manager.h"
#include "cache_util.h"
#include "organization_repo_manager.h"
#include "organization_user_rel_manager.h"
#include <chen/log/log.h>
#include <chen/worker.h>
#include <chen/http/http_connection.h>
#include <chen/http/uri.h>
#include <json/json.h>
#include <json/reader.h>
#include <json/writer.h>
#include "../util.h"
#include "protocol_ss_github.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

static const size_t kCacheMaxSize = 1000;

OrganizationRepoPrManager::OrganizationRepoPrManager()
    :m_cache(8, kCacheMaxSize, 0) {
}

data::OrganizationRepoPrsInfo::ptr OrganizationRepoPrManager::parseRow(chen::ISQLData::ptr rt) {
    data::OrganizationRepoPrsInfo::ptr v(new data::OrganizationRepoPrsInfo);
    v->setId(rt->getInt64(0));
    v->setRepoId(rt->getInt64(1));
    v->setGithubPrId(rt->getInt32(2));
    v->setTitle(rt->getString(3));
    v->setDescription(rt->getString(4));
    v->setState(rt->getString(5));
    v->setPriority(rt->getString(6));
    v->setAuthor(rt->getString(7));
    v->setHeadBranch(rt->getString(8));
    v->setBaseBranch(rt->getString(9));
    v->setCommitSha(rt->getString(10));
    v->setChangedFiles(rt->getInt32(11));
    v->setAdditions(rt->getInt32(12));
    v->setDeletions(rt->getInt32(13));
    v->setReviewers(rt->getString(14));
    v->setReviewStatus(rt->getString(15));
    v->setClosedAt(rt->getTime(16));
    v->setMergedAt(rt->getTime(17));
    v->setCreateTime(rt->getTime(18));
    v->setUpdateTime(rt->getTime(19));
    return v;
}

void OrganizationRepoPrManager::add(data::OrganizationRepoPrsInfo::ptr info) {
    m_cache.set(info->getId(), info);
}

void OrganizationRepoPrManager::del(int64_t id) {
    m_cache.del(id);
}

data::OrganizationRepoPrsInfo::ptr OrganizationRepoPrManager::get(int64_t id) {
    auto v = m_cache.get(id);
    if (v) {
        return v;
    }
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    v = data::OrganizationRepoPrsInfoDao::Query(id, db);
    if (v) {
        m_cache.set(id, v);
    }
    return v;
}

data::OrganizationRepoPrsInfo::ptr OrganizationRepoPrManager::getByRepoAndPrId(int64_t repo_id, int32_t github_pr_id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return nullptr;
    }
    return data::OrganizationRepoPrsInfoDao::QueryByRepoIdGithubPrId(repo_id, github_pr_id, db);
}

int64_t OrganizationRepoPrManager::listByRepoPages(std::vector<data::OrganizationRepoPrsInfo::ptr>& prs
        , int64_t repo_id, const std::string& state, uint64_t offset, uint64_t limit) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }

    auto qb = chen::QueryBuilder::Create("organization_repo_prs");
    qb->where("repo_id", "=", repo_id);
    qb->whereIf(!state.empty(), "state", "=", state);
    qb->orderBy("create_time", "DESC");

    int64_t total = executeCountCached(qb, db, "org_pr:list:" + std::to_string(repo_id) + ":" + state);
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
        prs.push_back(info);
        if (!m_cache.exists(info->getId())) {
            m_cache.set(info->getId(), info);
        }
    }
    return total;
}

int64_t OrganizationRepoPrManager::getCountByRepo(int64_t repo_id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }
    auto qb = chen::QueryBuilder::Create("organization_repo_prs");
    qb->where("repo_id", "=", repo_id);
    int64_t total = 0;
    if (qb->executeCount(total, db)) {
        return 0;
    }
    return total;
}

int64_t OrganizationRepoPrManager::listByOrgPages(std::vector<data::OrganizationRepoPrsInfo::ptr>& prs
        , int64_t org_id, const std::string& state, uint64_t offset, uint64_t limit) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }

    auto qb = chen::QueryBuilder::Create("organization_repo_prs prs");
    qb->select("prs.*");
    qb->join("organization_repos repos", "prs.repo_id = repos.id");
    qb->where("repos.org_id", "=", org_id);
    qb->whereIf(!state.empty(), "prs.state", "=", state);
    qb->orderBy("prs.create_time", "DESC");

    std::string cache_key = "org_pr:org:" + std::to_string(org_id) + ":" + state;
    int64_t total = executeCountCached(qb, db, cache_key);
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
        prs.push_back(info);
        if (!m_cache.exists(info->getId())) {
            m_cache.set(info->getId(), info);
        }
    }
    return total;
}

int64_t OrganizationRepoPrManager::listByUserPages(std::vector<data::OrganizationRepoPrsInfo::ptr>& prs
        , int64_t uid, const std::string& state, uint64_t offset, uint64_t limit) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return 0;
    }

    auto qb = chen::QueryBuilder::Create("organization_repo_prs prs");
    qb->select("prs.*");
    qb->join("organization_repos repos", "prs.repo_id = repos.id");
    qb->join("INNER", "organization_user_rel rel", "repos.org_id = rel.org_id");
    qb->where("rel.user_id", "=", uid);
    qb->where("rel.status", "=", (int64_t)OrganizationUserRelManager::Status::APPROVED);
    qb->where("rel.is_deleted", "=", (int64_t)0);
    qb->whereIf(!state.empty(), "prs.state", "=", state);
    qb->orderBy("prs.create_time", "DESC");

    int64_t total = executeCountCached(qb, db, "org_pr:user:" + std::to_string(uid) + ":" + state);
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
        prs.push_back(info);
        if (!m_cache.exists(info->getId())) {
            m_cache.set(info->getId(), info);
        }
    }
    return total;
}

/// 从 GitHub URL 提取 owner/repo
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

/// 从 Link header 中提取 rel="next" 的 URL
static std::string getNextPageUrl(const std::string& linkHeader) {
    // 格式: <https://...>; rel="next", <https://...>; rel="last"
    size_t pos = 0;
    while (pos < linkHeader.size()) {
        auto start = linkHeader.find('<', pos);
        if (start == std::string::npos) break;
        auto end = linkHeader.find('>', start);
        if (end == std::string::npos) break;
        std::string url = linkHeader.substr(start + 1, end - start - 1);
        auto relPos = linkHeader.find("rel=\"", end);
        if (relPos != std::string::npos) {
            auto quoteEnd = linkHeader.find('"', relPos + 5);
            std::string rel = linkHeader.substr(relPos + 5, quoteEnd - relPos - 5);
            if (rel == "next") {
                return url;
            }
        }
        pos = end + 1;
    }
    return "";
}

struct SyncPrItem {
    int number;
    std::string title;
    std::string description;
    std::string state;
    std::string author;
    std::string head_branch;
    std::string base_branch;
    std::string commit_sha;
    bool merged;
    int64_t created_at;
    int64_t closed_at;
    int64_t merged_at;
    std::string review_status;
    std::string reviewers;
};

static int syncPage(const Json::Value& arr, int64_t repo_id, const std::string& repo_path
        , const std::map<std::string, std::string>& headers, chen::IDB::ptr db) {
    // 先解析列表数据
    std::vector<SyncPrItem> items;
    for (auto& item : arr) {
        SyncPrItem si;
        si.number = item["number"].asInt();
        si.title = item["title"].asString();
        si.description = item["body"].asString();
        si.state = item["state"].asString();
        si.author = item["user"]["login"].asString();
        si.head_branch = item["head"]["ref"].asString();
        si.base_branch = item["base"]["ref"].asString();
        si.commit_sha = item["head"]["sha"].asString();
        si.merged = item["merged"].asBool();
        si.created_at = 0;
        si.closed_at = 0;
        si.review_status = "pending";
        si.reviewers = "[]";
        si.merged_at = 0;
        if (item.isMember("created_at") && !item["created_at"].isNull()) {
            std::string ts = item["created_at"].asString();
            // GitHub ISO 8601: "2024-06-25T10:30:00Z" → "2024-06-25 10:30:00"
            for (auto& c : ts) if (c == 'T') c = ' ';
            if (!ts.empty() && ts.back() == 'Z') ts.pop_back();
            si.created_at = chen::Str2Time(ts.c_str());
        }
        if (item.isMember("closed_at") && !item["closed_at"].isNull()) {
            std::string ts = item["closed_at"].asString();
            for (auto& c : ts) if (c == 'T') c = ' ';
            if (!ts.empty() && ts.back() == 'Z') ts.pop_back();
            si.closed_at = chen::Str2Time(ts.c_str());
        }
        if (item.isMember("merged_at") && !item["merged_at"].isNull()) {
            std::string ts = item["merged_at"].asString();
            for (auto& c : ts) if (c == 'T') c = ' ';
            if (!ts.empty() && ts.back() == 'Z') ts.pop_back();
            si.merged_at = chen::Str2Time(ts.c_str());
        }
        items.push_back(si);
    }

    // 并行拉详情 + 审查数据
    std::map<int, int> additions, deletions, changed_files;
    std::map<int, std::string> review_statuses, reviewers_json;
    {
        auto wg = chen::WorkerGroup::Create(5);
        for (auto& si : items) {
            int num = si.number;
            wg->schedule([repo_path, &headers, num, &additions, &deletions, &changed_files
                    , &review_statuses, &reviewers_json]() {
                // 1. 拉详情补全 additions/deletions/changed_files
                {
                    std::string detail_url = "https://api.github.com/repos/" + repo_path + "/pulls/" + std::to_string(num);
                    chen::http::HttpResult::ptr detail;
                    for (int retry = 0; retry <= 3; ++retry) {
                        detail = chen::http::HttpConnection::DoGet(detail_url, 15000, headers);
                        if (detail && detail->result == 0 && detail->response
                                && detail->response->getStatus() == chen::http::HttpStatus::OK) {
                            break;
                        }
                    }
                    if (detail && detail->result == 0 && detail->response
                            && detail->response->getStatus() == chen::http::HttpStatus::OK) {
                        Json::Value detailJson;
                        Json::Reader reader;
                        if (reader.parse(detail->response->getBody(), detailJson)) {
                            if (detailJson.isMember("additions")) additions[num] = detailJson["additions"].asInt();
                            if (detailJson.isMember("deletions")) deletions[num] = detailJson["deletions"].asInt();
                            if (detailJson.isMember("changed_files")) changed_files[num] = detailJson["changed_files"].asInt();
                        }
                    }
                }
                // 2. 拉审查数据
                {
                    std::string reviews_url = "https://api.github.com/repos/" + repo_path + "/pulls/" + std::to_string(num) + "/reviews";
                    chen::http::HttpResult::ptr reviews;
                    for (int retry = 0; retry <= 3; ++retry) {
                        reviews = chen::http::HttpConnection::DoGet(reviews_url, 15000, headers);
                        if (reviews && reviews->result == 0 && reviews->response
                                && reviews->response->getStatus() == chen::http::HttpStatus::OK) {
                            break;
                        }
                    }
                    if (reviews && reviews->result == 0 && reviews->response
                            && reviews->response->getStatus() == chen::http::HttpStatus::OK) {
                        Json::Value arr;
                        Json::Reader reader;
                        if (reader.parse(reviews->response->getBody(), arr) && arr.isArray()) {
                            bool has_approved = false;
                            bool has_changes = false;
                            Json::Value reviewerArr(Json::arrayValue);
                            for (auto& rv : arr) {
                                std::string state = rv["state"].asString();
                                if (state == "APPROVED") has_approved = true;
                                else if (state == "CHANGES_REQUESTED") has_changes = true;
                                Json::Value reviewer;
                                reviewer["reviewer"] = rv["user"]["login"].asString();
                                reviewer["status"] = state;
                                reviewerArr.append(reviewer);
                            }
                            if (has_approved) review_statuses[num] = "approved";
                            else if (has_changes) review_statuses[num] = "changes_requested";
                            else review_statuses[num] = "pending";
                            reviewers_json[num] = chen::JsonUtil::ToString(reviewerArr);
                        }
                    }
                }
            });
        }
        wg->waitAll();
    }

    // 写入 DB
    int synced = 0;
    for (auto& si : items) {
        auto existing = OrganizationRepoPrMgr::GetInstance()->getByRepoAndPrId(repo_id, si.number);
        data::OrganizationRepoPrsInfo::ptr info;
        if (existing) {
            info = existing;
        } else {
            info.reset(new data::OrganizationRepoPrsInfo);
            info->setRepoId(repo_id);
            info->setGithubPrId(si.number);
        }
        if (si.created_at) {
            info->setCreateTime(si.created_at);
        }

        info->setTitle(si.title);
        info->setDescription(si.description);
        if (si.merged) {
            info->setState("merged");
        } else {
            info->setState(si.state);
        }
        info->setAuthor(si.author);
        info->setHeadBranch(si.head_branch);
        info->setBaseBranch(si.base_branch);
        info->setCommitSha(si.commit_sha);

        auto it_a = additions.find(si.number);
        if (it_a != additions.end()) info->setAdditions(it_a->second);
        auto it_d = deletions.find(si.number);
        if (it_d != deletions.end()) info->setDeletions(it_d->second);
        auto it_c = changed_files.find(si.number);
        if (it_c != changed_files.end()) info->setChangedFiles(it_c->second);

        {
            auto it = review_statuses.find(si.number);
            info->setReviewStatus(it != review_statuses.end() ? it->second : "pending");
        }
        {
            auto it = reviewers_json.find(si.number);
            info->setReviewers(it != reviewers_json.end() ? it->second : "[]");
        }
        info->setClosedAt(si.closed_at);
        info->setMergedAt(si.merged_at);
        info->setUpdateTime(time(0));

        if (existing) {
            if (data::OrganizationRepoPrsInfoDao::Update(info, db)) {
                ERROR(logger) << "SyncPrFromGitHub: Update failed for PR #" << info->getGithubPrId();
                continue;
            }
        } else {
            if (data::OrganizationRepoPrsInfoDao::Insert(info, db)) {
                ERROR(logger) << "SyncPrFromGitHub: Insert failed for PR #" << info->getGithubPrId();
                continue;
            }
            OrganizationRepoPrMgr::GetInstance()->add(info);
        }
        ++synced;
    }
    return synced;
}

void OrganizationRepoPrManager::SyncFromGitHub(int64_t repo_id, const std::string& repo_url, const std::string& token) {
    std::string repo_path = extractGithubRepo(repo_url);
    if (repo_path.empty()) {
        ERROR(logger) << "SyncPrFromGitHub: invalid repo url=" << repo_url;
        return;
    }

    std::map<std::string, std::string> headers;
    headers["Accept"] = "application/vnd.github.v3+json";
    headers["Accept-Encoding"] = "identity";
    headers["User-Agent"] = "Blog-Server/1.0";
    if (!token.empty()) {
        headers["Authorization"] = "Bearer " + token;
    }

    {
        auto repo = OrganizationRepoMgr::GetInstance()->get(repo_id);
        if (repo) {
            auto db2 = GetDB();
            if (db2) {
                repo->setPrSyncStatus("syncing");
                repo->setGithubFullName(repo_path);
                data::OrganizationReposInfoDao::Update(repo, db2);
            }
        }
    }

    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "SyncPrFromGitHub: GetDB failed";
        return;
    }

    const int kMaxPages = 10;
    int page_no = 0;
    int total_synced = 0;
    std::string api_url = "https://api.github.com/repos/" + repo_path + "/pulls?state=all&per_page=50";

    auto doGetWithRetry = [&](const std::string& url, int max_retries) -> chen::http::HttpResult::ptr {
        for (int retry = 0; retry <= max_retries; ++retry) {
            auto r = chen::http::HttpConnection::DoGet(url, 30000, headers);
            if (r && r->result == 0 && r->response
                    && r->response->getStatus() == chen::http::HttpStatus::OK) {
                return r;
            }
            if (retry < max_retries) {
                int delay = (2 << retry) * 1000000;  // 2s, 4s, 8s
                WARN(logger) << "SyncPrFromGitHub: retry " << (retry + 1) << "/" << max_retries
                    << " for " << url;
                usleep(delay);
            }
        }
        return nullptr;
    };

    while (!api_url.empty() && page_no < kMaxPages) {
        ++page_no;
        INFO(logger) << "SyncPrFromGitHub: page " << page_no << " " << api_url;

        auto result = doGetWithRetry(api_url, 3);
        if (!result) {
            ERROR(logger) << "SyncPrFromGitHub: HTTP request failed on page " << page_no;
            break;
        }

        Json::Value arr;
        Json::Reader reader;
        if (!reader.parse(result->response->getBody(), arr) || !arr.isArray()) {
            ERROR(logger) << "SyncPrFromGitHub: parse JSON failed on page " << page_no;
            break;
        }

        total_synced += syncPage(arr, repo_id, repo_path, headers, db);

        // 检查下一页
        std::string link = result->response->getHeader("link");
        if (link.empty()) {
            break;
        }
        api_url = getNextPageUrl(link);
    }

    // 更新仓库的 PR 同步状态
    {
        auto repo = OrganizationRepoMgr::GetInstance()->get(repo_id);
        if (repo) {
            auto repoDb = GetDB();
            if (repoDb) {
                repo->setPrSyncStatus(total_synced >= 0 ? "success" : "failed");
                repo->setPrSyncedAt(time(0));
                data::OrganizationReposInfoDao::Update(repo, repoDb);
            }
        }
    }

    INFO(logger) << "SyncPrFromGitHub: synced " << total_synced << " PRs across "
        << page_no << " pages for repo " << repo_id;
}

// ==================== RPC Webhook Handlers ====================

data::OrganizationReposInfo::ptr OrganizationRepoPrManager::findRepoByOwnerAndName(const std::string& owner, const std::string& name) {
    if (owner.empty() || name.empty()) {
        return nullptr;
    }
    std::string full_name = owner + "/" + name;

    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "findRepoByOwnerAndName: GetDB failed for " << full_name;
        return nullptr;
    }

    // 用 github_full_name 字段精确匹配
    std::string sql = "SELECT id FROM organization_repos WHERE github_full_name = ? LIMIT 1";
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "findRepoByOwnerAndName: prepare failed for " << full_name;
        return nullptr;
    }
    stmt->bindString(1, full_name);
    auto rt = stmt->query();
    if (!rt || !rt->next()) {
        DEBUG(logger) << "findRepoByOwnerAndName: no repo found for " << full_name;
        return nullptr;
    }
    return OrganizationRepoMgr::GetInstance()->get(rt->getInt64(0));
}

int32_t OrganizationRepoPrManager::HandlePRWebhook(const tagGithubPRInfo& info) {
    // 1. 查找本地仓库
    auto repo = findRepoByOwnerAndName(info.RepoOwner, info.RepoName);
    if (!repo) {
        WARN(logger) << "HandlePRWebhook: repo not found for "
            << info.RepoOwner << "/" << info.RepoName
            << ", PR #" << info.Number << " \"" << info.Title << "\"";
        return -1;
    }
    int64_t repo_id = repo->getId();

    // 2. 解析时间
    time_t created_at = chen::Str2Time(info.CreatedAt);
    time_t closed_at  = chen::Str2Time(info.ClosedAt);
    time_t merged_at  = chen::Str2Time(info.MergedAt);

    // 3. 获取 DB 连接
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "HandlePRWebhook: GetDB failed for repo " << repo_id << " PR #" << info.Number;
        return -2;
    }

    // 4. 查找已有记录（幂等去重）
    auto existing = OrganizationRepoPrMgr::GetInstance()->getByRepoAndPrId(repo_id, info.Number);

    data::OrganizationRepoPrsInfo::ptr pr;
    if (existing) {
        pr = existing;
    } else {
        pr.reset(new data::OrganizationRepoPrsInfo);
        pr->setRepoId(repo_id);
        pr->setGithubPrId(info.Number);
        if (created_at) {
            pr->setCreateTime(created_at);
        }
    }

    // 5. 更新字段
    pr->setTitle(info.Title);
    pr->setDescription(info.Body);
    pr->setAuthor(info.Author);
    pr->setHeadBranch(info.HeadBranch);
    pr->setBaseBranch(info.BaseBranch);
    pr->setCommitSha(info.HeadSha);
    pr->setChangedFiles(info.ChangedFiles);
    pr->setAdditions(info.Additions);
    pr->setDeletions(info.Deletions);

    // 状态：merged > open/closed
    if (info.Merged) {
        pr->setState("merged");
    } else {
        pr->setState(info.State);
    }

    // opened/reopened 时重置审查状态
    if (info.Action == "opened" || info.Action == "reopened") {
        pr->setReviewStatus("pending");
        pr->setReviewers("[]");
    }

    // synchronized(push) 时更新时间，但保留 review 状态
    if (info.Action == "synchronize") {
        pr->setCommitSha(info.HeadSha);
    }

    pr->setClosedAt(closed_at);
    pr->setMergedAt(merged_at);
    pr->setUpdateTime(time(0));

    // 6. 写入 DB
    if (existing) {
        if (data::OrganizationRepoPrsInfoDao::Update(pr, db)) {
            ERROR(logger) << "HandlePRWebhook: Update failed for repo " << repo_id << " PR #" << info.Number;
            return -3;
        }
        DEBUG(logger) << "HandlePRWebhook: updated PR #" << info.Number
            << " state=" << pr->getState() << " action=" << info.Action;
    } else {
        if (data::OrganizationRepoPrsInfoDao::Insert(pr, db)) {
            ERROR(logger) << "HandlePRWebhook: Insert failed for repo " << repo_id << " PR #" << info.Number;
            return -4;
        }
        OrganizationRepoPrMgr::GetInstance()->add(pr);
        DEBUG(logger) << "HandlePRWebhook: inserted PR #" << info.Number
            << " state=" << pr->getState();
    }

    return 0;
}

int32_t OrganizationRepoPrManager::HandlePRReviewWebhook(const tagGithubPRReviewInfo& info) {
    // 1. 查找本地仓库
    auto repo = findRepoByOwnerAndName(info.RepoOwner, info.RepoName);
    if (!repo) {
        WARN(logger) << "HandlePRReviewWebhook: repo not found for "
            << info.RepoOwner << "/" << info.RepoName
            << ", PR #" << info.PRNumber;
        return -1;
    }
    int64_t repo_id = repo->getId();

    // 2. 查找 PR
    auto pr = OrganizationRepoPrMgr::GetInstance()->getByRepoAndPrId(repo_id, info.PRNumber);
    if (!pr) {
        WARN(logger) << "HandlePRReviewWebhook: PR #" << info.PRNumber
            << " not found in repo " << repo_id
            << " (" << info.RepoOwner << "/" << info.RepoName << ")";
        return -2;
    }

    // 3. 获取 DB
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "HandlePRReviewWebhook: GetDB failed for repo " << repo_id << " PR #" << info.PRNumber;
        return -3;
    }

    // 4. 更新审查状态
    // 规则：一旦 approved 不再降级为 changes_requested（除非 dismissed）
    std::string current_status = pr->getReviewStatus();
    if (info.Action == "dismissed") {
        // 重新计算：保留其他 reviewer 的状态
        // 简化处理：设为 pending，后面的 reviewers 重建时会重新计算
        pr->setReviewStatus("pending");
    } else if (info.State == "approved") {
        pr->setReviewStatus("approved");
    } else if (info.State == "changes_requested") {
        if (current_status != "approved") {
            pr->setReviewStatus("changes_requested");
        }
    }
    // "commented" 类型的 review 不影响整体状态

    // 5. 更新审查者列表
    {
        Json::Value reviewerArr(Json::arrayValue);
        Json::Reader reader;

        if (!pr->getReviewers().empty() && reader.parse(pr->getReviewers(), reviewerArr)
                && reviewerArr.isArray()) {
            // 查找该 reviewer 是否已存在
            bool found = false;
            for (auto& r : reviewerArr) {
                if (r.isObject() && r["reviewer"].asString() == info.Reviewer) {
                    r["status"] = info.State;
                    if (!info.SubmittedAt.empty()) {
                        r["submitted_at"] = info.SubmittedAt;
                    }
                    found = true;
                    break;
                }
            }
            if (!found) {
                Json::Value entry;
                entry["reviewer"] = info.Reviewer;
                entry["status"] = info.State;
                if (!info.SubmittedAt.empty()) {
                    entry["submitted_at"] = info.SubmittedAt;
                }
                reviewerArr.append(entry);
            }
        } else {
            Json::Value entry;
            entry["reviewer"] = info.Reviewer;
            entry["status"] = info.State;
            if (!info.SubmittedAt.empty()) {
                entry["submitted_at"] = info.SubmittedAt;
            }
            reviewerArr.append(entry);
        }

        Json::FastWriter writer;
        pr->setReviewers(writer.write(reviewerArr));
    }

    pr->setUpdateTime(time(0));

    // 6. 写入 DB
    if (data::OrganizationRepoPrsInfoDao::Update(pr, db)) {
        ERROR(logger) << "HandlePRReviewWebhook: Update failed for repo " << repo_id << " PR #" << info.PRNumber;
        return -4;
    }

    DEBUG(logger) << "HandlePRReviewWebhook: updated review for PR #" << info.PRNumber
        << " reviewer=" << info.Reviewer << " state=" << info.State;
    return 0;
}

}
