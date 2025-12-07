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
	data::AssignmentInfo::ptr get(int64_t id);

private:
	std::shared_mutex m_mutex;
	// 作业id -> data
	std::unordered_map<int64_t, data::AssignmentInfo::ptr> m_datas;
};

typedef chen::Singleton<AssignmentManager> AssignmentMgr;

}

#endif // __BLOG_MANAGER_ASSIGNMENT_MANAGER_H__