#pragma once

#include "blog/data/help_faqs_info.h"

#include <chen/util/singleton.h>

namespace blog {

class FaqManager {
public:
    FaqManager();

    bool listAll(std::vector<data::HelpFaqsInfo::ptr>& infos);
    bool searchByKeyword(const std::string& keyword, std::vector<data::HelpFaqsInfo::ptr>& infos);
};

typedef chen::Singleton<FaqManager> FaqMgr;

}
