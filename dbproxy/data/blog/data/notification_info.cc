#include "notification_info.h"
#include "chen/log/log.h"

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

NotificationInfo::NotificationInfo()
    :m_isRead(0)
    ,m_isDeleted(0)
    ,m_id()
    ,m_userId()
    ,m_senderId()
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
    std::string sql = "update notification set user_id = ?, title = ?, content = ?, type = ?, sender_id = ?, is_read = ?, read_time = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
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
    stmt->bindInt32(6, info->m_isRead);
    stmt->bindTime(7, info->m_readTime);
    stmt->bindInt32(8, info->m_isDeleted);
    stmt->bindTime(9, info->m_createTime);
    stmt->bindTime(10, info->m_updateTime);
    stmt->bindInt64(11, info->m_id);
    return stmt->execute();
}

int NotificationInfoDao::Insert(NotificationInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into notification (user_id, title, content, type, sender_id, is_read, read_time, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
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
    stmt->bindInt32(6, info->m_isRead);
    stmt->bindTime(7, info->m_readTime);
    stmt->bindInt32(8, info->m_isDeleted);
    stmt->bindTime(9, info->m_createTime);
    stmt->bindTime(10, info->m_updateTime);
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
    std::string sql = "replace into notification (id, user_id, title, content, type, sender_id, is_read, read_time, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
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
    stmt->bindInt32(7, info->m_isRead);
    stmt->bindTime(8, info->m_readTime);
    stmt->bindInt32(9, info->m_isDeleted);
    stmt->bindTime(10, info->m_createTime);
    stmt->bindTime(11, info->m_updateTime);
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
    std::string sql = "select id, user_id, title, content, type, sender_id, is_read, read_time, is_deleted, create_time, update_time from notification";
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
        v->m_isRead = rt->getInt32(6);
        v->m_readTime = rt->getTime(7);
        v->m_isDeleted = rt->getInt32(8);
        v->m_createTime = rt->getTime(9);
        v->m_updateTime = rt->getTime(10);
        results.push_back(v);
    }
    return 0;
}

NotificationInfo::ptr NotificationInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, title, content, type, sender_id, is_read, read_time, is_deleted, create_time, update_time from notification where id = ?";
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
    v->m_isRead = rt->getInt32(6);
    v->m_readTime = rt->getTime(7);
    v->m_isDeleted = rt->getInt32(8);
    v->m_createTime = rt->getTime(9);
    v->m_updateTime = rt->getTime(10);
    return v;
}

int NotificationInfoDao::QueryByUserId(std::vector<NotificationInfo::ptr>& results,  const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, title, content, type, sender_id, is_read, read_time, is_deleted, create_time, update_time from notification where user_id = ?";
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
        v->m_isRead = rt->getInt32(6);
        v->m_readTime = rt->getTime(7);
        v->m_isDeleted = rt->getInt32(8);
        v->m_createTime = rt->getTime(9);
        v->m_updateTime = rt->getTime(10);
        results.push_back(v);
    };
    return 0;
}

int NotificationInfoDao::QueryByUserIdIsRead(std::vector<NotificationInfo::ptr>& results,  const int64_t& user_id,  const int32_t& is_read, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, title, content, type, sender_id, is_read, read_time, is_deleted, create_time, update_time from notification where user_id = ? and is_read = ?";
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
        v->m_isRead = rt->getInt32(6);
        v->m_readTime = rt->getTime(7);
        v->m_isDeleted = rt->getInt32(8);
        v->m_createTime = rt->getTime(9);
        v->m_updateTime = rt->getTime(10);
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
            "`is_read` int NOT NULL DEFAULT 0 COMMENT '是否已读: 0未读 1已读',"
            "`read_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '阅读时间',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT current_timestamp COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "KEY `notification_user_id` (`user_id`),"
            "KEY `notification_user_id_is_read` (`user_id`,`is_read`)) COMMENT='用户通知'");
}
} //namespace data
} //namespace blog
