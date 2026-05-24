#include "system_settings_info.h"
#include "chen/log/log.h"
#include <set>

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

SystemSettingsInfo::SystemSettingsInfo()
    :m_smtpPort(587)
    ,m_enableRegistration(1)
    ,m_requireEmailVerification(1)
    ,m_allowComments(1)
    ,m_moderateComments(0)
    ,m_maxFileSize(10)
    ,m_sessionTimeout(24)
    ,m_maintenanceMode(0)
    ,m_id()
    ,m_siteName("TechBlog")
    ,m_siteDescription()
    ,m_siteKeywords()
    ,m_siteIcon()
    ,m_siteLogo()
    ,m_favicon()
    ,m_adminEmail()
    ,m_timezone("Asia/Shanghai")
    ,m_language("zh-CN")
    ,m_smtpHost()
    ,m_smtpUsername()
    ,m_smtpPassword()
    ,m_smtpEncryption("tls")
    ,m_fromEmail()
    ,m_fromName()
    ,m_replyTo()
    ,m_allowedFileTypes("jpg,jpeg,png,gif,pdf,doc,docx")
    ,m_backupSchedule("daily")
    ,m_createdAt(time(0))
    ,m_updatedAt(time(0)) {
}

std::string SystemSettingsInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["site_name"] = m_siteName;
    v["site_description"] = m_siteDescription;
    v["site_keywords"] = m_siteKeywords;
    v["site_icon"] = m_siteIcon;
    v["site_logo"] = m_siteLogo;
    v["favicon"] = m_favicon;
    v["admin_email"] = m_adminEmail;
    v["timezone"] = m_timezone;
    v["language"] = m_language;
    v["smtp_host"] = m_smtpHost;
    v["smtp_port"] = m_smtpPort;
    v["smtp_username"] = m_smtpUsername;
    v["smtp_password"] = m_smtpPassword;
    v["smtp_encryption"] = m_smtpEncryption;
    v["from_email"] = m_fromEmail;
    v["from_name"] = m_fromName;
    v["reply_to"] = m_replyTo;
    v["enable_registration"] = m_enableRegistration;
    v["require_email_verification"] = m_requireEmailVerification;
    v["allow_comments"] = m_allowComments;
    v["moderate_comments"] = m_moderateComments;
    v["max_file_size"] = m_maxFileSize;
    v["allowed_file_types"] = m_allowedFileTypes;
    v["session_timeout"] = m_sessionTimeout;
    v["maintenance_mode"] = m_maintenanceMode;
    v["backup_schedule"] = m_backupSchedule;
    v["created_at"] = chen::Time2Str(m_createdAt);
    v["updated_at"] = chen::Time2Str(m_updatedAt);
    return chen::JsonUtil::ToString(v);
}

void SystemSettingsInfo::setId(const int64_t& v) {
    m_id = v;
}

void SystemSettingsInfo::setSiteName(const std::string& v) {
    m_siteName = v;
}

void SystemSettingsInfo::setSiteDescription(const std::string& v) {
    m_siteDescription = v;
}

void SystemSettingsInfo::setSiteKeywords(const std::string& v) {
    m_siteKeywords = v;
}

void SystemSettingsInfo::setSiteIcon(const std::string& v) {
    m_siteIcon = v;
}

void SystemSettingsInfo::setSiteLogo(const std::string& v) {
    m_siteLogo = v;
}

void SystemSettingsInfo::setFavicon(const std::string& v) {
    m_favicon = v;
}

void SystemSettingsInfo::setAdminEmail(const std::string& v) {
    m_adminEmail = v;
}

void SystemSettingsInfo::setTimezone(const std::string& v) {
    m_timezone = v;
}

void SystemSettingsInfo::setLanguage(const std::string& v) {
    m_language = v;
}

void SystemSettingsInfo::setSmtpHost(const std::string& v) {
    m_smtpHost = v;
}

void SystemSettingsInfo::setSmtpPort(const int32_t& v) {
    m_smtpPort = v;
}

void SystemSettingsInfo::setSmtpUsername(const std::string& v) {
    m_smtpUsername = v;
}

void SystemSettingsInfo::setSmtpPassword(const std::string& v) {
    m_smtpPassword = v;
}

