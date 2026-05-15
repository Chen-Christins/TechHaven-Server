#ifndef __BLOG_MANAGER_COMMENT_MANAGER_H__
#define __BLOG_MANAGER_COMMENT_MANAGER_H__

#include "blog/data/comment_info.h"
#include <chen/singleton.h>
#include <map>
#include <shared_mutex>
#include <unordered_map>

namespace blog {

class CommentManager {
public:
    enum Status { PENDING = 1, APPROVED = 2, REJECTED = 3, SPAM = 4 };

    bool loadAll();
    void add(data::CommentInfo::ptr info);
    data::CommentInfo::ptr get(int64_t id);

    // create a comment, returns the inserted info
    data::CommentInfo::ptr create(int64_t article_id, int64_t user_id, const std::string& content, int64_t parent_id,
                                  const std::string& ip, const std::string& user_agent);

    // update comment content
    bool update(int64_t id, const std::string& content);

    // soft-delete a comment
    bool del(int64_t id);

    // list top-level approved comments (parent_id == 0, status == APPROVED) for an article, latest first
    void listByArticle(std::vector<data::CommentInfo::ptr>& results, int64_t article_id, uint64_t offset,
                       uint64_t size);

    // list replies to a parent comment (approved only), earliest first
    void listReplies(std::vector<data::CommentInfo::ptr>& results, int64_t parent_id, uint64_t offset, uint64_t size);

    // count top-level approved comments for an article
    int64_t countByArticle(int64_t article_id);

    // count replies to a parent comment (approved only)
    int64_t countReplies(int64_t parent_id);

    // --- admin methods ---

    // list all comments with filtering and pagination
    int64_t listByAdmin(std::vector<data::CommentInfo::ptr>& results, int64_t page_num, int64_t page_size,
                        int32_t status, const std::string& keyword, int64_t article_id, int32_t is_reported);

    // batch update status
    int64_t batchUpdateStatus(const std::vector<int64_t>& ids, int32_t status);

    // batch delete (hard delete)
    int64_t batchDelete(const std::vector<int64_t>& ids);

    // get stats for dashboard
    struct CommentStats {
        int64_t total = 0;
        int64_t pending = 0;
        int64_t approved = 0;
        int64_t spam = 0;
        int64_t reported = 0;
    };
    CommentStats getStats();

private:
    std::shared_mutex m_mutex;
    std::unordered_map<int64_t, data::CommentInfo::ptr> m_datas;
    // article_id -> (id -> info), all comments for an article
    std::unordered_map<int64_t, std::map<int64_t, data::CommentInfo::ptr>> m_articleComments;
    // parent_id -> (id -> info), replies to a parent comment
    std::unordered_map<int64_t, std::map<int64_t, data::CommentInfo::ptr>> m_replies;
};

typedef chen::Singleton<CommentManager> CommentMgr;

} // namespace blog

#endif // __BLOG_MANAGER_COMMENT_MANAGER_H__
