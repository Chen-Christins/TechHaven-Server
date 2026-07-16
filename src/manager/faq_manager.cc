#include "faq_manager.h"

#include "../util.h"

#include <chen/log/log.h>

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

FaqManager::FaqManager() {
}

bool FaqManager::listAll(std::vector<data::HelpFaqsInfo::ptr>& infos) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }
    auto qb = chen::QueryBuilder::Create("help_faqs");
    qb->where("is_deleted", "=", (int64_t)0);
    qb->orderBy("sort_order", "ASC");
    qb->orderBy("id", "ASC");
    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return false;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (!rt) {
        return false;
    }
    while (rt->next()) {
        auto info = data::HelpFaqsInfoDao::ParseRow(rt);
        if (info) {
            infos.push_back(info);
        }
    }
    return true;
}

bool FaqManager::searchByKeyword(const std::string& keyword, std::vector<data::HelpFaqsInfo::ptr>& infos) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }
    auto qb = chen::QueryBuilder::Create("help_faqs");
    qb->where("is_deleted", "=", (int64_t)0);
    qb->orderBy("sort_order", "ASC");
    qb->orderBy("id", "ASC");
    std::string sql = qb->buildQuerySQL();
    auto stmt = db->prepare(sql);
    if (!stmt) {
        ERROR(logger) << "stmt=" << sql << " errno=" << db->getErrno() << " errstr=" << db->getErrStr();
        return false;
    }
    qb->bindParams(stmt);
    auto rt = stmt->query();
    if (!rt) {
        return false;
    }
    while (rt->next()) {
        auto info = data::HelpFaqsInfoDao::ParseRow(rt);
        if (info) {
            if (info->getQ().find(keyword) != std::string::npos ||
                info->getA().find(keyword) != std::string::npos) {
                infos.push_back(info);
            }
        }
    }
    return true;
}

bool FaqManager::remove(int64_t id) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }

    auto info = data::HelpFaqsInfoDao::Query(id, db);
    if (!info) {
        ERROR(logger) << "faq not found: " << id;
        return false;
    }

    info->setIsDeleted(1);
    info->setUpdateTime(time(0));

    if (data::HelpFaqsInfoDao::Update(info, db)) {
        ERROR(logger) << "soft delete faq failed, errno=" << db->getErrno()
            << " errstr=" << db->getErrStr();
        return false;
    }

    return true;
}

bool FaqManager::update(int64_t id, const std::string& q, const std::string& a, const std::string& cat) {
    auto db = GetDB();
    if (!db) {
        ERROR(logger) << "Get DB connection fail";
        return false;
    }

    auto info = data::HelpFaqsInfoDao::Query(id, db);
    if (!info) {
        ERROR(logger) << "faq not found: " << id;
        return false;
    }

    if (!q.empty()) {
        info->setQ(q);
    }
    if (!a.empty()) {
        info->setA(a);
    }
    if (!cat.empty()) {
        info->setCat(cat);
    }
    info->setUpdateTime(time(0));

    if (data::HelpFaqsInfoDao::Update(info, db)) {
        ERROR(logger) << "update faq failed, errno=" << db->getErrno()
            << " errstr=" << db->getErrStr();
        return false;
    }

    return true;
}

}
