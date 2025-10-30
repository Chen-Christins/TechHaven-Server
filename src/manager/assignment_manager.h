#ifndef __BLOG_MANAGER_ASSIGNMENT_MANAGER_H__
#define __BLOG_MANAGER_ASSIGNMENT_MANAGER_H__

#include <memory>
#include <unordered_map>
#include "blog/data/assignment_info.h"
#include <shared_mutex>
#include <chen/singleton.h>

namespace blog {

class AssignmentManager {
public:
	typedef std::shared_ptr<AssignmentManager> ptr;

	bool loadAll();
	void add(data::AssignmentInfo::ptr info);
	bool listAll(std::vector<data::AssignmentInfo::ptr>& infos, bool valid);
	data::AssignmentInfo::ptr getBySubjectIdName(int64_t id, const std::string& name);
	data::AssignmentInfo::ptr get(int64_t id);

private:
	std::shared_mutex m_mutex;
	// 作业id -> data
	std::unordered_map<int64_t, data::AssignmentInfo::ptr> m_datas;
	// 科目id -> [作业名称, data]
	std::unordered_map<int64_t, std::unordered_map<std::string, data::AssignmentInfo::ptr>> m_subject_names;
};

typedef chen::Singleton<AssignmentManager> AssignmentMgr;

}

#endif // __BLOG_MANAGER_ASSIGNMENT_MANAGER_H__