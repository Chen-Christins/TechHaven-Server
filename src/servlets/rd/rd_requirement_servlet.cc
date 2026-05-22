#include "rd_requirement_servlet.h"
#include "rd_helper.h"
#include <chen/log/log.h>
#include "../../manager/user_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/requirement_manager.h"
#include "../../permission.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

// ============================================================================
// RdRequirementServlet
// ============================================================================

RdRequirementServlet::RdRequirementServlet()
    : BlogLoginedServlet("RdRequirementServlet") {
}

int32_t RdRequirementServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    auto method = request->getMethod();
    if (method == chen::http::HttpMethod::GET) {
        return handleList(request, response, session, result);
    } else if (method == chen::http::HttpMethod::POST) {
        std::string path = request->getPath();
        if (path.find("/delete") != std::string::npos) {
            return handleDelete(request, response, session, result);
        }
        return handleCreate(request, response, session, result);
    }
    result->setResult(405, "Method Not Allowed");
    response->setBody(result->toJsonString());
    return 0;
}

void RdRequirementServlet::buildRequirementJson(Json::Value& item, data::RequirementInfo::ptr info) {
    item["id"] = info->getId();
    item["title"] = info->getTitle();
    item["description"] = info->getDescription();
    item["priority"] = rd::priorityToString(info->getPriority());
    item["status"] = rd::requirementStatusToString(info->getStatus());
    item["creator"] = rd::getUserName(info->getCreatorId());
    item["creator_id"] = info->getCreatorId();
    item["assignee"] = rd::getUserName(info->getAssigneeId());
    item["assignee_id"] = info->getAssigneeId();
    item["org_id"] = info->getOrgId();
    item["org_name"] = rd::getOrgName(info->getOrgId());
    item["iteration"] = info->getIteration();
    item["category"] = info->getCategory();
    item["source"] = info->getSource();
    item["deadline"] = info->getDeadline();
    item["created_at"] = info->getCreateTime();
    item["updated_at"] = info->getUpdateTime();
}

int32_t RdRequirementServlet::handleList(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t id = request->getParamAs<int64_t>("id", 0);
        int64_t org_id = request->getParamAs<int64_t>("org_id", 0);

        int64_t uid = getUserId(request);
        int32_t systemRole = UserMgr::GetInstance()->get(uid)->getRole();
        bool isPlatformAdmin = (systemRole == UserManager::Role::ADMIN);

        // Detail mode
        if (id) {
            auto info = RequirementMgr::GetInstance()->get(id);
            if (!info || info->getIsDeleted()) {
                result->setResult(404, "requirement not exist");
                break;
            }
            int64_t infoOrgId = info->getOrgId();
            if (!isPlatformAdmin) {
                auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(infoOrgId, uid);
                if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
                    result->setResult(403, "Access Denied");
                    break;
                }
                if (!permission::canViewRequirement(rel->getRole(), uid, info->getCreatorId())) {
                    result->setResult(403, "Access Denied");
                    break;
                }
            }
            buildRequirementJson(result->jsondata, info);
            break;
        }

        // List mode
        uint64_t page = request->getParamAs<uint64_t>("page", 1);
        uint64_t pageSize = request->getParamAs<uint64_t>("page_size", 10);
        uint64_t offset, size;
        rd::parsePagination(page, pageSize, offset, size);

        std::string search = request->getParam("search");
        std::string statusStr = request->getParam("status");
        std::string priorityStr = request->getParam("priority");

        std::vector<data::RequirementInfo::ptr> all;
        if (isPlatformAdmin && org_id) {
            RequirementMgr::GetInstance()->listByOrg(all, org_id, 0, UINT64_MAX, -1, true);
        } else if (!isPlatformAdmin) {
            // 获取用户所有组织的需求
            std::vector<data::OrganizationUserRelInfo::ptr> userOrgs;
            OrganizationUserRelMgr::GetInstance()->getOrgByUserId(userOrgs, uid, OrganizationUserRelManager::Status::APPROVED, true);
            for (auto& rel : userOrgs) {
                if (org_id && rel->getOrgId() != org_id) continue;
                std::vector<data::RequirementInfo::ptr> orgReqs;
                RequirementMgr::GetInstance()->listByOrg(orgReqs, rel->getOrgId(), 0, UINT64_MAX, -1, true);
                for (auto& req : orgReqs) {
                    if (permission::canViewRequirement(rel->getRole(), uid, req->getCreatorId())) {
                        all.push_back(req);
                    }
                }
            }
        } else {
            RequirementMgr::GetInstance()->listByPages(all, 0, UINT64_MAX, -1, true);
        }

        // 过滤
        std::vector<data::RequirementInfo::ptr> filtered;
        for (auto& info : all) {
            if (!statusStr.empty() && rd::stringToRequirementStatus(statusStr) != info->getStatus()) continue;
            if (!priorityStr.empty() && rd::stringToPriority(priorityStr) != info->getPriority()) continue;
            if (!search.empty()) {
                std::string title = info->getTitle();
                std::string desc = info->getDescription();
                std::string creator = rd::getUserName(info->getCreatorId());
                if (title.find(search) == std::string::npos
                        && desc.find(search) == std::string::npos
                        && creator.find(search) == std::string::npos) continue;
            }
            filtered.push_back(info);
        }

        uint64_t total = filtered.size();
        Json::Value arr(Json::arrayValue);
        for (uint64_t i = offset; i < filtered.size() && arr.size() < size; ++i) {
            Json::Value item;
            buildRequirementJson(item, filtered[i]);
            arr.append(item);
        }

        result->set("total", total);
        result->set("list", arr);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

