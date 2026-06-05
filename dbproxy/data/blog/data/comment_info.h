#ifndef BLOG_DATACOMMENT_INFO_H
#define BLOG_DATACOMMENT_INFO_H

#include <json/json.h>
#include <vector>
#include "chen/db/db.h"
#include "chen/util/util.h"


namespace blog {
namespace data {

class CommentInfoDao;
class CommentInfo {
friend class CommentInfoDao;
public:
    typedef std::shared_ptr<CommentInfo> ptr;

    CommentInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const int64_t& getArticleId() { return m_articleId; }
    void setArticleId(const int64_t& v);

    const int64_t& getUserId() { return m_userId; }
    void setUserId(const int64_t& v);

    const int64_t& getParentId() { return m_parentId; }
    void setParentId(const int64_t& v);

    const std::string& getContent() { return m_content; }
    void setContent(const std::string& v);

    const std::string& getIp() { return m_ip; }
    void setIp(const std::string& v);

    const std::string& getUserAgent() { return m_userAgent; }
    void setUserAgent(const std::string& v);

    const int32_t& getStatus() { return m_status; }
    void setStatus(const int32_t& v);

    const int32_t& getIsReported() { return m_isReported; }
    void setIsReported(const int32_t& v);

    const int32_t& getReportCount() { return m_reportCount; }
    void setReportCount(const int32_t& v);

    const int32_t& getIsDeleted() { return m_isDeleted; }
    void setIsDeleted(const int32_t& v);

    const int64_t& getCreateTime() { return m_createTime; }
    void setCreateTime(const int64_t& v);

    const int64_t& getUpdateTime() { return m_updateTime; }
    void setUpdateTime(const int64_t& v);

    std::string toJsonString() const;

private:
    int32_t m_status;
    int32_t m_isReported;
    int32_t m_reportCount;
    int32_t m_isDeleted;
    int64_t m_id;
    int64_t m_articleId;
    int64_t m_userId;
    int64_t m_parentId;
    std::string m_ip;
    std::string m_userAgent;
    std::string m_content;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class CommentInfoDao {
public:
    typedef std::shared_ptr<CommentInfoDao> ptr;
    static int Update(CommentInfo::ptr info, chen::IDB::ptr conn);
    static int Insert(CommentInfo::ptr info, chen::IDB::ptr conn);
    static int InsertOrUpdate(CommentInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(CommentInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(const int64_t& id, chen::IDB::ptr conn);
    static int DeleteById( const int64_t& id, chen::IDB::ptr conn);
    static int DeleteByArticleId( const int64_t& article_id, chen::IDB::ptr conn);
    static int DeleteByUserId( const int64_t& user_id, chen::IDB::ptr conn);
    static int DeleteByParentId( const int64_t& parent_id, chen::IDB::ptr conn);
    static int DeleteByStatus( const int32_t& status, chen::IDB::ptr conn);
    static int QueryAll(std::vector<CommentInfo::ptr>& results, chen::IDB::ptr conn);
    static CommentInfo::ptr Query( const int64_t& id, chen::IDB::ptr conn);
    static int QueryByArticleId(std::vector<CommentInfo::ptr>& results,  const int64_t& article_id, chen::IDB::ptr conn);
    static int QueryByArticleIdPages(std::vector<CommentInfo::ptr>& results, int64_t& total,  const int64_t& article_id, int32_t offset, int32_t limit, chen::IDB::ptr conn);
    static int QueryByUserId(std::vector<CommentInfo::ptr>& results,  const int64_t& user_id, chen::IDB::ptr conn);
    static int QueryByUserIdPages(std::vector<CommentInfo::ptr>& results, int64_t& total,  const int64_t& user_id, int32_t offset, int32_t limit, chen::IDB::ptr conn);
    static int QueryByParentId(std::vector<CommentInfo::ptr>& results,  const int64_t& parent_id, chen::IDB::ptr conn);
    static int QueryByParentIdPages(std::vector<CommentInfo::ptr>& results, int64_t& total,  const int64_t& parent_id, int32_t offset, int32_t limit, chen::IDB::ptr conn);
    static int QueryByStatus(std::vector<CommentInfo::ptr>& results,  const int32_t& status, chen::IDB::ptr conn);
    static int QueryByStatusPages(std::vector<CommentInfo::ptr>& results, int64_t& total,  const int32_t& status, int32_t offset, int32_t limit, chen::IDB::ptr conn);
    static int CreateTableSQLite3(chen::IDB::ptr info);
    static int CreateTableMySQL(chen::IDB::ptr info);
    static int MigrateTableSQLite3(chen::IDB::ptr info);
    static int MigrateTableMySQL(chen::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //BLOG_DATACOMMENT_INFO_H
