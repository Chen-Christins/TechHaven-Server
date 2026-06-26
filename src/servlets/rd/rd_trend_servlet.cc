/**
 * @file rd_trend_servlet.cc
 * @brief 研发平台趋势分析接口实现
 * @author Christins
 * @date 2026-06-25
 * @copyright Apache 2.0
 */
#include "rd_trend_servlet.h"
#include <chen/log/log.h>
#include <cmath>
#include "../../manager/user_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/organization_manager.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

RdTrendServlet::RdTrendServlet()
    : BlogLoginedServlet("RdTrendServlet") {
}

// ==================== 静态工具函数 ====================

time_t RdTrendServlet::parseDate(const std::string& date_str) {
    return chen::Str2Time(date_str.c_str(), "%Y-%m-%d");
}

std::string RdTrendServlet::formatDate(time_t t) {
    return chen::Time2Str(t, "%Y-%m-%d");
}

std::string RdTrendServlet::formatGroupKey(time_t t, const std::string& granularity) {
    struct tm tm;
    localtime_r(&t, &tm);
    char buf[32];
    if (granularity == "month") {
        snprintf(buf, sizeof(buf), "%04d-%02d", tm.tm_year + 1900, tm.tm_mon + 1);
    } else if (granularity == "week") {
        int wday = tm.tm_wday;
        int days_to_monday = (wday == 0 ? 6 : wday - 1);
        time_t monday = t - days_to_monday * 86400;
        return formatDate(monday);
    } else {
        return formatDate(t);
    }
    return buf;
}

// ==================== 查询辅助函数 ====================

static std::string buildOrgFilter(const std::vector<int64_t>& target_orgs) {
    if (target_orgs.empty()) return "";
    std::string cond = " org_id IN (";
    for (size_t i = 0; i < target_orgs.size(); ++i) {
        if (i) cond += ",";
        cond += std::to_string(target_orgs[i]);
    }
    cond += ")";
    return cond;
}

std::string RdTrendServlet::buildTimeCond(time_t start, time_t end) {
    return "create_time >= FROM_UNIXTIME(" + std::to_string(start) + ")"
         + " AND create_time < FROM_UNIXTIME(" + std::to_string(end) + ")";
}

std::string RdTrendServlet::buildPRTimeCond(time_t start, time_t end) {
    return "prs.create_time >= FROM_UNIXTIME(" + std::to_string(start) + ")"
         + " AND prs.create_time < FROM_UNIXTIME(" + std::to_string(end) + ")";
}

static std::string buildOrgFilterForPR(const std::vector<int64_t>& target_orgs) {
    if (target_orgs.empty()) return "";
    std::string cond = " AND repos.org_id IN (";
    for (size_t i = 0; i < target_orgs.size(); ++i) {
        if (i) cond += ",";
        cond += std::to_string(target_orgs[i]);
    }
    cond += ")";
    return cond;
}

int64_t RdTrendServlet::countBySql(const TrendContext& ctx, const std::string& table, const std::string& extra_cond) {
    std::string sql = "SELECT COUNT(*) FROM " + table + " WHERE is_deleted = 0";
    std::string org_filter = buildOrgFilter(ctx.target_orgs);
    if (!org_filter.empty()) sql += " AND " + org_filter;
    if (!extra_cond.empty()) sql += " AND " + extra_cond;

    auto stmt = ctx.db->prepare(sql);
    if (!stmt) return 0;
    auto rt = stmt->query();
    if (!rt || !rt->next()) return 0;
    return rt->getInt64(0);
}

double RdTrendServlet::avgCycleTime(const TrendContext& ctx, const std::string& table, const std::string& status_cond) {
    std::string sql = "SELECT AVG(TIMESTAMPDIFF(SECOND, create_time, update_time)) / 86400.0 FROM " + table
                   + " WHERE is_deleted = 0 AND " + status_cond;
    std::string org_filter = buildOrgFilter(ctx.target_orgs);
    if (!org_filter.empty()) sql += " AND " + org_filter;
    sql += " AND " + buildTimeCond(ctx.start_date, ctx.end_date);

    auto stmt = ctx.db->prepare(sql);
    if (!stmt) return 0.0;
    auto rt = stmt->query();
    if (!rt || !rt->next()) return 0.0;
    return rt->getDouble(0);
}