int32_t RdRequirementServlet::handleCreate(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t id = request->getParamAs<int64_t>("id", 0);
        // 也尝试从 JSON body 读取
        std::string reqBody = request->getBody();
        Json::Value body;
        if (!reqBody.empty()) {
            Json::Reader reader;
            if (reader.parse(reqBody, body)) {
                if (!id) id = rd::getJsonInt64(body, "id");
            }
        }

        int64_t org_id = request->getParamAs<int64_t>("org_id", 0);
        if (!org_id && !body.isNull()) org_id = rd::getJsonInt64(body, "org_id");

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }

        auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
            result->setResult(403, "not a member of this organization");
            break;
        }
        int32_t orgRole = rel->getRole();

        bool is_new = false;
        data::RequirementInfo::ptr info;

        if (id) {
            info = RequirementMgr::GetInstance()->get(id);
            if (!info || info->getOrgId() != org_id) {
                result->setResult(404, "requirement not exist");
                break;
            }
            if (!permission::canEditRequirement(orgRole)) {
                result->setResult(403, "Access Denied");
                break;
            }
        } else {
            if (!permission::canCreateRequirement(orgRole)) {
                result->setResult(403, "Access Denied");
                break;
            }
            info.reset(new data::RequirementInfo);
            info->setOrgId(org_id);
            info->setCreatorId(uid);
            info->setCreateTime(time(0));
            is_new = true;
        }

        // Parse params from JSON body or query params
        auto getParam = [&](const std::string& key) -> std::string {
            if (!body.isNull() && body.isMember(key)) {
                if (body[key].isString()) return body[key].asString();
                return std::to_string(rd::getJsonInt64(body, key));
            }
            return request->getParam(key);
        };
        auto getParamInt = [&](const std::string& key) -> int64_t {
            if (!body.isNull() && body.isMember(key)) return rd::getJsonInt64(body, key);
            return request->getParamAs<int64_t>(key, 0);
        };

        std::string title = getParam("title");
        if (!title.empty()) info->setTitle(title);
        std::string desc = getParam("description");
        if (!desc.empty()) info->setDescription(desc);

        std::string priorityStr = getParam("priority");
        if (!priorityStr.empty()) {
            int32_t p = rd::stringToPriority(priorityStr);
            if (p >= 0) info->setPriority(p);
        }
        std::string statusStr = getParam("status");
        if (!statusStr.empty()) {
            int32_t s = rd::stringToRequirementStatus(statusStr);
            if (s >= 0) info->setStatus(s);
        }

        int64_t assignee_id = getParamInt("assignee_id");
        if (assignee_id) info->setAssigneeId(assignee_id);
        int64_t deadline = getParamInt("deadline");
        if (deadline) info->setDeadline(deadline);

        std::string iteration = getParam("iteration");
        if (!iteration.empty()) info->setIteration(iteration);
        std::string category = getParam("category");
        if (!category.empty()) info->setCategory(category);
        std::string source = getParam("source");
        if (!source.empty()) info->setSource(source);

        info->setIsDeleted(0);
        info->setUpdateTime(time(0));

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db error");
            break;
        }

        if (data::RequirementInfoDao::InsertOrUpdate(info, db)) {
            result->setResult(500, "insert or update requirement fail");
            ERROR(logger) << "db error, errno=" << db->getErrno() << ", errstr=" << db->getErrStr();
            break;
        }

        if (is_new) RequirementMgr::GetInstance()->add(info);

        buildRequirementJson(result->jsondata, info);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

