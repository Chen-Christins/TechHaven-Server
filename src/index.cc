#include "index.h"
#include "blog/data/article_category_rel_info.h"
#include <chen/log/log.h>
#include "manager/article_manager.h"
#include "manager/article_category_rel_manager.h"
#include "manager/article_label_rel_manager.h"
#include "manager/category_manager.h"
#include "manager/label_manager.h"
#include <chen/util/hash_util.h>

namespace blog {

static chen::Logger::ptr logger = LOG_ROOT();

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
		b.reset(new chen::ds::Bitmap(m_docs.size()));
		m_indexs[type][key] = b;
	}
	b->set(idx, v);
	return true;
}

chen::ds::Bitmap::ptr Index::get(uint64_t type, uint64_t key) {
	auto it = m_indexs.find(type);
	if (it == m_indexs.end()) {
		return nullptr;
	}
	auto itt = it->second.find(key);
	return itt == it->second.end() ? nullptr : itt->second;
}

void Index::build() {
	INFO(logger) << "Index build begin...";
	m_createTime = time(0);
	std::vector<data::ArticleInfo::ptr> infos;
	ArticleMgr::GetInstance()->listByUserIdPages(infos, 0, 0, 0x7FFFFFFF, true, 0);
	std::sort(infos.begin(), infos.end(), [](const data::ArticleInfo::ptr a, const data::ArticleInfo::ptr b) {
		if (a->getWeight() != b->getWeight()) {
			return a->getWeight() > b->getWeight();
		}
		return a->getId() > b->getId();
	});
	m_docs.reserve(infos.size());
	for (auto& info : infos) {
		m_docs.emplace_back(info->getId());
	}
	for (size_t i = 0; i < infos.size(); ++i) {
		buildIdx(infos[i], i);
	}
	m_endTime = time(0);
	INFO(logger) << "Index build over... used=" 
		<< (m_endTime - m_createTime) << " doc.size=" << m_docs.size();	
}

void Index::buildIdx(data::ArticleInfo::ptr info, uint32_t idx) {
	set((uint64_t)IndexType::USER_ID, info->getUserId(), idx, true);
	set((uint64_t)IndexType::STATE, info->getState(), idx, true);
	set((uint64_t)IndexType::YEAR_MON, hash(chen::Time2Str(info->getPublishTime(), "%Y年%m月"), true), idx, true);
	set((uint64_t)IndexType::CHANNEL, info->getChannel(), idx, true);

	// TODO: 根据文章内容和文章标题建立索引

	std::vector<data::ArticleCategoryRelInfo::ptr> cats;
	ArticleCategoryRelMgr::GetInstance()->listByArticleId(cats, info->getId(), true);
	for (auto& i : cats) {
		auto cat = CategoryMgr::GetInstance()->get(i->getCategoryId());
		if (!cat) {
			ERROR(logger) << "invalid cat_id=" << i->getCategoryId();
			continue;
		}
		set((uint64_t)IndexType::CAT_ID, cat->getId(), idx, true);
		set((uint64_t)IndexType::CAT_NAME, hash(cat->getName().c_str(), true), idx, true);
	}

	std::vector<data::ArticleLabelRelInfo::ptr> labels;
	ArticleLabelRelMgr::GetInstance()->listByArticleId(labels, info->getId(), true);
	for (auto& i : labels) {
		auto label = LabelMgr::GetInstance()->get(i->getLabelId());
		if (!label) {
			ERROR(logger) << "invalid label_id=" << i->getLabelId();
			continue;
		}
		set((uint64_t)IndexType::LABEL_ID, label->getId(), idx, true);
		set((uint64_t)IndexType::LABEL_NAME, hash(label->getName().c_str(), true), idx, true);
	}
}

int32_t Index::search(std::vector<uint64_t>& ids, const std::map<uint64_t, std::set<uint64_t>>& params
		, uint32_t max_size) {
	auto v = query(params);
	if (!v) {
		return -1;
	}
	uint32_t i = 0;
	for (auto it = v->begin_new(); !*it && i < max_size; it->next(), ++i) {
		ids.emplace_back(m_docs[**it]);
	}
	return v->getCount();
}

int32_t Index::property(std::map<uint64_t, std::map<uint64_t, uint64_t>>& props
		, const std::map<uint64_t, std::set<uint64_t>>& params
		, std::map<uint64_t, std::set<uint64_t>>& querys) {
	// TODO: Index::property
	return -1;
}

uint64_t Index::StrHash(const std::string& str) {
	return chen::murmur3_hash64(chen::ToLower(str).c_str());
}

std::string Index::toString() {
	// TODO: Index::toString
	return "";
}

std::string Index::getStr(uint64_t id) {
	auto it = m_strings.find(id);
	return it == m_strings.end() ? "" : it->second;
}

chen::ds::Bitmap::ptr Index::query(const std::map<uint64_t, std::set<uint64_t>>& params) {
	// TODO: Index::query
	return nullptr;
}

uint64_t Index::hash(const std::string& str, bool save) {
	auto v = StrHash(str);
	if (save) {
		m_strings.insert(std::pair(v, str));
	}
	return v;
}

void Index::buildWordIdx(const std::string& str, uint32_t idx) {
	// TODO: Index::buildWordIdx
}


}