void SystemSettingsInfo::setSmtpEncryption(const std::string& v) {
    m_smtpEncryption = v;
}

void SystemSettingsInfo::setFromEmail(const std::string& v) {
    m_fromEmail = v;
}

void SystemSettingsInfo::setFromName(const std::string& v) {
    m_fromName = v;
}

void SystemSettingsInfo::setReplyTo(const std::string& v) {
    m_replyTo = v;
}

void SystemSettingsInfo::setEnableRegistration(const int32_t& v) {
    m_enableRegistration = v;
}

void SystemSettingsInfo::setRequireEmailVerification(const int32_t& v) {
    m_requireEmailVerification = v;
}

void SystemSettingsInfo::setAllowComments(const int32_t& v) {
    m_allowComments = v;
}

void SystemSettingsInfo::setModerateComments(const int32_t& v) {
    m_moderateComments = v;
}

void SystemSettingsInfo::setMaxFileSize(const int32_t& v) {
    m_maxFileSize = v;
}

void SystemSettingsInfo::setAllowedFileTypes(const std::string& v) {
    m_allowedFileTypes = v;
}

void SystemSettingsInfo::setSessionTimeout(const int32_t& v) {
    m_sessionTimeout = v;
}

void SystemSettingsInfo::setMaintenanceMode(const int32_t& v) {
    m_maintenanceMode = v;
}

void SystemSettingsInfo::setBackupSchedule(const std::string& v) {
    m_backupSchedule = v;
}

void SystemSettingsInfo::setCreatedAt(const int64_t& v) {
    m_createdAt = v;
}

void SystemSettingsInfo::setUpdatedAt(const int64_t& v) {
    m_updatedAt = v;
}


int SystemSettingsInfoDao::Update(SystemSettingsInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update system_settings set site_name = ?, site_description = ?, site_keywords = ?, site_icon = ?, site_logo = ?, favicon = ?, admin_email = ?, timezone = ?, language = ?, smtp_host = ?, smtp_port = ?, smtp_username = ?, smtp_password = ?, smtp_encryption = ?, from_email = ?, from_name = ?, reply_to = ?, enable_registration = ?, require_email_verification = ?, allow_comments = ?, moderate_comments = ?, max_file_size = ?, allowed_file_types = ?, session_timeout = ?, maintenance_mode = ?, backup_schedule = ?, created_at = ?, updated_at = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_siteName);
    stmt->bindString(2, info->m_siteDescription);
    stmt->bindString(3, info->m_siteKeywords);
    stmt->bindString(4, info->m_siteIcon);
    stmt->bindString(5, info->m_siteLogo);
    stmt->bindString(6, info->m_favicon);
    stmt->bindString(7, info->m_adminEmail);
    stmt->bindString(8, info->m_timezone);
    stmt->bindString(9, info->m_language);
    stmt->bindString(10, info->m_smtpHost);
    stmt->bindInt32(11, info->m_smtpPort);
    stmt->bindString(12, info->m_smtpUsername);
    stmt->bindString(13, info->m_smtpPassword);
    stmt->bindString(14, info->m_smtpEncryption);
    stmt->bindString(15, info->m_fromEmail);
    stmt->bindString(16, info->m_fromName);
    stmt->bindString(17, info->m_replyTo);
    stmt->bindInt32(18, info->m_enableRegistration);
    stmt->bindInt32(19, info->m_requireEmailVerification);
    stmt->bindInt32(20, info->m_allowComments);
    stmt->bindInt32(21, info->m_moderateComments);
    stmt->bindInt32(22, info->m_maxFileSize);
    stmt->bindString(23, info->m_allowedFileTypes);
    stmt->bindInt32(24, info->m_sessionTimeout);
    stmt->bindInt32(25, info->m_maintenanceMode);
    stmt->bindString(26, info->m_backupSchedule);
    stmt->bindTime(27, info->m_createdAt);
    stmt->bindTime(28, info->m_updatedAt);
    stmt->bindInt64(29, info->m_id);
    return stmt->execute();
}