int32_t RdRequirementServlet::handleDelete(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        std::string reqBody = request->getBody();
        Json::Value body;
        if (!reqBody.empty()) { Json::Reader reader; reader.parse(reqBody, body); }

        int64_t id = rd::getJsonInt64(body, "id");
        if (!id) id = request->getParamAs<int64_t>("id", 0);
        std::string idsStr = body.get("ids", "").asString();
        if (idsStr.empty()) idsStr = request->getParam("ids");
        int64_t org_id = rd::getJsonInt64(body, "org_id");
        if (!org_id) org_id = request->getParamAs<int64_t>("org_id", 0);

        std::set<int64_t> delIds;
        if (id) delIds.insert(id);
        if (!idsStr.empty()) {
            for (auto& s : chen::split(idsStr, ",")) {
                delIds.insert(chen::TypeUtil::Atoi(s));
            }
        }

        int64_t uid = getUserId(request);
        auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
            result->setResult(403, "Access Denied");
            break;
        }
        if (!permission::canDeleteRequirement(rel->getRole())) {
            result->setResult(403, "Access Denied");
            break;
        }

        std::vector<data::RequirementInfo::ptr> all;
        RequirementMgr::GetInstance()->listByOrg(all, org_id, 0, UINT64_MAX, -1, true);

        std::vector<data::RequirementInfo::ptr> delItems;
        for (auto& i : all) {
            if (delIds.count(i->getId())) delItems.push_back(i);
        }

        auto db = getDB();
        if (!db) { result->setResult(500, "get db error"); break; }

        auto trans = db->openTransaction();
        if (!trans) { result->setResult(500, "open transaction fail"); break; }

        time_t now = time(0);
        for (auto& i : delItems) {
            i->setIsDeleted(1);
            i->setUpdateTime(now);
            data::RequirementInfoDao::Update(i, db);
        }
        if (!trans->commit()) {
            for (auto& i : delItems) i->setIsDeleted(0);
            result->setResult(500, "commit fail");
            break;
        }

        if (!delItems.empty()) {
            auto& jids = result->jsondata["ids"];
            for (auto& i : delItems) jids.append(i->getId());
        }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

// ============================================================================
// RdBugServlet
// ============================================================================

RdBugServlet::RdBugServlet()
    : BlogLoginedServlet("RdBugServlet") {
}

int32_t RdBugServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    auto method = request->getMethod();
    if (method == chen::http::HttpMethod::GET) {
        return handleList(request, response, session, result);
    } else if (method == chen::http::HttpMethod::POST) {
        std::string path = request->getPath();
        if (path.find("/delete") != std::string::npos) {
            return handleDelete(request, response, session, result);
        }
        return handleCreate(request, response, session, result);
    }
    result->setResult(405, "Method Not Allowed");
    response->setBody(result->toJsonString());
    return 0;
}