void RdTrendServlet::addSeriesBySql(TrendContext& ctx, const std::string& table, const std::string& extra_cond, const std::string& field_name) {
    std::string sql = "SELECT DATE_FORMAT(create_time, '%Y-%m-%d') as d, COUNT(*) as cnt FROM " + table
                   + " WHERE is_deleted = 0";
    std::string org_filter = buildOrgFilter(ctx.target_orgs);
    if (!org_filter.empty()) sql += " AND " + org_filter;
    if (!extra_cond.empty()) sql += " AND " + extra_cond;
    sql += " AND " + buildTimeCond(ctx.start_date, ctx.end_date) + " GROUP BY d ORDER BY d";

    auto stmt = ctx.db->prepare(sql);
    if (!stmt) return;
    auto rt = stmt->query();
    if (!rt) return;
    while (rt->next()) {
        std::string d = rt->getString(0);
        std::string key = d;
        if (ctx.granularity == "week") {
            key = formatGroupKey(parseDate(d), "week");
        } else if (ctx.granularity == "month") {
            key = d.substr(0, 7);
        }
        auto it = ctx.series_map.find(key);
        if (it != ctx.series_map.end()) {
            it->second[field_name] = it->second[field_name].asInt64() + rt->getInt64(1);
        }
    }
}

// ==================== 核心处理 ====================

