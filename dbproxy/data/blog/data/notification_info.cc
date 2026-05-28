#include "notification_info.h"
#include "chen/log/log.h"
#include <map>

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

NotificationInfo::NotificationInfo()
    :m_isRead(0)
    ,m_isDeleted(0)
    ,m_id()
    ,m_userId()
    ,m_senderId()
    ,m_articleId()
    ,m_commentId()
    ,m_title()
    ,m_type()
    ,m_content()
    ,m_readTime()
    ,m_createTime(time(0))
    ,m_updateTime(time(0)) {
}

std::string NotificationInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["user_id"] = std::to_string(m_userId);
    v["title"] = m_title;
    v["content"] = m_content;
    v["type"] = m_type;
    v["sender_id"] = std::to_string(m_senderId);
    v["article_id"] = std::to_string(m_articleId);
    v["comment_id"] = std::to_string(m_commentId);
    v["is_read"] = m_isRead;
    v["read_time"] = chen::Time2Str(m_readTime);
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void NotificationInfo::setId(const int64_t& v) {
    m_id = v;
}

void NotificationInfo::setUserId(const int64_t& v) {
    m_userId = v;
}

void NotificationInfo::setTitle(const std::string& v) {
    m_title = v;
}

void NotificationInfo::setContent(const std::string& v) {
    m_content = v;
}

void NotificationInfo::setType(const std::string& v) {
    m_type = v;
}

void NotificationInfo::setSenderId(const int64_t& v) {
    m_senderId = v;
}

void NotificationInfo::setArticleId(const int64_t& v) {
    m_articleId = v;
}

void NotificationInfo::setCommentId(const int64_t& v) {
    m_commentId = v;
}

void NotificationInfo::setIsRead(const int32_t& v) {
    m_isRead = v;
}

void NotificationInfo::setReadTime(const int64_t& v) {
    m_readTime = v;
}

void NotificationInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void NotificationInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void NotificationInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int NotificationInfoDao::Update(NotificationInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update notification set user_id = ?, title = ?, content = ?, type = ?, sender_id = ?, article_id = ?, comment_id = ?, is_read = ?, read_time = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_userId);
    stmt->bindString(2, info->m_title);
    stmt->bindString(3, info->m_content);
    stmt->bindString(4, info->m_type);
    stmt->bindInt64(5, info->m_senderId);
    stmt->bindInt64(6, info->m_articleId);
    stmt->bindInt64(7, info->m_commentId);
    stmt->bindInt32(8, info->m_isRead);
    stmt->bindTime(9, info->m_readTime);
    stmt->bindInt32(10, info->m_isDeleted);
    stmt->bindTime(11, info->m_createTime);
    stmt->bindTime(12, info->m_updateTime);
    stmt->bindInt64(13, info->m_id);
    return stmt->execute();
}