void RdBugServlet::buildBugJson(Json::Value& item, data::BugInfo::ptr info) {
    item["id"] = info->getId();
    item["title"] = info->getTitle();
    item["description"] = info->getDescription();
    item["severity"] = rd::severityToString(info->getSeverity());
    item["priority"] = rd::priorityToString(info->getPriority());
    item["status"] = rd::bugStatusToString(info->getStatus());
    item["creator"] = rd::getUserName(info->getCreatorId());
    item["creator_id"] = info->getCreatorId();
    item["assignee"] = rd::getUserName(info->getAssigneeId());
    item["assignee_id"] = info->getAssigneeId();
    item["org_id"] = info->getOrgId();
    item["org_name"] = rd::getOrgName(info->getOrgId());
    item["related_requirement_id"] = info->getRequirementId();
    item["module"] = info->getModule();
    item["steps_to_reproduce"] = info->getStepsToReproduce();
    item["environment"] = info->getEnvironment();
    item["created_at"] = info->getCreateTime();
    item["updated_at"] = info->getUpdateTime();
}

int32_t RdBugServlet::handleList(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t id = request->getParamAs<int64_t>("id", 0);
        int64_t org_id = request->getParamAs<int64_t>("org_id", 0);
        int64_t uid = getUserId(request);
        int32_t systemRole = UserMgr::GetInstance()->get(uid)->getRole();
        bool isPlatformAdmin = (systemRole == UserManager::Role::ADMIN);

        if (id) {
            auto info = BugMgr::GetInstance()->get(id);
            if (!info || info->getIsDeleted()) { result->setResult(404, "bug not exist"); break; }
            int64_t infoOrgId = info->getOrgId();
            if (!isPlatformAdmin) {
                auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(infoOrgId, uid);
                if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
                    result->setResult(403, "Access Denied"); break;
                }
                if (!permission::canViewBug(rel->getRole(), uid, info->getCreatorId())) {
                    result->setResult(403, "Access Denied"); break;
                }
            }
            buildBugJson(result->jsondata, info);
            break;
        }

        uint64_t page = request->getParamAs<uint64_t>("page", 1);
        uint64_t pageSize = request->getParamAs<uint64_t>("page_size", 10);
        uint64_t offset, size;
        rd::parsePagination(page, pageSize, offset, size);

        std::string search = request->getParam("search");
        std::string statusStr = request->getParam("status");
        std::string priorityStr = request->getParam("priority");
        std::string severityStr = request->getParam("severity");

        std::vector<data::BugInfo::ptr> all;
        if (isPlatformAdmin && org_id) {
            BugMgr::GetInstance()->listByOrg(all, org_id, 0, UINT64_MAX, -1, true);
        } else if (!isPlatformAdmin) {
            std::vector<data::OrganizationUserRelInfo::ptr> userOrgs;
            OrganizationUserRelMgr::GetInstance()->getOrgByUserId(userOrgs, uid, OrganizationUserRelManager::Status::APPROVED, true);
            for (auto& rel : userOrgs) {
                if (org_id && rel->getOrgId() != org_id) continue;
                std::vector<data::BugInfo::ptr> orgBugs;
                BugMgr::GetInstance()->listByOrg(orgBugs, rel->getOrgId(), 0, UINT64_MAX, -1, true);
                for (auto& bug : orgBugs) {
                    if (permission::canViewBug(rel->getRole(), uid, bug->getCreatorId())) {
                        all.push_back(bug);
                    }
                }
            }
        } else {
            BugMgr::GetInstance()->listByPages(all, 0, UINT64_MAX, -1, true);
        }

        std::vector<data::BugInfo::ptr> filtered;
        for (auto& info : all) {
            if (!statusStr.empty() && rd::stringToBugStatus(statusStr) != info->getStatus()) continue;
            if (!priorityStr.empty() && rd::stringToPriority(priorityStr) != info->getPriority()) continue;
            if (!severityStr.empty() && rd::stringToSeverity(severityStr) != info->getSeverity()) continue;
            if (!search.empty()) {
                std::string title = info->getTitle();
                std::string desc = info->getDescription();
                if (title.find(search) == std::string::npos && desc.find(search) == std::string::npos) continue;
            }
            filtered.push_back(info);
        }

        uint64_t total = filtered.size();
        Json::Value arr(Json::arrayValue);
        for (uint64_t i = offset; i < filtered.size() && arr.size() < size; ++i) {
            Json::Value item;
            buildBugJson(item, filtered[i]);
            arr.append(item);
        }
        result->set("total", total);
        result->set("list", arr);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

