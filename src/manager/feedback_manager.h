#pragma once

#include "blog/data/user_feedback_info.h"

#include <chen/util/singleton.h>

namespace blog {

class FeedbackManager {
public:
    FeedbackManager();
};

typedef chen::Singleton<FeedbackManager> FeedbackMgr;

}
