#ifndef BLOG_DATASYSTEM_SETTINGS_INFO_H
#define BLOG_DATASYSTEM_SETTINGS_INFO_H

#include <json/json.h>
#include <vector>
#include "chen/db/db.h"
#include "chen/util/util.h"


namespace blog {
namespace data {

class SystemSettingsInfoDao;
class SystemSettingsInfo {
friend class SystemSettingsInfoDao;
public:
    typedef std::shared_ptr<SystemSettingsInfo> ptr;

    SystemSettingsInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const std::string& getSiteName() { return m_siteName; }
    void setSiteName(const std::string& v);

    const std::string& getSiteDescription() { return m_siteDescription; }
    void setSiteDescription(const std::string& v);

    const std::string& getSiteKeywords() { return m_siteKeywords; }
    void setSiteKeywords(const std::string& v);

    const std::string& getSiteIcon() { return m_siteIcon; }
    void setSiteIcon(const std::string& v);

    const std::string& getSiteLogo() { return m_siteLogo; }
    void setSiteLogo(const std::string& v);

    const std::string& getFavicon() { return m_favicon; }
    void setFavicon(const std::string& v);

    const std::string& getAdminEmail() { return m_adminEmail; }
    void setAdminEmail(const std::string& v);

    const std::string& getTimezone() { return m_timezone; }
    void setTimezone(const std::string& v);

    const std::string& getLanguage() { return m_language; }
    void setLanguage(const std::string& v);

    const std::string& getSmtpHost() { return m_smtpHost; }
    void setSmtpHost(const std::string& v);

    const int32_t& getSmtpPort() { return m_smtpPort; }
    void setSmtpPort(const int32_t& v);

    const std::string& getSmtpUsername() { return m_smtpUsername; }
    void setSmtpUsername(const std::string& v);

    const std::string& getSmtpPassword() { return m_smtpPassword; }
    void setSmtpPassword(const std::string& v);

    const std::string& getSmtpEncryption() { return m_smtpEncryption; }
    void setSmtpEncryption(const std::string& v);

    const std::string& getFromEmail() { return m_fromEmail; }
    void setFromEmail(const std::string& v);

    const std::string& getFromName() { return m_fromName; }
    void setFromName(const std::string& v);

    const std::string& getReplyTo() { return m_replyTo; }
    void setReplyTo(const std::string& v);

    const int32_t& getEnableRegistration() { return m_enableRegistration; }
    void setEnableRegistration(const int32_t& v);

    const int32_t& getRequireEmailVerification() { return m_requireEmailVerification; }
    void setRequireEmailVerification(const int32_t& v);

    const int32_t& getAllowComments() { return m_allowComments; }
    void setAllowComments(const int32_t& v);

    const int32_t& getModerateComments() { return m_moderateComments; }
    void setModerateComments(const int32_t& v);

    const int32_t& getMaxFileSize() { return m_maxFileSize; }
    void setMaxFileSize(const int32_t& v);

    const std::string& getAllowedFileTypes() { return m_allowedFileTypes; }
    void setAllowedFileTypes(const std::string& v);

    const int32_t& getSessionTimeout() { return m_sessionTimeout; }
    void setSessionTimeout(const int32_t& v);

    const int32_t& getMaintenanceMode() { return m_maintenanceMode; }
    void setMaintenanceMode(const int32_t& v);

    const std::string& getBackupSchedule() { return m_backupSchedule; }
    void setBackupSchedule(const std::string& v);

    const int64_t& getCreatedAt() { return m_createdAt; }
    void setCreatedAt(const int64_t& v);

    const int64_t& getUpdatedAt() { return m_updatedAt; }
    void setUpdatedAt(const int64_t& v);

    std::string toJsonString() const;

private:
    int32_t m_smtpPort;
    int32_t m_enableRegistration;
    int32_t m_requireEmailVerification;
    int32_t m_allowComments;
    int32_t m_moderateComments;
    int32_t m_maxFileSize;
    int32_t m_sessionTimeout;
    int32_t m_maintenanceMode;
    int64_t m_id;
    std::string m_siteName;
    std::string m_siteDescription;
    std::string m_siteKeywords;
    std::string m_siteIcon;
    std::string m_siteLogo;
    std::string m_favicon;
    std::string m_adminEmail;
    std::string m_timezone;
    std::string m_language;
    std::string m_smtpHost;
    std::string m_smtpUsername;
    std::string m_smtpPassword;
    std::string m_smtpEncryption;
    std::string m_fromEmail;
    std::string m_fromName;
    std::string m_replyTo;
    std::string m_allowedFileTypes;
    std::string m_backupSchedule;
    int64_t m_createdAt;
    int64_t m_updatedAt;
};


class SystemSettingsInfoDao {
public:
    typedef std::shared_ptr<SystemSettingsInfoDao> ptr;
    static int Update(SystemSettingsInfo::ptr info, chen::IDB::ptr conn);
    static int Insert(SystemSettingsInfo::ptr info, chen::IDB::ptr conn);
    static int InsertOrUpdate(SystemSettingsInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(SystemSettingsInfo::ptr info, chen::IDB::ptr conn);
    static int Delete(const int64_t& id, chen::IDB::ptr conn);
    static int DeleteById( const int64_t& id, chen::IDB::ptr conn);
    static int QueryAll(std::vector<SystemSettingsInfo::ptr>& results, chen::IDB::ptr conn);
    static SystemSettingsInfo::ptr Query( const int64_t& id, chen::IDB::ptr conn);
    static int CreateTableSQLite3(chen::IDB::ptr info);
    static int CreateTableMySQL(chen::IDB::ptr info);
    static int MigrateTableSQLite3(chen::IDB::ptr info);
    static int MigrateTableMySQL(chen::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //BLOG_DATASYSTEM_SETTINGS_INFO_H