int SystemSettingsInfoDao::Insert(SystemSettingsInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into system_settings (site_name, site_description, site_keywords, site_icon, site_logo, favicon, admin_email, timezone, language, smtp_host, smtp_port, smtp_username, smtp_password, smtp_encryption, from_email, from_name, reply_to, enable_registration, require_email_verification, allow_comments, moderate_comments, max_file_size, allowed_file_types, session_timeout, maintenance_mode, backup_schedule, created_at, updated_at) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_siteName);
    stmt->bindString(2, info->m_siteDescription);
    stmt->bindString(3, info->m_siteKeywords);
    stmt->bindString(4, info->m_siteIcon);
    stmt->bindString(5, info->m_siteLogo);
    stmt->bindString(6, info->m_favicon);
    stmt->bindString(7, info->m_adminEmail);
    stmt->bindString(8, info->m_timezone);
    stmt->bindString(9, info->m_language);
    stmt->bindString(10, info->m_smtpHost);
    stmt->bindInt32(11, info->m_smtpPort);
    stmt->bindString(12, info->m_smtpUsername);
    stmt->bindString(13, info->m_smtpPassword);
    stmt->bindString(14, info->m_smtpEncryption);
    stmt->bindString(15, info->m_fromEmail);
    stmt->bindString(16, info->m_fromName);
    stmt->bindString(17, info->m_replyTo);
    stmt->bindInt32(18, info->m_enableRegistration);
    stmt->bindInt32(19, info->m_requireEmailVerification);
    stmt->bindInt32(20, info->m_allowComments);
    stmt->bindInt32(21, info->m_moderateComments);
    stmt->bindInt32(22, info->m_maxFileSize);
    stmt->bindString(23, info->m_allowedFileTypes);
    stmt->bindInt32(24, info->m_sessionTimeout);
    stmt->bindInt32(25, info->m_maintenanceMode);
    stmt->bindString(26, info->m_backupSchedule);
    stmt->bindTime(27, info->m_createdAt);
    stmt->bindTime(28, info->m_updatedAt);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int SystemSettingsInfoDao::InsertOrUpdate(SystemSettingsInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into system_settings (id, site_name, site_description, site_keywords, site_icon, site_logo, favicon, admin_email, timezone, language, smtp_host, smtp_port, smtp_username, smtp_password, smtp_encryption, from_email, from_name, reply_to, enable_registration, require_email_verification, allow_comments, moderate_comments, max_file_size, allowed_file_types, session_timeout, maintenance_mode, backup_schedule, created_at, updated_at) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindString(2, info->m_siteName);
    stmt->bindString(3, info->m_siteDescription);
    stmt->bindString(4, info->m_siteKeywords);
    stmt->bindString(5, info->m_siteIcon);
    stmt->bindString(6, info->m_siteLogo);
    stmt->bindString(7, info->m_favicon);
    stmt->bindString(8, info->m_adminEmail);
    stmt->bindString(9, info->m_timezone);
    stmt->bindString(10, info->m_language);
    stmt->bindString(11, info->m_smtpHost);
    stmt->bindInt32(12, info->m_smtpPort);
    stmt->bindString(13, info->m_smtpUsername);
    stmt->bindString(14, info->m_smtpPassword);
    stmt->bindString(15, info->m_smtpEncryption);
    stmt->bindString(16, info->m_fromEmail);
    stmt->bindString(17, info->m_fromName);
    stmt->bindString(18, info->m_replyTo);
    stmt->bindInt32(19, info->m_enableRegistration);
    stmt->bindInt32(20, info->m_requireEmailVerification);
    stmt->bindInt32(21, info->m_allowComments);
    stmt->bindInt32(22, info->m_moderateComments);
    stmt->bindInt32(23, info->m_maxFileSize);
    stmt->bindString(24, info->m_allowedFileTypes);
    stmt->bindInt32(25, info->m_sessionTimeout);
    stmt->bindInt32(26, info->m_maintenanceMode);
    stmt->bindString(27, info->m_backupSchedule);
    stmt->bindTime(28, info->m_createdAt);
    stmt->bindTime(29, info->m_updatedAt);
    return stmt->execute();
}

