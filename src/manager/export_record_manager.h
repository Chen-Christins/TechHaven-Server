#pragma once

#include "blog/data/export_record_info.h"

#include <chen/ds/lru_cache.h>
#include <chen/db/query_builder.h>
#include <chen/util/singleton.h>

namespace blog {

class ExportRecordManager {
public:
    ExportRecordManager();

    void add(data::ExportRecordInfo::ptr info);
    data::ExportRecordInfo::ptr get(int64_t id);
    bool list(std::vector<data::ExportRecordInfo::ptr>& results, int64_t& total,
              const std::string& search, const std::string& type,
              const std::string& status, int32_t offset, int32_t limit);
    data::ExportRecordInfo::ptr create(int64_t uid, const std::string& type,
                                       const std::string& name);
    bool remove(int64_t id);

private:
    static data::ExportRecordInfo::ptr parseRow(chen::ISQLData::ptr rt);
    chen::ds::HashLruCache<int64_t, data::ExportRecordInfo::ptr> m_cache;
};

typedef chen::Singleton<ExportRecordManager> ExportRecordMgr;

}