int NotificationInfoDao::Insert(NotificationInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into notification (user_id, title, content, type, sender_id, article_id, comment_id, is_read, read_time, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_userId);
    stmt->bindString(2, info->m_title);
    stmt->bindString(3, info->m_content);
    stmt->bindString(4, info->m_type);
    stmt->bindInt64(5, info->m_senderId);
    stmt->bindInt64(6, info->m_articleId);
    stmt->bindInt64(7, info->m_commentId);
    stmt->bindInt32(8, info->m_isRead);
    stmt->bindTime(9, info->m_readTime);
    stmt->bindInt32(10, info->m_isDeleted);
    stmt->bindTime(11, info->m_createTime);
    stmt->bindTime(12, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int NotificationInfoDao::InsertOrUpdate(NotificationInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into notification (id, user_id, title, content, type, sender_id, article_id, comment_id, is_read, read_time, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_userId);
    stmt->bindString(3, info->m_title);
    stmt->bindString(4, info->m_content);
    stmt->bindString(5, info->m_type);
    stmt->bindInt64(6, info->m_senderId);
    stmt->bindInt64(7, info->m_articleId);
    stmt->bindInt64(8, info->m_commentId);
    stmt->bindInt32(9, info->m_isRead);
    stmt->bindTime(10, info->m_readTime);
    stmt->bindInt32(11, info->m_isDeleted);
    stmt->bindTime(12, info->m_createTime);
    stmt->bindTime(13, info->m_updateTime);
    return stmt->execute();
}

int NotificationInfoDao::Delete(NotificationInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from notification where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int NotificationInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from notification where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int NotificationInfoDao::DeleteByUserId( const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "delete from notification where user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    return stmt->execute();
}

int NotificationInfoDao::DeleteByUserIdIsRead( const int64_t& user_id,  const int32_t& is_read, chen::IDB::ptr conn) {
    std::string sql = "delete from notification where user_id = ? and is_read = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    stmt->bindInt32(1, is_read);
    return stmt->execute();
}

int NotificationInfoDao::QueryAll(std::vector<NotificationInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, title, content, type, sender_id, article_id, comment_id, is_read, read_time, is_deleted, create_time, update_time from notification";
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
        NotificationInfo::ptr v(new NotificationInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_title = rt->getString(2);
        v->m_content = rt->getString(3);
        v->m_type = rt->getString(4);
        v->m_senderId = rt->getInt64(5);
        v->m_articleId = rt->getInt64(6);
        v->m_commentId = rt->getInt64(7);
        v->m_isRead = rt->getInt32(8);
        v->m_readTime = rt->getTime(9);
        v->m_isDeleted = rt->getInt32(10);
        v->m_createTime = rt->getTime(11);
        v->m_updateTime = rt->getTime(12);
        results.push_back(v);
    }
    return 0;
}

NotificationInfo::ptr NotificationInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, title, content, type, sender_id, article_id, comment_id, is_read, read_time, is_deleted, create_time, update_time from notification where id = ?";
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
    NotificationInfo::ptr v(new NotificationInfo);
    v->m_id = rt->getInt64(0);
    v->m_userId = rt->getInt64(1);
    v->m_title = rt->getString(2);
    v->m_content = rt->getString(3);
    v->m_type = rt->getString(4);
    v->m_senderId = rt->getInt64(5);
    v->m_articleId = rt->getInt64(6);
    v->m_commentId = rt->getInt64(7);
    v->m_isRead = rt->getInt32(8);
    v->m_readTime = rt->getTime(9);
    v->m_isDeleted = rt->getInt32(10);
    v->m_createTime = rt->getTime(11);
    v->m_updateTime = rt->getTime(12);
    return v;
}

int NotificationInfoDao::QueryByUserId(std::vector<NotificationInfo::ptr>& results,  const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, title, content, type, sender_id, article_id, comment_id, is_read, read_time, is_deleted, create_time, update_time from notification where user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        NotificationInfo::ptr v(new NotificationInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_title = rt->getString(2);
        v->m_content = rt->getString(3);
        v->m_type = rt->getString(4);
        v->m_senderId = rt->getInt64(5);
        v->m_articleId = rt->getInt64(6);
        v->m_commentId = rt->getInt64(7);
        v->m_isRead = rt->getInt32(8);
        v->m_readTime = rt->getTime(9);
        v->m_isDeleted = rt->getInt32(10);
        v->m_createTime = rt->getTime(11);
        v->m_updateTime = rt->getTime(12);
        results.push_back(v);
    };
    return 0;
}

int NotificationInfoDao::QueryByUserIdIsRead(std::vector<NotificationInfo::ptr>& results,  const int64_t& user_id,  const int32_t& is_read, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, title, content, type, sender_id, article_id, comment_id, is_read, read_time, is_deleted, create_time, update_time from notification where user_id = ? and is_read = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    stmt->bindInt32(2, is_read);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        NotificationInfo::ptr v(new NotificationInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_title = rt->getString(2);
        v->m_content = rt->getString(3);
        v->m_type = rt->getString(4);
        v->m_senderId = rt->getInt64(5);
        v->m_articleId = rt->getInt64(6);
        v->m_commentId = rt->getInt64(7);
        v->m_isRead = rt->getInt32(8);
        v->m_readTime = rt->getTime(9);
        v->m_isDeleted = rt->getInt32(10);
        v->m_createTime = rt->getTime(11);
        v->m_updateTime = rt->getTime(12);
        results.push_back(v);
    };
    return 0;
}

int NotificationInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS notification("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "user_id INTEGER NOT NULL DEFAULT 0,"
            "title TEXT NOT NULL DEFAULT '',"
            "content TEXT NOT NULL DEFAULT '',"
            "type TEXT NOT NULL DEFAULT '',"
            "sender_id INTEGER NOT NULL DEFAULT 0,"
            "article_id INTEGER NOT NULL DEFAULT 0,"
            "comment_id INTEGER NOT NULL DEFAULT 0,"
            "is_read INTEGER NOT NULL DEFAULT 0,"
            "read_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "update_time TIMESTAMP NOT NULL DEFAULT current_timestamp);"
            "CREATE INDEX IF NOT EXISTS notification_user_id ON notification(user_id);"
            "CREATE INDEX IF NOT EXISTS notification_user_id_is_read ON notification(user_id,is_read);"
            );
}

int NotificationInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS notification("
            "`id` bigint AUTO_INCREMENT COMMENT '主键id',"
            "`user_id` bigint NOT NULL DEFAULT 0 COMMENT '接收用户id',"
            "`title` varchar(128) NOT NULL DEFAULT '' COMMENT '通知标题',"
            "`content` text NOT NULL DEFAULT '' COMMENT '通知内容',"
            "`type` varchar(32) NOT NULL DEFAULT '' COMMENT '通知类型: system/announcement/article',"
            "`sender_id` bigint NOT NULL DEFAULT 0 COMMENT '发送者id(0=系统)',"
            "`article_id` bigint NOT NULL DEFAULT 0 COMMENT '关联文章ID',"
            "`comment_id` bigint NOT NULL DEFAULT 0 COMMENT '关联评论ID',"
            "`is_read` int NOT NULL DEFAULT 0 COMMENT '是否已读: 0未读 1已读',"
            "`read_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '阅读时间',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "KEY `notification_user_id` (`user_id`),"
            "KEY `notification_user_id_is_read` (`user_id`,`is_read`)) COMMENT='用户通知'");
}

int NotificationInfoDao::MigrateTableSQLite3(chen::IDB::ptr conn) {
    auto data = conn->query("PRAGMA table_info(notification)");
    if (!data) {
        ERROR(logger) << "PRAGMA table_info(notification) errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::map<std::string, std::string> existing_cols;  // name -> type
    while (data->next()) {
        existing_cols[data->getString(1)] = data->getString(2);
    }

    bool need_recreate = false;
    {
        auto it = existing_cols.find("id");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: notification.id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("user_id");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: notification.user_id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("title");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: notification.title " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("content");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: notification.content " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("type");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: notification.type " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("sender_id");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: notification.sender_id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("article_id");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: notification.article_id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("comment_id");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: notification.comment_id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("is_read");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: notification.is_read " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("read_time");
        if (it != existing_cols.end() && it->second != "TIMESTAMP") {
            INFO(logger) << "Column type changed: notification.read_time " << it->second << " -> TIMESTAMP";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("is_deleted");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: notification.is_deleted " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("create_time");
        if (it != existing_cols.end() && it->second != "TIMESTAMP") {
            INFO(logger) << "Column type changed: notification.create_time " << it->second << " -> TIMESTAMP";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("update_time");
        if (it != existing_cols.end() && it->second != "TIMESTAMP") {
            INFO(logger) << "Column type changed: notification.update_time " << it->second << " -> TIMESTAMP";
            need_recreate = true;
        }
    }
    if (!need_recreate) {
        for (auto& [name, _] : existing_cols) {
            (void)_;  // suppress unused warning
            bool found = false;
            if (name == "id") found = true;
            if (name == "user_id") found = true;
            if (name == "title") found = true;
            if (name == "content") found = true;
            if (name == "type") found = true;
            if (name == "sender_id") found = true;
            if (name == "article_id") found = true;
            if (name == "comment_id") found = true;
            if (name == "is_read") found = true;
            if (name == "read_time") found = true;
            if (name == "is_deleted") found = true;
            if (name == "create_time") found = true;
            if (name == "update_time") found = true;
            if (!found) {
                need_recreate = true;
                WARN(logger) << "Column notification." << name << " removed, table recreate required";
                break;
            }
        }
    }

    if (need_recreate) {
        INFO(logger) << "Recreating table notification";

        std::vector<std::string> common_cols;
        if (existing_cols.find("id") != existing_cols.end()) {
            common_cols.push_back("id");
        }
        if (existing_cols.find("user_id") != existing_cols.end()) {
            common_cols.push_back("user_id");
        }
        if (existing_cols.find("title") != existing_cols.end()) {
            common_cols.push_back("title");
        }
        if (existing_cols.find("content") != existing_cols.end()) {
            common_cols.push_back("content");
        }
        if (existing_cols.find("type") != existing_cols.end()) {
            common_cols.push_back("type");
        }
        if (existing_cols.find("sender_id") != existing_cols.end()) {
            common_cols.push_back("sender_id");
        }
        if (existing_cols.find("article_id") != existing_cols.end()) {
            common_cols.push_back("article_id");
        }
        if (existing_cols.find("comment_id") != existing_cols.end()) {
            common_cols.push_back("comment_id");
        }
        if (existing_cols.find("is_read") != existing_cols.end()) {
            common_cols.push_back("is_read");
        }
        if (existing_cols.find("read_time") != existing_cols.end()) {
            common_cols.push_back("read_time");
        }
        if (existing_cols.find("is_deleted") != existing_cols.end()) {
            common_cols.push_back("is_deleted");
        }
        if (existing_cols.find("create_time") != existing_cols.end()) {
            common_cols.push_back("create_time");
        }
        if (existing_cols.find("update_time") != existing_cols.end()) {
            common_cols.push_back("update_time");
        }

        if (conn->execute("ALTER TABLE notification RENAME TO notification_tmp")) {
            ERROR(logger) << "RENAME TABLE notification failed";
            return conn->getErrno();
        }
        CreateTableSQLite3(conn);
        if (!common_cols.empty()) {
            std::string cols;
            for (size_t i = 0; i < common_cols.size(); ++i) {
                if (i) cols += ",";
                cols += common_cols[i];
            }
            std::string sql = "INSERT INTO notification (" + cols + ") SELECT " + cols + " FROM notification_tmp";
            if (int rt = conn->execute(sql)) {
                ERROR(logger) << "copy data from notification_tmp to notification failed, errno=" << rt;
                // don't return; try to continue
            }
        }
        conn->execute("DROP TABLE notification_tmp");
        return 0;
    }

    if (existing_cols.find("user_id") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.user_id";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN user_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN user_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("title") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.title";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN title TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN title failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("content") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.content";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN content TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN content failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("type") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.type";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN type TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("sender_id") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.sender_id";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN sender_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN sender_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("article_id") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.article_id";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN article_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN article_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("comment_id") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.comment_id";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN comment_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN comment_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_read") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.is_read";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN is_read INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN is_read failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("read_time") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.read_time";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN read_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN read_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.is_deleted";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.create_time";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN create_time TIMESTAMP NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.update_time";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN update_time TIMESTAMP NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    return 0;
}

int NotificationInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM notification");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM notification errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::map<std::string, std::string> existing_cols;  // name -> type
    while (data->next()) {
        existing_cols[data->getString(0)] = data->getString(1);
    }

    {
        auto it = existing_cols.find("id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column notification.id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE notification MODIFY COLUMN `id` bigint NOT NULL DEFAULT 0 COMMENT '主键id'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN notification.id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("user_id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column notification.user_id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE notification MODIFY COLUMN `user_id` bigint NOT NULL DEFAULT 0 COMMENT '接收用户id'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN notification.user_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("title");
        if (it != existing_cols.end() && it->second != "varchar(128)") {
            INFO(logger) << "Modifying column notification.title " << it->second << " -> varchar(128)";
            int rt = conn->execute("ALTER TABLE notification MODIFY COLUMN `title` varchar(128) NOT NULL DEFAULT '' COMMENT '通知标题'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN notification.title failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("content");
        if (it != existing_cols.end() && it->second != "text") {
            INFO(logger) << "Modifying column notification.content " << it->second << " -> text";
            int rt = conn->execute("ALTER TABLE notification MODIFY COLUMN `content` text NOT NULL DEFAULT '' COMMENT '通知内容'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN notification.content failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("type");
        if (it != existing_cols.end() && it->second != "varchar(32)") {
            INFO(logger) << "Modifying column notification.type " << it->second << " -> varchar(32)";
            int rt = conn->execute("ALTER TABLE notification MODIFY COLUMN `type` varchar(32) NOT NULL DEFAULT '' COMMENT '通知类型: system/announcement/article'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN notification.type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("sender_id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column notification.sender_id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE notification MODIFY COLUMN `sender_id` bigint NOT NULL DEFAULT 0 COMMENT '发送者id(0=系统)'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN notification.sender_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("article_id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column notification.article_id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE notification MODIFY COLUMN `article_id` bigint NOT NULL DEFAULT 0 COMMENT '关联文章ID'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN notification.article_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("comment_id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column notification.comment_id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE notification MODIFY COLUMN `comment_id` bigint NOT NULL DEFAULT 0 COMMENT '关联评论ID'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN notification.comment_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("is_read");
        if (it != existing_cols.end() && it->second != "int") {
            INFO(logger) << "Modifying column notification.is_read " << it->second << " -> int";
            int rt = conn->execute("ALTER TABLE notification MODIFY COLUMN `is_read` int NOT NULL DEFAULT 0 COMMENT '是否已读: 0未读 1已读'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN notification.is_read failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("read_time");
        if (it != existing_cols.end() && it->second != "timestamp") {
            INFO(logger) << "Modifying column notification.read_time " << it->second << " -> timestamp";
            int rt = conn->execute("ALTER TABLE notification MODIFY COLUMN `read_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '阅读时间'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN notification.read_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("is_deleted");
        if (it != existing_cols.end() && it->second != "int") {
            INFO(logger) << "Modifying column notification.is_deleted " << it->second << " -> int";
            int rt = conn->execute("ALTER TABLE notification MODIFY COLUMN `is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN notification.is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("create_time");
        if (it != existing_cols.end() && it->second != "timestamp") {
            INFO(logger) << "Modifying column notification.create_time " << it->second << " -> timestamp";
            int rt = conn->execute("ALTER TABLE notification MODIFY COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN notification.create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("update_time");
        if (it != existing_cols.end() && it->second != "timestamp") {
            INFO(logger) << "Modifying column notification.update_time " << it->second << " -> timestamp";
            int rt = conn->execute("ALTER TABLE notification MODIFY COLUMN `update_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '更新时间'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN notification.update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    for (auto& [name, _] : existing_cols) {
        (void)_;
        bool found = false;
        if (name == "id") found = true;
        if (name == "user_id") found = true;
        if (name == "title") found = true;
        if (name == "content") found = true;
        if (name == "type") found = true;
        if (name == "sender_id") found = true;
        if (name == "article_id") found = true;
        if (name == "comment_id") found = true;
        if (name == "is_read") found = true;
        if (name == "read_time") found = true;
        if (name == "is_deleted") found = true;
        if (name == "create_time") found = true;
        if (name == "update_time") found = true;
        if (!found) {
            WARN(logger) << "Dropping column notification." << name << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE notification DROP COLUMN `" + name + "`");
            if (rt) {
                ERROR(logger) << "DROP COLUMN notification." << name << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    if (existing_cols.find("user_id") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.user_id";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN `user_id` bigint NOT NULL DEFAULT 0 COMMENT '接收用户id'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN user_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("title") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.title";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN `title` varchar(128) NOT NULL DEFAULT '' COMMENT '通知标题'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN title failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("content") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.content";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN `content` text NOT NULL DEFAULT '' COMMENT '通知内容'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN content failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("type") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.type";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN `type` varchar(32) NOT NULL DEFAULT '' COMMENT '通知类型: system/announcement/article'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN type failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("sender_id") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.sender_id";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN `sender_id` bigint NOT NULL DEFAULT 0 COMMENT '发送者id(0=系统)'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN sender_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("article_id") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.article_id";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN `article_id` bigint NOT NULL DEFAULT 0 COMMENT '关联文章ID'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN article_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("comment_id") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.comment_id";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN `comment_id` bigint NOT NULL DEFAULT 0 COMMENT '关联评论ID'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN comment_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_read") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.is_read";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN `is_read` int NOT NULL DEFAULT 0 COMMENT '是否已读: 0未读 1已读'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN is_read failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("read_time") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.read_time";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN `read_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '阅读时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN read_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.is_deleted";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN `is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.create_time";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column notification.update_time";
        int rt = conn->execute("ALTER TABLE notification ADD COLUMN `update_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '更新时间'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE notification ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    return 0;
}


} //namespace data
} //namespace blog
