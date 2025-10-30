#include "subject_manager.h"
#include <chen/log/log.h>
#include "../util.h"

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

bool SubjectManager::loadAll() {
	auto db = GetDB();
	if (!db) {
        ERROR(logger) << "get db connection fail";
        return false;
    }
	std::vector<data::SubjectInfo::ptr> results;
    if (data::SubjectInfoDao::QueryAll(results, db)) {
        ERROR(logger) << "SubjectManager loadAll fail";
        return false;
    }

	std::unordered_map<int64_t, data::SubjectInfo::ptr> datas;
	std::unordered_map<std::string, data::SubjectInfo::ptr> names;
	for (auto& i : results) {
		datas[i->getId()] = i;
		names[i->getName()] = i;
	}

	std::unique_lock<std::shared_mutex> lock(m_mutex);
	m_datas.swap(datas);
	m_names.swap(names);

	return true;
}

bool SubjectManager::listAll(std::vector<data::SubjectInfo::ptr>& infos, bool valid) {
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

void SubjectManager::add(data::SubjectInfo::ptr v) {
	std::unique_lock<std::shared_mutex> lock(m_mutex);
	m_datas[v->getId()] = v;
	m_names[v->getName()] = v;
}

data::SubjectInfo::ptr SubjectManager::get(int64_t id) {
	std::shared_lock<std::shared_mutex> lock(m_mutex);
	auto it = m_datas.find(id);
	return it == m_datas.end() ? nullptr : it->second;
}

data::SubjectInfo::ptr SubjectManager::getByName(const std::string& v) {
	std::shared_lock<std::shared_mutex> lock(m_mutex);
	auto it = m_names.find(v);
	return it == m_names.end() ? nullptr : it->second;
}


}