int32_t RdBugServlet::handleCreate(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        std::string reqBody = request->getBody();
        Json::Value body;
        if (!reqBody.empty()) { Json::Reader reader; reader.parse(reqBody, body); }

        int64_t id = request->getParamAs<int64_t>("id", 0);
        if (!id && !body.isNull()) id = rd::getJsonInt64(body, "id");
        int64_t org_id = request->getParamAs<int64_t>("org_id", 0);
        if (!org_id && !body.isNull()) org_id = rd::getJsonInt64(body, "org_id");

        int64_t uid = getUserId(request);
        if (!uid) { result->setResult(500, "not login"); break; }

        auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
            result->setResult(403, "not a member of this organization"); break;
        }
        int32_t orgRole = rel->getRole();

        bool is_new = false;
        data::BugInfo::ptr info;
        if (id) {
            info = BugMgr::GetInstance()->get(id);
            if (!info || info->getOrgId() != org_id) { result->setResult(404, "bug not exist"); break; }
            if (!permission::canEditBug(orgRole, uid, info->getCreatorId())) {
                result->setResult(403, "Access Denied"); break;
            }
        } else {
            if (!permission::canCreateBug(orgRole)) { result->setResult(403, "Access Denied"); break; }
            info.reset(new data::BugInfo);
            info->setOrgId(org_id);
            info->setCreatorId(uid);
            info->setCreateTime(time(0));
            is_new = true;
        }

        auto getParam = [&](const std::string& key) -> std::string {
            if (!body.isNull() && body.isMember(key)) {
                if (body[key].isString()) return body[key].asString();
                return std::to_string(rd::getJsonInt64(body, key));
            }
            return request->getParam(key);
        };
        auto getParamInt = [&](const std::string& key) -> int64_t {
            if (!body.isNull() && body.isMember(key)) return rd::getJsonInt64(body, key);
            return request->getParamAs<int64_t>(key, 0);
        };

        std::string title = getParam("title");
        if (!title.empty()) info->setTitle(title);
        std::string desc = getParam("description");
        if (!desc.empty()) info->setDescription(desc);
        std::string severityStr = getParam("severity");
        if (!severityStr.empty()) { int32_t s = rd::stringToSeverity(severityStr); if (s >= 0) info->setSeverity(s); }
        std::string priorityStr = getParam("priority");
        if (!priorityStr.empty()) { int32_t p = rd::stringToPriority(priorityStr); if (p >= 0) info->setPriority(p); }
        std::string statusStr = getParam("status");
        if (!statusStr.empty()) { int32_t s = rd::stringToBugStatus(statusStr); if (s >= 0) info->setStatus(s); }

        int64_t assignee_id = getParamInt("assignee_id");
        if (assignee_id) info->setAssigneeId(assignee_id);
        int64_t requirement_id = getParamInt("related_requirement_id");
        if (requirement_id) info->setRequirementId(requirement_id);

        std::string module = getParam("module");
        if (!module.empty()) info->setModule(module);
        std::string steps = getParam("steps_to_reproduce");
        if (!steps.empty()) info->setStepsToReproduce(steps);
        std::string env = getParam("environment");
        if (!env.empty()) info->setEnvironment(env);

        info->setIsDeleted(0);
        info->setUpdateTime(time(0));

        auto db = getDB();
        if (!db) { result->setResult(500, "get db error"); break; }
        if (data::BugInfoDao::InsertOrUpdate(info, db)) {
            result->setResult(500, "insert or update bug fail");
            break;
        }
        if (is_new) BugMgr::GetInstance()->add(info);
        buildBugJson(result->jsondata, info);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