int32_t RdTrendServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    TrendContext ctx;
    do {
        int64_t org_id = request->getParamAs<int64_t>("org_id", 0);
        int64_t period_days = request->getParamAs<int64_t>("period_days", 30);
        std::string start_date_str = request->getParam("start_date");
        std::string end_date_str = request->getParam("end_date");
        ctx.granularity = request->getParam("granularity", "day");

        if (ctx.granularity != "day" && ctx.granularity != "week" && ctx.granularity != "month") {
            ctx.granularity = "day";
        }

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setErrno(errcode::NOT_LOGIN);
            break;
        }

        auto user = UserMgr::GetInstance()->get(uid);
        if (!user) {
            result->setErrno(errcode::USER_NOT_FOUND);
            break;
        }
        int32_t system_role = user->getRole();
        bool is_admin = (system_role == UserManager::Role::ADMIN);

        time_t now = time(0);
        if (!end_date_str.empty()) {
            ctx.end_date = parseDate(end_date_str) + 86400;
        } else {
            ctx.end_date = now;
        }
        if (!start_date_str.empty()) {
            ctx.start_date = parseDate(start_date_str);
        } else {
            ctx.start_date = ctx.end_date - period_days * 86400;
        }

        if (ctx.start_date >= ctx.end_date) {
            result->setErrno(errcode::PARAM_INVALID, "日期范围不合法");
            break;
        }

        // 确定统计范围
        if (org_id) {
            ctx.target_orgs.push_back(org_id);
        } else if (!is_admin) {
            std::vector<data::OrganizationUserRelInfo::ptr> user_orgs;
            OrganizationUserRelMgr::GetInstance()->getOrgByUserId(user_orgs, uid,
                OrganizationUserRelManager::Status::APPROVED, true);
            for (auto& rel : user_orgs) {
                ctx.target_orgs.push_back(rel->getOrgId());
            }
            if (ctx.target_orgs.empty()) {
                result->setErrno(errcode::ACCESS_DENIED, "无权访问任何组织");
                break;
            }
        }
        // 管理员不传 org_id → target_orgs 为空 → 查所有组织

        // 权限校验
        if (org_id && !is_admin) {
            auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
            if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED
                    || rel->getRole() < OrganizationManager::Role::REPORTER) {
                result->setErrno(errcode::ACCESS_DENIED);
                break;
            }
        }

        ctx.db = getDB();
        if (!ctx.db) {
            result->setErrno(errcode::DB_OPERATION_FAILED);
            break;
        }

        // ====== 汇总统计 ======
        std::string time_cond = buildTimeCond(ctx.start_date, ctx.end_date);
        ctx.new_req   = countBySql(ctx, "requirement", time_cond);
        ctx.done_req  = countBySql(ctx, "requirement", "status >= 3 AND " + time_cond);
        ctx.bug_total = countBySql(ctx, "bug", time_cond);
        ctx.done_bug  = countBySql(ctx, "bug", "status >= 2 AND " + time_cond);
        ctx.new_task  = countBySql(ctx, "task", time_cond);
        ctx.done_task = countBySql(ctx, "task", "status >= 2 AND " + time_cond);
        ctx.total_completed = ctx.done_req + ctx.done_bug + ctx.done_task;

        // avg_review_pass_rate
        {
            std::string pr_time = buildPRTimeCond(ctx.start_date, ctx.end_date);
            std::string org_pr = buildOrgFilterForPR(ctx.target_orgs);
            std::string base_sql = "FROM organization_repo_prs prs"
                                   " INNER JOIN organization_repos repos ON prs.repo_id = repos.id"
                                   " WHERE prs.review_status != '' AND prs.review_status != 'pending'"
                                   + org_pr + " AND " + pr_time;
            auto countReviewed = [&](const std::string& extra) -> int64_t {
                std::string sql = "SELECT COUNT(*) " + base_sql + extra;
                auto stmt = ctx.db->prepare(sql);
                if (!stmt) return 0;
                auto rt = stmt->query();
                if (rt && rt->next()) return rt->getInt64(0);
                return 0;
            };
            int64_t total_reviewed = countReviewed("");
            if (total_reviewed > 0) {
                int64_t approved = countReviewed(" AND prs.review_status = 'approved'");
                ctx.review_pass_rate = approved * 100 / total_reviewed;
            }
        }

        // avg_cycle_time
        {
            double req_cycle  = avgCycleTime(ctx, "requirement", "status >= 3");
            double bug_cycle  = avgCycleTime(ctx, "bug", "status >= 2");
            double task_cycle = avgCycleTime(ctx, "task", "status >= 2");
            if (ctx.total_completed > 0) {
                ctx.avg_cycle_time = (req_cycle * ctx.done_req + bug_cycle * ctx.done_bug + task_cycle * ctx.done_task)
                                   / ctx.total_completed;
            }
        }

        // task_delta
        {
            int64_t tasks_at_end   = countBySql(ctx, "task", "create_time < FROM_UNIXTIME(" + std::to_string(ctx.end_date) + ")");
            int64_t tasks_at_start = countBySql(ctx, "task", "create_time < FROM_UNIXTIME(" + std::to_string(ctx.start_date) + ")");
            ctx.task_delta = tasks_at_end - tasks_at_start;
        }

        // cycle_delta
        {
            time_t prev_end = ctx.start_date;
            time_t prev_start = ctx.start_date - (ctx.end_date - ctx.start_date);
            if (prev_start > 0) {
                TrendContext prev_ctx;
                prev_ctx.db = ctx.db;
                prev_ctx.target_orgs = ctx.target_orgs;
                prev_ctx.start_date = prev_start;
                prev_ctx.end_date = prev_end;

                double prev_req  = avgCycleTime(prev_ctx, "requirement", "status >= 3");
                double prev_bug  = avgCycleTime(prev_ctx, "bug", "status >= 2");
                double prev_task = avgCycleTime(prev_ctx, "task", "status >= 2");
                if (ctx.total_completed > 0) {
                    double prev_total = (prev_req * ctx.done_req + prev_bug * ctx.done_bug + prev_task * ctx.done_task)
                                      / ctx.total_completed;
                    ctx.cycle_delta = ctx.avg_cycle_time - prev_total;
                }
            }
        }

        // ====== 趋势序列 ======
        Json::Value series_arr(Json::arrayValue);
        {
            std::vector<time_t> date_points;
            time_t cursor = ctx.start_date;
            while (cursor < ctx.end_date) {
                date_points.push_back(cursor);
                if (ctx.granularity == "day") {
                    cursor += 86400;
                } else if (ctx.granularity == "week") {
                    cursor += 7 * 86400;
                } else {
                    struct tm tm;
                    localtime_r(&cursor, &tm);
                    tm.tm_mon += 1;
                    cursor = mktime(&tm);
                }
            }

            for (auto& t : date_points) {
                std::string key = formatGroupKey(t, ctx.granularity);
                if (ctx.series_map.find(key) == ctx.series_map.end()) {
                    Json::Value pt;
                    pt["date"] = key;
                    pt["requirements"] = 0;
                    pt["bugs"] = 0;
                    pt["tasks"] = 0;
                    pt["completed"] = 0;
                    pt["reopened"] = 0;
                    pt["review_pass_rate"] = 0;
                    pt["cycle_time"] = 0;
                    ctx.series_map[key] = pt;
                }
            }

            addSeriesBySql(ctx, "requirement", "", "requirements");
            addSeriesBySql(ctx, "bug", "", "bugs");
            addSeriesBySql(ctx, "task", "", "tasks");
            addSeriesBySql(ctx, "requirement", "status >= 3", "completed");
            addSeriesBySql(ctx, "bug", "status >= 2", "completed");
            addSeriesBySql(ctx, "task", "status >= 2", "completed");

            for (auto& t : date_points) {
                std::string key = formatGroupKey(t, ctx.granularity);
                auto it = ctx.series_map.find(key);
                if (it != ctx.series_map.end()) {
                    series_arr.append(it->second);
                }
            }
        }

        // ====== Summary ======
        auto& summary = result->jsondata["summary"];
        summary["completed_total"] = ctx.total_completed;
        summary["bug_total"] = ctx.bug_total;
        summary["avg_review_pass_rate"] = ctx.review_pass_rate;
        summary["avg_cycle_time"] = std::round(ctx.avg_cycle_time * 10.0) / 10.0;
        summary["task_delta"] = ctx.task_delta;
        summary["cycle_delta"] = std::round(ctx.cycle_delta * 10.0) / 10.0;

        result->jsondata["series"] = series_arr;

        // ====== 工作分布 ======
        int64_t total_items = ctx.new_req + ctx.bug_total + ctx.new_task;
        auto& dist = result->jsondata["work_distribution"];
        {
            std::string pr_time = buildPRTimeCond(ctx.start_date, ctx.end_date);
            std::string org_pr = buildOrgFilterForPR(ctx.target_orgs);
            std::string sql = "SELECT COUNT(*) FROM organization_repo_prs prs"
                              " INNER JOIN organization_repos repos ON prs.repo_id = repos.id"
                              " WHERE 1=1" + org_pr + " AND " + pr_time;
            int64_t pr_count = 0;
            auto stmt = ctx.db->prepare(sql);
            if (stmt) {
                auto rt = stmt->query();
                if (rt && rt->next()) pr_count = rt->getInt64(0);
            }
            int64_t dist_total = total_items + pr_count;
            if (dist_total > 0) {
                dist["requirement_delivery"] = std::round(ctx.new_req * 100.0 / dist_total);
                dist["bug_fix"] = std::round(ctx.bug_total * 100.0 / dist_total);
                dist["rd_task"] = std::round(ctx.new_task * 100.0 / dist_total);
                dist["code_review"] = std::round(pr_count * 100.0 / dist_total);
            } else {
                dist["requirement_delivery"] = 0;
                dist["bug_fix"] = 0;
                dist["rd_task"] = 0;
                dist["code_review"] = 0;
            }
        }

        // ====== 团队健康度 ======
        auto& health = result->jsondata["team_health"];
        health["throughput"] = std::min((int64_t)100, ctx.total_completed);
        if (total_items > 0) {
            health["bug_pressure"] = 100 - std::min(ctx.bug_total * 100 / total_items, (int64_t)80);
        } else {
            health["bug_pressure"] = 100;
        }
        health["review_efficiency"] = ctx.review_pass_rate;
        health["rework_risk"] = 0;

        // ====== 洞察文案 ======
        result->jsondata["insights"] = Json::Value(Json::arrayValue);

    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
