#include "assignment_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

bool AssignmentManager::loadAll() {
	auto db = GetDB();
	if (!db) {
        ERROR(logger) << "Get SQLite3 connection fail";
        return false;
    }
	std::vector<data::AssignmentInfo::ptr> results;
	if (blog::data::AssignmentInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "AssignmentManager loadAll fail";
        return false;
    }
	
	std::unordered_map<int64_t, data::AssignmentInfo::ptr> datas;
    std::unordered_map<std::string, std::unordered_map<std::string, data::AssignmentInfo::ptr>> subject_name_datas;
	for (auto& i : results) {
		datas[i->getId()] = i;
        subject_name_datas[i->getSubjectName()][i->getName()] = i;
	}

	std::unique_lock<std::shared_mutex> lock(m_mutex);
	m_datas.swap(datas);
	m_subject_name_datas.swap(subject_name_datas);
	return true;
}

uint64_t AssignmentManager::listByPages(std::vector<data::AssignmentInfo::ptr>& infos
        , uint64_t offset, uint64_t size, int32_t status, bool isValid) {
	std::shared_lock<std::shared_mutex> lock(m_mutex);

    auto check = [&](auto info) -> bool {
        if (isValid && info->getIsDeleted()) {
            return false;
        }
        if (status != -1 && info->getStatus() != status) {
            return false;
        }
        return true;
    };

    std::vector<data::AssignmentInfo::ptr> temp;
	for (auto& i : m_datas) {
        if (check(i.second)) {
            temp.emplace_back(i.second);
        }
    }

    if (offset < temp.size()) {
        for (size_t i = offset; i < temp.size(); ++i) {
            if (infos.size() >= size) {
                break;
            }
            infos.emplace_back(temp[i]);
        }
    }
    return temp.size();
}

void AssignmentManager::add(data::AssignmentInfo::ptr info) {
	std::unique_lock<std::shared_mutex> lock(m_mutex);
	m_datas[info->getId()] = info;
    m_subject_name_datas[info->getSubjectName()][info->getName()] = info;
}

data::AssignmentInfo::ptr AssignmentManager::get(int64_t id) {
	std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    return it == m_datas.end() ? nullptr : it->second;
}

data::AssignmentInfo::ptr AssignmentManager::getByName(const std::string& subject_name, const std::string& name) {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto sit = m_subject_name_datas.find(subject_name);
    if (sit == m_subject_name_datas.end()) {
        return nullptr;
    }
    auto nit = sit->second.find(name);
    return nit == sit->second.end() ? nullptr : nit->second;
}

AssignmentManager::AssignmentStats AssignmentManager::getStats() {
    AssignmentStats stats;
    std::shared_lock<std::shared_mutex> lock(m_mutex);

    for (auto& i : m_datas) {
        auto& info = i.second;
        if (info->getIsDeleted()) {
            continue;
        }
        stats.total++;
        switch (info->getStatus()) {
        case Status::ACTIVE:
            stats.active++;
            break;
        case Status::INACTIVE:
            stats.closed++;
            break;
        case Status::DRAFT:
            stats.draft++;
            break;
        }
    }

    return stats;
}

}