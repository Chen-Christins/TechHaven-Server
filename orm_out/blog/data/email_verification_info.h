#ifndef BLOG_DATAEMAIL_VERIFICATION_INFO_H
#define BLOG_DATAEMAIL_VERIFICATION_INFO_H

#include <json/json.h>
#include <vector>
#include "chen/db/db.h"
#include "chen/util/util.h"


namespace blog {
namespace data {

class EmailVerificationInfoDao;
class EmailVerificationInfo {
friend class EmailVerificationInfoDao;
public:
    typedef std::shared_ptr<EmailVerificationInfo> ptr;

    EmailVerificationInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const std::string& getEmail() { return m_email; }
    void setEmail(const std::string& v);

    const std::string& getCode() { return m_code; }
    void setCode(const std::string& v);

    const std::string& getType() { return m_type; }
    void setType(const std::string& v);

    const int32_t& getState() { return m_state; }
    void setState(const int32_t& v);

    const int64_t& getCreateTime() { return m_createTime; }
    void setCreateTime(const int64_t& v);

    const int64_t& getExpiresTime() { return m_expiresTime; }
    void setExpiresTime(const int64_t& v);

    const std::string& getClientIp() { return m_clientIp; }
    void setClientIp(const std::string& v);

    const std::string& getUserAgent() { return m_userAgent; }
    void setUserAgent(const std::string& v);

    std::string toJsonString() const;

private:
    int32_t m_state;
    int64_t m_id;
    std::string m_email;
    std::string m_code;
    std::string m_type;
    std::string m_clientIp;
    std::string m_userAgent;
    int64_t m_createTime;
    int64_t m_expiresTime;
};


class EmailVerificationInfoDao {
public:
    typedef std::shared_ptr<EmailVerificationInfoDao> ptr;
    static int Update(EmailVerificationInfo::ptr info, sylar::IDB::ptr conn);
    static int Insert(EmailVerificationInfo::ptr info, sylar::IDB::ptr conn);
    static int InsertOrUpdate(EmailVerificationInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(EmailVerificationInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(const int64_t& id, sylar::IDB::ptr conn);
    static int DeleteById( const int64_t& id, sylar::IDB::ptr conn);
    static int DeleteByEmailCode( const std::string& email,  const std::string& code, sylar::IDB::ptr conn);
    static int DeleteByEmailType( const std::string& email,  const std::string& type, sylar::IDB::ptr conn);
    static int DeleteByExpiresTime( const int64_t& expires_time, sylar::IDB::ptr conn);
    static int DeleteByCreateTime( const int64_t& create_time, sylar::IDB::ptr conn);
    static int QueryAll(std::vector<EmailVerificationInfo::ptr>& results, sylar::IDB::ptr conn);
    static EmailVerificationInfo::ptr Query( const int64_t& id, sylar::IDB::ptr conn);
    static int QueryByEmailCode(std::vector<EmailVerificationInfo::ptr>& results,  const std::string& email,  const std::string& code, sylar::IDB::ptr conn);
    static int QueryByEmailType(std::vector<EmailVerificationInfo::ptr>& results,  const std::string& email,  const std::string& type, sylar::IDB::ptr conn);
    static int QueryByExpiresTime(std::vector<EmailVerificationInfo::ptr>& results,  const int64_t& expires_time, sylar::IDB::ptr conn);
    static int QueryByCreateTime(std::vector<EmailVerificationInfo::ptr>& results,  const int64_t& create_time, sylar::IDB::ptr conn);
    static int CreateTableSQLite3(sylar::IDB::ptr info);
    static int CreateTableMySQL(sylar::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //BLOG_DATAEMAIL_VERIFICATION_INFO_H