int SystemSettingsInfoDao::Delete(SystemSettingsInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from system_settings where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int SystemSettingsInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from system_settings where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int SystemSettingsInfoDao::QueryAll(std::vector<SystemSettingsInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, site_name, site_description, site_keywords, site_icon, site_logo, favicon, admin_email, timezone, language, smtp_host, smtp_port, smtp_username, smtp_password, smtp_encryption, from_email, from_name, reply_to, enable_registration, require_email_verification, allow_comments, moderate_comments, max_file_size, allowed_file_types, session_timeout, maintenance_mode, backup_schedule, created_at, updated_at from system_settings";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    auto rt = stmt->query();
    if(!rt) {
        return stmt->getErrno();
    }
    while (rt->next()) {
        SystemSettingsInfo::ptr v(new SystemSettingsInfo);
        v->m_id = rt->getInt64(0);
        v->m_siteName = rt->getString(1);
        v->m_siteDescription = rt->getString(2);
        v->m_siteKeywords = rt->getString(3);
        v->m_siteIcon = rt->getString(4);
        v->m_siteLogo = rt->getString(5);
        v->m_favicon = rt->getString(6);
        v->m_adminEmail = rt->getString(7);
        v->m_timezone = rt->getString(8);
        v->m_language = rt->getString(9);
        v->m_smtpHost = rt->getString(10);
        v->m_smtpPort = rt->getInt32(11);
        v->m_smtpUsername = rt->getString(12);
        v->m_smtpPassword = rt->getString(13);
        v->m_smtpEncryption = rt->getString(14);
        v->m_fromEmail = rt->getString(15);
        v->m_fromName = rt->getString(16);
        v->m_replyTo = rt->getString(17);
        v->m_enableRegistration = rt->getInt32(18);
        v->m_requireEmailVerification = rt->getInt32(19);
        v->m_allowComments = rt->getInt32(20);
        v->m_moderateComments = rt->getInt32(21);
        v->m_maxFileSize = rt->getInt32(22);
        v->m_allowedFileTypes = rt->getString(23);
        v->m_sessionTimeout = rt->getInt32(24);
        v->m_maintenanceMode = rt->getInt32(25);
        v->m_backupSchedule = rt->getString(26);
        v->m_createdAt = rt->getTime(27);
        v->m_updatedAt = rt->getTime(28);
        results.push_back(v);
    }
    return 0;
}

SystemSettingsInfo::ptr SystemSettingsInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, site_name, site_description, site_keywords, site_icon, site_logo, favicon, admin_email, timezone, language, smtp_host, smtp_port, smtp_username, smtp_password, smtp_encryption, from_email, from_name, reply_to, enable_registration, require_email_verification, allow_comments, moderate_comments, max_file_size, allowed_file_types, session_timeout, maintenance_mode, backup_schedule, created_at, updated_at from system_settings where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindInt64(1, id);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    SystemSettingsInfo::ptr v(new SystemSettingsInfo);
    v->m_id = rt->getInt64(0);
    v->m_siteName = rt->getString(1);
    v->m_siteDescription = rt->getString(2);
    v->m_siteKeywords = rt->getString(3);
    v->m_siteIcon = rt->getString(4);
    v->m_siteLogo = rt->getString(5);
    v->m_favicon = rt->getString(6);
    v->m_adminEmail = rt->getString(7);
    v->m_timezone = rt->getString(8);
    v->m_language = rt->getString(9);
    v->m_smtpHost = rt->getString(10);
    v->m_smtpPort = rt->getInt32(11);
    v->m_smtpUsername = rt->getString(12);
    v->m_smtpPassword = rt->getString(13);
    v->m_smtpEncryption = rt->getString(14);
    v->m_fromEmail = rt->getString(15);
    v->m_fromName = rt->getString(16);
    v->m_replyTo = rt->getString(17);
    v->m_enableRegistration = rt->getInt32(18);
    v->m_requireEmailVerification = rt->getInt32(19);
    v->m_allowComments = rt->getInt32(20);
    v->m_moderateComments = rt->getInt32(21);
    v->m_maxFileSize = rt->getInt32(22);
    v->m_allowedFileTypes = rt->getString(23);
    v->m_sessionTimeout = rt->getInt32(24);
    v->m_maintenanceMode = rt->getInt32(25);
    v->m_backupSchedule = rt->getString(26);
    v->m_createdAt = rt->getTime(27);
    v->m_updatedAt = rt->getTime(28);
    return v;
}

int SystemSettingsInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS system_settings("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "site_name TEXT NOT NULL DEFAULT 'TechBlog',"
            "site_description TEXT NOT NULL DEFAULT '',"
            "site_keywords TEXT NOT NULL DEFAULT '',"
            "site_icon TEXT NOT NULL DEFAULT '',"
            "site_logo TEXT NOT NULL DEFAULT '',"
            "favicon TEXT NOT NULL DEFAULT '',"
            "admin_email TEXT NOT NULL DEFAULT '',"
            "timezone TEXT NOT NULL DEFAULT 'Asia/Shanghai',"
            "language TEXT NOT NULL DEFAULT 'zh-CN',"
            "smtp_host TEXT NOT NULL DEFAULT '',"
            "smtp_port INTEGER NOT NULL DEFAULT 587,"
            "smtp_username TEXT NOT NULL DEFAULT '',"
            "smtp_password TEXT NOT NULL DEFAULT '',"
            "smtp_encryption TEXT NOT NULL DEFAULT 'tls',"
            "from_email TEXT NOT NULL DEFAULT '',"
            "from_name TEXT NOT NULL DEFAULT '',"
            "reply_to TEXT NOT NULL DEFAULT '',"
            "enable_registration INTEGER NOT NULL DEFAULT 1,"
            "require_email_verification INTEGER NOT NULL DEFAULT 1,"
            "allow_comments INTEGER NOT NULL DEFAULT 1,"
            "moderate_comments INTEGER NOT NULL DEFAULT 0,"
            "max_file_size INTEGER NOT NULL DEFAULT 10,"
            "allowed_file_types TEXT NOT NULL DEFAULT 'jpg,jpeg,png,gif,pdf,doc,docx',"
            "session_timeout INTEGER NOT NULL DEFAULT 24,"
            "maintenance_mode INTEGER NOT NULL DEFAULT 0,"
            "backup_schedule TEXT NOT NULL DEFAULT 'daily',"
            "created_at TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "updated_at TIMESTAMP NOT NULL DEFAULT current_timestamp);"
            );
}

int SystemSettingsInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS system_settings("
            "`id` bigint AUTO_INCREMENT COMMENT '主键id',"
            "`site_name` varchar(100) NOT NULL DEFAULT 'TechBlog' COMMENT '站点名称',"
            "`site_description` varchar(500) NOT NULL DEFAULT '' COMMENT '站点描述',"
            "`site_keywords` varchar(500) NOT NULL DEFAULT '' COMMENT '站点关键词',"
            "`site_icon` varchar(1024) NOT NULL DEFAULT '' COMMENT '站点图标URL',"
            "`site_logo` varchar(1024) NOT NULL DEFAULT '' COMMENT '站点Logo URL',"
            "`favicon` varchar(1024) NOT NULL DEFAULT '' COMMENT 'Favicon URL',"
            "`admin_email` varchar(255) NOT NULL DEFAULT '' COMMENT '管理员邮箱',"
            "`timezone` varchar(50) NOT NULL DEFAULT 'Asia/Shanghai' COMMENT '时区',"
            "`language` varchar(10) NOT NULL DEFAULT 'zh-CN' COMMENT '语言',"
            "`smtp_host` varchar(255) NOT NULL DEFAULT '' COMMENT 'SMTP主机',"
            "`smtp_port` int NOT NULL DEFAULT 587 COMMENT 'SMTP端口',"
            "`smtp_username` varchar(255) NOT NULL DEFAULT '' COMMENT 'SMTP用户名',"
            "`smtp_password` varchar(255) NOT NULL DEFAULT '' COMMENT 'SMTP密码',"
            "`smtp_encryption` varchar(10) NOT NULL DEFAULT 'tls' COMMENT 'SMTP加密方式: none/ssl/tls',"
            "`from_email` varchar(255) NOT NULL DEFAULT '' COMMENT '发件人邮箱',"
            "`from_name` varchar(100) NOT NULL DEFAULT '' COMMENT '发件人名称',"
            "`reply_to` varchar(255) NOT NULL DEFAULT '' COMMENT '回复邮箱',"
            "`enable_registration` int NOT NULL DEFAULT 1 COMMENT '是否允许注册',"
            "`require_email_verification` int NOT NULL DEFAULT 1 COMMENT '是否需要邮箱验证',"
            "`allow_comments` int NOT NULL DEFAULT 1 COMMENT '是否允许评论',"
            "`moderate_comments` int NOT NULL DEFAULT 0 COMMENT '评论是否需要审核',"
            "`max_file_size` int NOT NULL DEFAULT 10 COMMENT '最大上传文件大小(MB)',"
            "`allowed_file_types` varchar(500) NOT NULL DEFAULT 'jpg,jpeg,png,gif,pdf,doc,docx' COMMENT '允许上传的文件类型',"
            "`session_timeout` int NOT NULL DEFAULT 24 COMMENT '会话超时时间(小时)',"
            "`maintenance_mode` int NOT NULL DEFAULT 0 COMMENT '是否维护模式',"
            "`backup_schedule` varchar(20) NOT NULL DEFAULT 'daily' COMMENT '备份计划: disabled/daily/weekly/monthly',"
            "`created_at` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间',"
            "`updated_at` timestamp NOT NULL DEFAULT current_timestamp COMMENT '更新时间',"
            "PRIMARY KEY(`id`))");
}