int32_t RdBugServlet::handleDelete(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        std::string reqBody = request->getBody();
        Json::Value body;
        if (!reqBody.empty()) { Json::Reader reader; reader.parse(reqBody, body); }

        int64_t id = rd::getJsonInt64(body, "id");
        if (!id) id = request->getParamAs<int64_t>("id", 0);
        std::string idsStr = body.get("ids", "").asString();
        if (idsStr.empty()) idsStr = request->getParam("ids");
        int64_t org_id = rd::getJsonInt64(body, "org_id");
        if (!org_id) org_id = request->getParamAs<int64_t>("org_id", 0);

        std::set<int64_t> delIds;
        if (id) delIds.insert(id);
        if (!idsStr.empty()) for (auto& s : chen::split(idsStr, ",")) delIds.insert(chen::TypeUtil::Atoi(s));

        int64_t uid = getUserId(request);
        auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
            result->setResult(403, "Access Denied"); break;
        }
        if (!permission::canDeleteBug(rel->getRole())) { result->setResult(403, "Access Denied"); break; }

        std::vector<data::BugInfo::ptr> all;
        BugMgr::GetInstance()->listByOrg(all, org_id, 0, UINT64_MAX, -1, true);
        std::vector<data::BugInfo::ptr> delItems;
        for (auto& i : all) if (delIds.count(i->getId())) delItems.push_back(i);

        auto db = getDB();
        if (!db) { result->setResult(500, "get db error"); break; }
        auto trans = db->openTransaction();
        if (!trans) { result->setResult(500, "open transaction fail"); break; }
        time_t now = time(0);
        for (auto& i : delItems) { i->setIsDeleted(1); i->setUpdateTime(now); data::BugInfoDao::Update(i, db); }
        if (!trans->commit()) {
            for (auto& i : delItems) i->setIsDeleted(0);
            result->setResult(500, "commit fail"); break;
        }
        if (!delItems.empty()) { auto& jids = result->jsondata["ids"]; for (auto& i : delItems) jids.append(i->getId()); }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

// ============================================================================
// RdTaskServlet
// ============================================================================

RdTaskServlet::RdTaskServlet()
    : BlogLoginedServlet("RdTaskServlet") {
}

int32_t RdTaskServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    auto method = request->getMethod();
    if (method == chen::http::HttpMethod::GET) return handleList(request, response, session, result);
    else if (method == chen::http::HttpMethod::POST) {
        std::string path = request->getPath();
        if (path.find("/delete") != std::string::npos) {
            return handleDelete(request, response, session, result);
        }
        return handleCreate(request, response, session, result);
    }
    result->setResult(405, "Method Not Allowed");
    response->setBody(result->toJsonString());
    return 0;
}

void RdTaskServlet::buildTaskJson(Json::Value& item, data::TaskInfo::ptr info) {
    item["id"] = info->getId();
    item["title"] = info->getTitle();
    item["description"] = info->getDescription();
    item["status"] = rd::taskStatusToString(info->getStatus());
    item["priority"] = rd::priorityToString(info->getPriority());
    item["creator"] = rd::getUserName(info->getCreatorId());
    item["creator_id"] = info->getCreatorId();
    item["assignee"] = rd::getUserName(info->getAssigneeId());
    item["assignee_id"] = info->getAssigneeId();
    item["org_id"] = info->getOrgId();
    item["org_name"] = rd::getOrgName(info->getOrgId());
    item["requirement_id"] = info->getRequirementId();
    item["bug_id"] = info->getBugId();
    item["deadline"] = info->getDeadline();
    item["estimated_hours"] = info->getEstimatedHours();
    item["created_at"] = info->getCreateTime();
    item["updated_at"] = info->getUpdateTime();
}

