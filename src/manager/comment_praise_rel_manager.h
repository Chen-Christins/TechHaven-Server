#ifndef __BLOG_MANAGER_COMMENT_PRAISE_REL_MANAGER_H__
#define __BLOG_MANAGER_COMMENT_PRAISE_REL_MANAGER_H__

#include <shared_mutex>
#include <unordered_map>
#include <map>
#include "blog/data/comment_praise_rel_info.h"
#include <chen/singleton.h>

namespace blog {

class CommentPraiseRelManager {
public:
    bool loadAll();
    void add(data::CommentPraiseRelInfo::ptr info);
    data::CommentPraiseRelInfo::ptr get(int64_t id);
    data::CommentPraiseRelInfo::ptr getByUserAndComment(int64_t user_id, int64_t comment_id);

    // praise/unpraise
    data::CommentPraiseRelInfo::ptr praise(int64_t user_id, int64_t comment_id);
    bool unpraise(int64_t user_id, int64_t comment_id);
    bool isPraising(int64_t user_id, int64_t comment_id);

    // count praises for a comment
    int64_t countByComment(int64_t comment_id);

private:
    std::shared_mutex m_mutex;
    std::unordered_map<int64_t, data::CommentPraiseRelInfo::ptr> m_datas;
    // user_id -> (comment_id -> info)
    std::unordered_map<int64_t, std::map<int64_t, data::CommentPraiseRelInfo::ptr>> m_userPraises;
    // comment_id -> (user_id -> info)
    std::unordered_map<int64_t, std::map<int64_t, data::CommentPraiseRelInfo::ptr>> m_commentPraises;
};

typedef chen::Singleton<CommentPraiseRelManager> CommentPraiseRelMgr;

}

#endif // __BLOG_MANAGER_COMMENT_PRAISE_REL_MANAGER_H__
