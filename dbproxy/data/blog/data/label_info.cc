#include "label_info.h"
#include "chen/log/log.h"

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

LabelInfo::LabelInfo()
    :m_isDeleted()
    ,m_id()
    ,m_userId()
    ,m_name()
    ,m_color()
    ,m_description()
    ,m_createTime(time(0))
    ,m_updateTime() {
}

std::string LabelInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["user_id"] = std::to_string(m_userId);
    v["name"] = m_name;
    v["color"] = m_color;
    v["description"] = m_description;
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void LabelInfo::setId(const int64_t& v) {
    m_id = v;
}

void LabelInfo::setUserId(const int64_t& v) {
    m_userId = v;
}

void LabelInfo::setName(const std::string& v) {
    m_name = v;
}

void LabelInfo::setColor(const std::string& v) {
    m_color = v;
}

void LabelInfo::setDescription(const std::string& v) {
    m_description = v;
}

void LabelInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void LabelInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void LabelInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int LabelInfoDao::Update(LabelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update label set user_id = ?, name = ?, color = ?, description = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_userId);
    stmt->bindString(2, info->m_name);
    stmt->bindString(3, info->m_color);
    stmt->bindString(4, info->m_description);
    stmt->bindInt32(5, info->m_isDeleted);
    stmt->bindTime(6, info->m_createTime);
    stmt->bindTime(7, info->m_updateTime);
    stmt->bindInt64(8, info->m_id);
    return stmt->execute();
}

int LabelInfoDao::Insert(LabelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into label (user_id, name, color, description, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_userId);
    stmt->bindString(2, info->m_name);
    stmt->bindString(3, info->m_color);
    stmt->bindString(4, info->m_description);
    stmt->bindInt32(5, info->m_isDeleted);
    stmt->bindTime(6, info->m_createTime);
    stmt->bindTime(7, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int LabelInfoDao::InsertOrUpdate(LabelInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into label (id, user_id, name, color, description, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_userId);
    stmt->bindString(3, info->m_name);
    stmt->bindString(4, info->m_color);
    stmt->bindString(5, info->m_description);
    stmt->bindInt32(6, info->m_isDeleted);
    stmt->bindTime(7, info->m_createTime);
    stmt->bindTime(8, info->m_updateTime);
    return stmt->execute();
}

int LabelInfoDao::Delete(LabelInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from label where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int LabelInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from label where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int LabelInfoDao::DeleteByUserId( const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "delete from label where user_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    return stmt->execute();
}

int LabelInfoDao::DeleteByUserIdName( const int64_t& user_id,  const std::string& name, chen::IDB::ptr conn) {
    std::string sql = "delete from label where user_id = ? and name = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, user_id);
    stmt->bindString(1, name);
    return stmt->execute();
}

int LabelInfoDao::QueryAll(std::vector<LabelInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, name, color, description, is_deleted, create_time, update_time from label";
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
        LabelInfo::ptr v(new LabelInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_name = rt->getString(2);
        v->m_color = rt->getString(3);
        v->m_description = rt->getString(4);
        v->m_isDeleted = rt->getInt32(5);
        v->m_createTime = rt->getTime(6);
        v->m_updateTime = rt->getTime(7);
        results.push_back(v);
    }
    return 0;
}

LabelInfo::ptr LabelInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, name, color, description, is_deleted, create_time, update_time from label where id = ?";
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
    LabelInfo::ptr v(new LabelInfo);
    v->m_id = rt->getInt64(0);
    v->m_userId = rt->getInt64(1);
    v->m_name = rt->getString(2);
    v->m_color = rt->getString(3);
    v->m_description = rt->getString(4);
    v->m_isDeleted = rt->getInt32(5);
    v->m_createTime = rt->getTime(6);
    v->m_updateTime = rt->getTime(7);
    return v;
}

int LabelInfoDao::QueryByUserId(std::vector<LabelInfo::ptr>& results,  const int64_t& user_id, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, name, color, description, is_deleted, create_time, update_time from label where user_id = ?";
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
        LabelInfo::ptr v(new LabelInfo);
        v->m_id = rt->getInt64(0);
        v->m_userId = rt->getInt64(1);
        v->m_name = rt->getString(2);
        v->m_color = rt->getString(3);
        v->m_description = rt->getString(4);
        v->m_isDeleted = rt->getInt32(5);
        v->m_createTime = rt->getTime(6);
        v->m_updateTime = rt->getTime(7);
        results.push_back(v);
    };
    return 0;
}

LabelInfo::ptr LabelInfoDao::QueryByUserIdName( const int64_t& user_id,  const std::string& name, chen::IDB::ptr conn) {
    std::string sql = "select id, user_id, name, color, description, is_deleted, create_time, update_time from label where user_id = ? and name = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindInt64(1, user_id);
    stmt->bindString(2, name);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    LabelInfo::ptr v(new LabelInfo);
    v->m_id = rt->getInt64(0);
    v->m_userId = rt->getInt64(1);
    v->m_name = rt->getString(2);
    v->m_color = rt->getString(3);
    v->m_description = rt->getString(4);
    v->m_isDeleted = rt->getInt32(5);
    v->m_createTime = rt->getTime(6);
    v->m_updateTime = rt->getTime(7);
    return v;
}

int LabelInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS label("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "user_id INTEGER NOT NULL DEFAULT 0,"
            "name TEXT NOT NULL DEFAULT '',"
            "color TEXT NOT NULL DEFAULT '',"
            "description TEXT NOT NULL DEFAULT '',"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE INDEX IF NOT EXISTS label_user_id ON label(user_id);"
            "CREATE UNIQUE INDEX IF NOT EXISTS label_user_id_name ON label(user_id,name);"
            );
}

int LabelInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE IF NOT EXISTS label("
            "`id` bigint AUTO_INCREMENT,"
            "`user_id` bigint NOT NULL DEFAULT 0,"
            "`name` varchar(20) NOT NULL DEFAULT '',"
            "`color` varchar(10) NOT NULL DEFAULT '',"
            "`description` varchar(255) NOT NULL DEFAULT '',"
            "`is_deleted` int NOT NULL DEFAULT 0,"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp,"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp ,"
            "PRIMARY KEY(`id`),"
            "KEY `label_user_id` (`user_id`),"
            "UNIQUE KEY `label_user_id_name` (`user_id`,`name`))");
}
} //namespace data
} //namespace blog
