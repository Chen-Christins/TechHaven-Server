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
	for (auto& i : results) {
		datas[i->getId()] = i;
	}

	std::unique_lock<std::shared_mutex> lock(m_mutex);
	m_datas.swap(datas);
	
	return true;
}

bool AssignmentManager::listAll(std::vector<data::AssignmentInfo::ptr>& infos, bool valid) {
	std::shared_lock<std::shared_mutex> lock(m_mutex);
	if (m_datas.empty()) {
		return false;
	}
	if (valid) {
		for (auto& [id, info] : m_datas) {
			if (!info->getIsDeleted()) {
				infos.emplace_back(info);
			}
		}
	} else {
		for (auto& [id, info] : m_datas) {
			infos.emplace_back(info);
		}
	}
	return true;
}

void AssignmentManager::add(data::AssignmentInfo::ptr info) {
	std::unique_lock<std::shared_mutex> lock(m_mutex);
	m_datas[info->getId()] = info;
}

data::AssignmentInfo::ptr AssignmentManager::get(int64_t id) {
	std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    return it == m_datas.end() ? nullptr : it->second;
}

}