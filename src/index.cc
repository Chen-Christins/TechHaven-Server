#include "index.h"
#include "blog/data/article_category_rel_info.h"
#include <chen/log/log.h>

namespace blog {

static sylar::Logger::ptr logger = LOG_ROOT();

struct ParamArgsInfo {
	std::string name;
	uint64_t key;
	uint32_t type;
};

Index::Index()
	:m_createTime(0)
	,m_endTime(0) {
}

bool Index::set(uint64_t type, uint64_t key, uint32_t idx, bool v) {
	auto b = m_indexs[type][key];
	if (!b) {
		b.reset(new sylar::ds::Bitmap(m_docs.size()));
		m_indexs[type][key] = b;
	}
	b->set(idx, v);
	return true;
}

sylar::ds::Bitmap::ptr Index::get(uint64_t type, uint64_t key) {
	auto it = m_indexs.find(type);
	if (it == m_indexs.end()) {
		return nullptr;
	}
	auto itt = it->second.find(key);
	return itt == it->second.end() ? nullptr : itt->second;
}

void Index::build() {
}

void Index::buildIdx(data::ArticleInfo::ptr info, uint32_t idx) {
}

int32_t Index::search(std::vector<uint64_t>& ids, const std::map<uint64_t, std::set<uint64_t>>& params
		, uint32_t max_size) {
	return -1;
}

int32_t Index::property(std::map<uint64_t, std::map<uint64_t, uint64_t>>& props
		, const std::map<uint64_t, std::set<uint64_t>>& params
		, std::map<uint64_t, std::set<uint64_t>>& querys) {
	return -1;
}

uint64_t Index::StrHash(const std::string& str) {
	return -1;
}

std::string Index::toString() {
	return "";
}

std::string Index::getStr(uint64_t id) {
	return "";
}

sylar::ds::Bitmap::ptr Index::query(const std::map<uint64_t, std::set<uint64_t>>& params) {
	return nullptr;
}

uint64_t Index::hash(const std::string& str, bool save) {
	return -1;
}

void Index::buildWordIdx(const std::string& str, uint32_t idx) {
}


}