int32_t RdTaskServlet::handleList(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t id = request->getParamAs<int64_t>("id", 0);
        int64_t org_id = request->getParamAs<int64_t>("org_id", 0);
        int64_t uid = getUserId(request);
        int32_t systemRole = UserMgr::GetInstance()->get(uid)->getRole();
        bool isPlatformAdmin = (systemRole == UserManager::Role::ADMIN);

        if (id) {
            auto info = TaskMgr::GetInstance()->get(id);
            if (!info || info->getIsDeleted()) { result->setResult(404, "task not exist"); break; }
            int64_t infoOrgId = info->getOrgId();
            if (!isPlatformAdmin) {
                auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(infoOrgId, uid);
                if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
                    result->setResult(403, "Access Denied"); break;
                }
                bool isRelated = (uid == info->getCreatorId() || uid == info->getAssigneeId());
                if (!permission::canViewTask(rel->getRole(), isRelated)) {
                    result->setResult(403, "Access Denied"); break;
                }
            }
            buildTaskJson(result->jsondata, info);
            break;
        }

        uint64_t page = request->getParamAs<uint64_t>("page", 1);
        uint64_t pageSize = request->getParamAs<uint64_t>("page_size", 10);
        uint64_t offset, size;
        rd::parsePagination(page, pageSize, offset, size);

        std::string search = request->getParam("search");
        std::string statusStr = request->getParam("status");
        std::string priorityStr = request->getParam("priority");
        int64_t filterAssigneeId = request->getParamAs<int64_t>("assignee_id", 0);

        std::vector<data::TaskInfo::ptr> all;
        if (isPlatformAdmin && org_id) {
            TaskMgr::GetInstance()->listByOrg(all, org_id, 0, UINT64_MAX, -1, true);
        } else if (!isPlatformAdmin) {
            std::vector<data::OrganizationUserRelInfo::ptr> userOrgs;
            OrganizationUserRelMgr::GetInstance()->getOrgByUserId(userOrgs, uid, OrganizationUserRelManager::Status::APPROVED, true);
            for (auto& rel : userOrgs) {
                if (org_id && rel->getOrgId() != org_id) continue;
                std::vector<data::TaskInfo::ptr> orgTasks;
                TaskMgr::GetInstance()->listByOrg(orgTasks, rel->getOrgId(), 0, UINT64_MAX, -1, true);
                for (auto& task : orgTasks) {
                    bool isRelated = (uid == task->getCreatorId() || uid == task->getAssigneeId());
                    if (permission::canViewTask(rel->getRole(), isRelated)) all.push_back(task);
                }
            }
        } else {
            TaskMgr::GetInstance()->listByPages(all, 0, UINT64_MAX, -1, true);
        }

        std::vector<data::TaskInfo::ptr> filtered;
        for (auto& info : all) {
            if (!statusStr.empty() && rd::stringToTaskStatus(statusStr) != info->getStatus()) continue;
            if (!priorityStr.empty() && rd::stringToPriority(priorityStr) != info->getPriority()) continue;
            if (filterAssigneeId && info->getAssigneeId() != filterAssigneeId) continue;
            if (!search.empty()) {
                std::string title = info->getTitle();
                if (title.find(search) == std::string::npos) continue;
            }
            filtered.push_back(info);
        }

        uint64_t total = filtered.size();
        Json::Value arr(Json::arrayValue);
        for (uint64_t i = offset; i < filtered.size() && arr.size() < size; ++i) {
            Json::Value item;
            buildTaskJson(item, filtered[i]);
            arr.append(item);
        }
        result->set("total", total);
        result->set("list", arr);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

