#ifndef __BLOG_MANAGER_SUBJECT_MANAGER_H__
#define __BLOG_MANAGER_SUBJECT_MANAGER_H__

#include <memory>
#include <unordered_map>
#include "blog/data/subject_info.h"
#include <shared_mutex>
#include <chen/singleton.h>

namespace blog {

class SubjectManager {
public:
	typedef std::shared_ptr<SubjectManager> ptr;
	
	bool loadAll();
	void add(blog::data::SubjectInfo::ptr v);
	bool listAll(std::vector<data::SubjectInfo::ptr>& infos, bool valid);
	blog::data::SubjectInfo::ptr get(int64_t id);
	blog::data::SubjectInfo::ptr getByName(const std::string& v);

private:
	std::unordered_map<int64_t, data::SubjectInfo::ptr> m_datas;
	std::unordered_map<std::string, data::SubjectInfo::ptr> m_names;
	std::shared_mutex m_mutex;
};

typedef chen::Singleton<SubjectManager> SubjectMgr;

}

#endif // __BLOG_MANAGER_SUBJECT_MANAGER_H__