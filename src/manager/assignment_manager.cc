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
	std::unordered_map<int64_t, std::unordered_map<std::string, data::AssignmentInfo::ptr>> subject_names;
	for (auto& i : results) {
		datas[i->getId()] = i;
		subject_names[i->getSubjectId()][i->getName()] = i;
	}

	std::unique_lock<std::shared_mutex> lock(m_mutex);
	m_datas.swap(datas);
	m_subject_names.swap(subject_names);
	
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

bool AssignmentManager::listBySubjectId(std::vector<data::AssignmentInfo::ptr>& infos, int64_t id, bool valid) {
	std::shared_lock<std::shared_mutex> lock(m_mutex);
	auto it = m_subject_names.find(id);
    if (it == m_subject_names.end()) {
        return false;
    }
    if (valid) {
        for (auto& i : it->second) {
            if (i.second->getIsDeleted() == 0) {
                infos.push_back(i.second);
            }
        }
    } else {
        for (auto& i : it->second) {
            infos.push_back(i.second);
        }
    }
    return true;
}

void AssignmentManager::add(data::AssignmentInfo::ptr info) {
	std::unique_lock<std::shared_mutex> lock(m_mutex);
	m_datas[info->getId()] = info;
	m_subject_names[info->getSubjectId()][info->getName()] = info;
}

data::AssignmentInfo::ptr AssignmentManager::getBySubjectIdName(int64_t id, const std::string& name) {
	std::shared_lock<std::shared_mutex> lock(m_mutex);
	auto it = m_subject_names.find(id);
	if (it != m_subject_names.end()) {
		auto iit = it->second.find(name);
		return iit == it->second.end() ? nullptr : iit->second;
	}
	return nullptr;
}

data::AssignmentInfo::ptr AssignmentManager::get(int64_t id) {
	std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_datas.find(id);
    return it == m_datas.end() ? nullptr : it->second;
}

}