int SystemSettingsInfoDao::MigrateTableSQLite3(chen::IDB::ptr conn) {
    auto data = conn->query("PRAGMA table_info(system_settings)");
    if (!data) {
        ERROR(logger) << "PRAGMA table_info(system_settings) errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(1));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("site_name");
    expected_cols.insert("site_description");
    expected_cols.insert("site_keywords");
    expected_cols.insert("site_icon");
    expected_cols.insert("site_logo");
    expected_cols.insert("favicon");
    expected_cols.insert("admin_email");
    expected_cols.insert("timezone");
    expected_cols.insert("language");
    expected_cols.insert("smtp_host");
    expected_cols.insert("smtp_port");
    expected_cols.insert("smtp_username");
    expected_cols.insert("smtp_password");
    expected_cols.insert("smtp_encryption");
    expected_cols.insert("from_email");
    expected_cols.insert("from_name");
    expected_cols.insert("reply_to");
    expected_cols.insert("enable_registration");
    expected_cols.insert("require_email_verification");
    expected_cols.insert("allow_comments");
    expected_cols.insert("moderate_comments");
    expected_cols.insert("max_file_size");
    expected_cols.insert("allowed_file_types");
    expected_cols.insert("session_timeout");
    expected_cols.insert("maintenance_mode");
    expected_cols.insert("backup_schedule");
    expected_cols.insert("created_at");
    expected_cols.insert("updated_at");

    if (existing_cols.find("site_name") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.site_name";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN site_name TEXT NOT NULL DEFAULT 'TechBlog'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN site_name failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("site_description") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.site_description";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN site_description TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN site_description failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("site_keywords") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.site_keywords";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN site_keywords TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN site_keywords failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("site_icon") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.site_icon";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN site_icon TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN site_icon failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("site_logo") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.site_logo";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN site_logo TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN site_logo failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("favicon") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.favicon";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN favicon TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN favicon failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("admin_email") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.admin_email";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN admin_email TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN admin_email failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("timezone") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.timezone";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN timezone TEXT NOT NULL DEFAULT 'Asia/Shanghai'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN timezone failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("language") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.language";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN language TEXT NOT NULL DEFAULT 'zh-CN'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN language failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("smtp_host") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.smtp_host";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN smtp_host TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN smtp_host failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("smtp_port") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.smtp_port";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN smtp_port INTEGER NOT NULL DEFAULT 587");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN smtp_port failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("smtp_username") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.smtp_username";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN smtp_username TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN smtp_username failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("smtp_password") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.smtp_password";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN smtp_password TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN smtp_password failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("smtp_encryption") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.smtp_encryption";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN smtp_encryption TEXT NOT NULL DEFAULT 'tls'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN smtp_encryption failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("from_email") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.from_email";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN from_email TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN from_email failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("from_name") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.from_name";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN from_name TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN from_name failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("reply_to") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.reply_to";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN reply_to TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN reply_to failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("enable_registration") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.enable_registration";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN enable_registration INTEGER NOT NULL DEFAULT 1");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN enable_registration failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("require_email_verification") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.require_email_verification";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN require_email_verification INTEGER NOT NULL DEFAULT 1");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN require_email_verification failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("allow_comments") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.allow_comments";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN allow_comments INTEGER NOT NULL DEFAULT 1");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN allow_comments failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("moderate_comments") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.moderate_comments";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN moderate_comments INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN moderate_comments failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("max_file_size") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.max_file_size";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN max_file_size INTEGER NOT NULL DEFAULT 10");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN max_file_size failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("allowed_file_types") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.allowed_file_types";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN allowed_file_types TEXT NOT NULL DEFAULT 'jpg,jpeg,png,gif,pdf,doc,docx'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN allowed_file_types failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("session_timeout") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.session_timeout";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN session_timeout INTEGER NOT NULL DEFAULT 24");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN session_timeout failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("maintenance_mode") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.maintenance_mode";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN maintenance_mode INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN maintenance_mode failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("backup_schedule") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.backup_schedule";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN backup_schedule TEXT NOT NULL DEFAULT 'daily'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN backup_schedule failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("created_at") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.created_at";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN created_at TIMESTAMP NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN created_at failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("updated_at") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.updated_at";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN updated_at TIMESTAMP NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN updated_at failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column system_settings." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE system_settings DROP COLUMN " + col);
            if (rt) {
                ERROR(logger) << "ALTER TABLE system_settings DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}

int SystemSettingsInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM system_settings");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM system_settings errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::set<std::string> existing_cols;
    while (data->next()) {
        existing_cols.insert(data->getString(0));
    }

    std::set<std::string> expected_cols;
    expected_cols.insert("id");
    expected_cols.insert("site_name");
    expected_cols.insert("site_description");
    expected_cols.insert("site_keywords");
    expected_cols.insert("site_icon");
    expected_cols.insert("site_logo");
    expected_cols.insert("favicon");
    expected_cols.insert("admin_email");
    expected_cols.insert("timezone");
    expected_cols.insert("language");
    expected_cols.insert("smtp_host");
    expected_cols.insert("smtp_port");
    expected_cols.insert("smtp_username");
    expected_cols.insert("smtp_password");
    expected_cols.insert("smtp_encryption");
    expected_cols.insert("from_email");
    expected_cols.insert("from_name");
    expected_cols.insert("reply_to");
    expected_cols.insert("enable_registration");
    expected_cols.insert("require_email_verification");
    expected_cols.insert("allow_comments");
    expected_cols.insert("moderate_comments");
    expected_cols.insert("max_file_size");
    expected_cols.insert("allowed_file_types");
    expected_cols.insert("session_timeout");
    expected_cols.insert("maintenance_mode");
    expected_cols.insert("backup_schedule");
    expected_cols.insert("created_at");
    expected_cols.insert("updated_at");

    if (existing_cols.find("site_name") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.site_name";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `site_name` varchar(100) NOT NULL DEFAULT 'TechBlog' COMMENT '站点名称'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN site_name failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("site_description") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.site_description";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `site_description` varchar(500) NOT NULL DEFAULT '' COMMENT '站点描述'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN site_description failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("site_keywords") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.site_keywords";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `site_keywords` varchar(500) NOT NULL DEFAULT '' COMMENT '站点关键词'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN site_keywords failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("site_icon") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.site_icon";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `site_icon` varchar(1024) NOT NULL DEFAULT '' COMMENT '站点图标URL'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN site_icon failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("site_logo") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.site_logo";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `site_logo` varchar(1024) NOT NULL DEFAULT '' COMMENT '站点Logo URL'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN site_logo failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("favicon") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.favicon";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `favicon` varchar(1024) NOT NULL DEFAULT '' COMMENT 'Favicon URL'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN favicon failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("admin_email") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.admin_email";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `admin_email` varchar(255) NOT NULL DEFAULT '' COMMENT '管理员邮箱'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN admin_email failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("timezone") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.timezone";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `timezone` varchar(50) NOT NULL DEFAULT 'Asia/Shanghai' COMMENT '时区'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN timezone failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("language") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.language";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `language` varchar(10) NOT NULL DEFAULT 'zh-CN' COMMENT '语言'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN language failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("smtp_host") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.smtp_host";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `smtp_host` varchar(255) NOT NULL DEFAULT '' COMMENT 'SMTP主机'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN smtp_host failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("smtp_port") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.smtp_port";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `smtp_port` int NOT NULL DEFAULT 587 COMMENT 'SMTP端口'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN smtp_port failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("smtp_username") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.smtp_username";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `smtp_username` varchar(255) NOT NULL DEFAULT '' COMMENT 'SMTP用户名'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN smtp_username failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("smtp_password") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.smtp_password";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `smtp_password` varchar(255) NOT NULL DEFAULT '' COMMENT 'SMTP密码'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN smtp_password failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("smtp_encryption") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.smtp_encryption";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `smtp_encryption` varchar(10) NOT NULL DEFAULT 'tls' COMMENT 'SMTP加密方式: none/ssl/tls'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN smtp_encryption failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("from_email") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.from_email";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `from_email` varchar(255) NOT NULL DEFAULT '' COMMENT '发件人邮箱'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN from_email failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("from_name") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.from_name";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `from_name` varchar(100) NOT NULL DEFAULT '' COMMENT '发件人名称'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN from_name failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("reply_to") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.reply_to";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `reply_to` varchar(255) NOT NULL DEFAULT '' COMMENT '回复邮箱'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN reply_to failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("enable_registration") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.enable_registration";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `enable_registration` int NOT NULL DEFAULT 1 COMMENT '是否允许注册'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN enable_registration failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("require_email_verification") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.require_email_verification";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `require_email_verification` int NOT NULL DEFAULT 1 COMMENT '是否需要邮箱验证'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN require_email_verification failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("allow_comments") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.allow_comments";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `allow_comments` int NOT NULL DEFAULT 1 COMMENT '是否允许评论'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN allow_comments failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("moderate_comments") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.moderate_comments";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `moderate_comments` int NOT NULL DEFAULT 0 COMMENT '评论是否需要审核'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN moderate_comments failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("max_file_size") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.max_file_size";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `max_file_size` int NOT NULL DEFAULT 10 COMMENT '最大上传文件大小(MB)'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN max_file_size failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("allowed_file_types") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.allowed_file_types";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `allowed_file_types` varchar(500) NOT NULL DEFAULT 'jpg,jpeg,png,gif,pdf,doc,docx' COMMENT '允许上传的文件类型'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN allowed_file_types failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("session_timeout") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.session_timeout";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `session_timeout` int NOT NULL DEFAULT 24 COMMENT '会话超时时间(小时)'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN session_timeout failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("maintenance_mode") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.maintenance_mode";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `maintenance_mode` int NOT NULL DEFAULT 0 COMMENT '是否维护模式'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN maintenance_mode failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("backup_schedule") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.backup_schedule";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `backup_schedule` varchar(20) NOT NULL DEFAULT 'daily' COMMENT '备份计划: disabled/daily/weekly/monthly'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN backup_schedule failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("created_at") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.created_at";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `created_at` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN created_at failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("updated_at") == existing_cols.end()) {
        INFO(logger) << "Adding column system_settings.updated_at";
        int rt = conn->execute("ALTER TABLE system_settings ADD COLUMN `updated_at` timestamp NOT NULL DEFAULT current_timestamp COMMENT '更新时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE system_settings ADD COLUMN updated_at failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    for (auto& col : existing_cols) {
        if (expected_cols.find(col) == expected_cols.end()) {
            WARN(logger) << "Dropping column system_settings." << col << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE system_settings DROP COLUMN `" + col + "`");
            if (rt) {
                ERROR(logger) << "ALTER TABLE system_settings DROP COLUMN " << col << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    return 0;
}


} //namespace data
} //namespace blog