int32_t RdTaskServlet::handleCreate(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        std::string reqBody = request->getBody();
        Json::Value body;
        if (!reqBody.empty()) { Json::Reader reader; reader.parse(reqBody, body); }

        int64_t id = request->getParamAs<int64_t>("id", 0);
        if (!id && !body.isNull()) id = rd::getJsonInt64(body, "id");
        int64_t org_id = request->getParamAs<int64_t>("org_id", 0);
        if (!org_id && !body.isNull()) org_id = rd::getJsonInt64(body, "org_id");

        int64_t uid = getUserId(request);
        if (!uid) { result->setResult(500, "not login"); break; }

        auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
            result->setResult(403, "not a member of this organization"); break;
        }
        int32_t orgRole = rel->getRole();

        bool is_new = false;
        data::TaskInfo::ptr info;
        if (id) {
            info = TaskMgr::GetInstance()->get(id);
            if (!info || info->getOrgId() != org_id) { result->setResult(404, "task not exist"); break; }
            if (!permission::canEditTask(orgRole, uid, info->getAssigneeId())) {
                result->setResult(403, "Access Denied"); break;
            }
        } else {
            if (!permission::canCreateTask(orgRole)) { result->setResult(403, "Access Denied"); break; }
            info.reset(new data::TaskInfo);
            info->setOrgId(org_id);
            info->setCreatorId(uid);
            info->setCreateTime(time(0));
            is_new = true;
        }

        auto getParam = [&](const std::string& key) -> std::string {
            if (!body.isNull() && body.isMember(key)) {
                if (body[key].isString()) return body[key].asString();
                return std::to_string(rd::getJsonInt64(body, key));
            }
            return request->getParam(key);
        };
        auto getParamInt = [&](const std::string& key) -> int64_t {
            if (!body.isNull() && body.isMember(key)) return rd::getJsonInt64(body, key);
            return request->getParamAs<int64_t>(key, 0);
        };

        std::string title = getParam("title");
        if (!title.empty()) info->setTitle(title);
        std::string desc = getParam("description");
        if (!desc.empty()) info->setDescription(desc);
        std::string priorityStr = getParam("priority");
        if (!priorityStr.empty()) { int32_t p = rd::stringToPriority(priorityStr); if (p >= 0) info->setPriority(p); }
        std::string statusStr = getParam("status");
        if (!statusStr.empty()) { int32_t s = rd::stringToTaskStatus(statusStr); if (s >= 0) info->setStatus(s); }

        int64_t assignee_id = getParamInt("assignee_id");
        if (assignee_id) info->setAssigneeId(assignee_id);
        int64_t requirement_id = getParamInt("requirement_id");
        if (requirement_id) info->setRequirementId(requirement_id);
        int64_t bug_id = getParamInt("bug_id");
        if (bug_id) info->setBugId(bug_id);
        int64_t deadline = getParamInt("deadline");
        if (deadline) info->setDeadline(deadline);
        int64_t estimated_hours = getParamInt("estimated_hours");
        if (estimated_hours) info->setEstimatedHours(estimated_hours);

        info->setIsDeleted(0);
        info->setUpdateTime(time(0));

        auto db = getDB();
        if (!db) { result->setResult(500, "get db error"); break; }
        if (data::TaskInfoDao::InsertOrUpdate(info, db)) {
            result->setResult(500, "insert or update task fail"); break;
        }
        if (is_new) TaskMgr::GetInstance()->add(info);
        buildTaskJson(result->jsondata, info);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

int32_t RdTaskServlet::handleDelete(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        std::string reqBody = request->getBody();
        Json::Value body;
        if (!reqBody.empty()) { Json::Reader reader; reader.parse(reqBody, body); }

        int64_t id = rd::getJsonInt64(body, "id");
        if (!id) id = request->getParamAs<int64_t>("id", 0);
        std::string idsStr = body.get("ids", "").asString();
        if (idsStr.empty()) idsStr = request->getParam("ids");
        int64_t org_id = rd::getJsonInt64(body, "org_id");
        if (!org_id) org_id = request->getParamAs<int64_t>("org_id", 0);

        std::set<int64_t> delIds;
        if (id) delIds.insert(id);
        if (!idsStr.empty()) for (auto& s : chen::split(idsStr, ",")) delIds.insert(chen::TypeUtil::Atoi(s));

        int64_t uid = getUserId(request);
        auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
            result->setResult(403, "Access Denied"); break;
        }
        if (!permission::canDeleteTask(rel->getRole())) { result->setResult(403, "Access Denied"); break; }

        std::vector<data::TaskInfo::ptr> all;
        TaskMgr::GetInstance()->listByOrg(all, org_id, 0, UINT64_MAX, -1, true);
        std::vector<data::TaskInfo::ptr> delItems;
        for (auto& i : all) if (delIds.count(i->getId())) delItems.push_back(i);

        auto db = getDB();
        if (!db) { result->setResult(500, "get db error"); break; }
        auto trans = db->openTransaction();
        if (!trans) { result->setResult(500, "open transaction fail"); break; }
        time_t now = time(0);
        for (auto& i : delItems) { i->setIsDeleted(1); i->setUpdateTime(now); data::TaskInfoDao::Update(i, db); }
        if (!trans->commit()) {
            for (auto& i : delItems) i->setIsDeleted(0);
            result->setResult(500, "commit fail"); break;
        }
        if (!delItems.empty()) { auto& jids = result->jsondata["ids"]; for (auto& i : delItems) jids.append(i->getId()); }
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
