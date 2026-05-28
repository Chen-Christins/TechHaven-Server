#include "label_info.h"
#include "chen/log/log.h"
#include <map>

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

int LabelInfoDao::MigrateTableSQLite3(chen::IDB::ptr conn) {
    auto data = conn->query("PRAGMA table_info(label)");
    if (!data) {
        ERROR(logger) << "PRAGMA table_info(label) errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
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
            INFO(logger) << "Column type changed: label.id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("user_id");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: label.user_id " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("name");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: label.name " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("color");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: label.color " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("description");
        if (it != existing_cols.end() && it->second != "TEXT") {
            INFO(logger) << "Column type changed: label.description " << it->second << " -> TEXT";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("is_deleted");
        if (it != existing_cols.end() && it->second != "INTEGER") {
            INFO(logger) << "Column type changed: label.is_deleted " << it->second << " -> INTEGER";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("create_time");
        if (it != existing_cols.end() && it->second != "TIMESTAMP") {
            INFO(logger) << "Column type changed: label.create_time " << it->second << " -> TIMESTAMP";
            need_recreate = true;
        }
    }
    {
        auto it = existing_cols.find("update_time");
        if (it != existing_cols.end() && it->second != "TIMESTAMP") {
            INFO(logger) << "Column type changed: label.update_time " << it->second << " -> TIMESTAMP";
            need_recreate = true;
        }
    }
    if (!need_recreate) {
        for (auto& [name, _] : existing_cols) {
            (void)_;  // suppress unused warning
            bool found = false;
            if (name == "id") found = true;
            if (name == "user_id") found = true;
            if (name == "name") found = true;
            if (name == "color") found = true;
            if (name == "description") found = true;
            if (name == "is_deleted") found = true;
            if (name == "create_time") found = true;
            if (name == "update_time") found = true;
            if (!found) {
                need_recreate = true;
                WARN(logger) << "Column label." << name << " removed, table recreate required";
                break;
            }
        }
    }

    if (need_recreate) {
        INFO(logger) << "Recreating table label";

        std::vector<std::string> common_cols;
        if (existing_cols.find("id") != existing_cols.end()) {
            common_cols.push_back("id");
        }
        if (existing_cols.find("user_id") != existing_cols.end()) {
            common_cols.push_back("user_id");
        }
        if (existing_cols.find("name") != existing_cols.end()) {
            common_cols.push_back("name");
        }
        if (existing_cols.find("color") != existing_cols.end()) {
            common_cols.push_back("color");
        }
        if (existing_cols.find("description") != existing_cols.end()) {
            common_cols.push_back("description");
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

        if (conn->execute("ALTER TABLE label RENAME TO label_tmp")) {
            ERROR(logger) << "RENAME TABLE label failed";
            return conn->getErrno();
        }
        CreateTableSQLite3(conn);
        if (!common_cols.empty()) {
            std::string cols;
            for (size_t i = 0; i < common_cols.size(); ++i) {
                if (i) cols += ",";
                cols += common_cols[i];
            }
            std::string sql = "INSERT INTO label (" + cols + ") SELECT " + cols + " FROM label_tmp";
            if (int rt = conn->execute(sql)) {
                ERROR(logger) << "copy data from label_tmp to label failed, errno=" << rt;
                // don't return; try to continue
            }
        }
        conn->execute("DROP TABLE label_tmp");
        return 0;
    }

    if (existing_cols.find("user_id") == existing_cols.end()) {
        INFO(logger) << "Adding column label.user_id";
        int rt = conn->execute("ALTER TABLE label ADD COLUMN user_id INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE label ADD COLUMN user_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("name") == existing_cols.end()) {
        INFO(logger) << "Adding column label.name";
        int rt = conn->execute("ALTER TABLE label ADD COLUMN name TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE label ADD COLUMN name failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("color") == existing_cols.end()) {
        INFO(logger) << "Adding column label.color";
        int rt = conn->execute("ALTER TABLE label ADD COLUMN color TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE label ADD COLUMN color failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("description") == existing_cols.end()) {
        INFO(logger) << "Adding column label.description";
        int rt = conn->execute("ALTER TABLE label ADD COLUMN description TEXT NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE label ADD COLUMN description failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column label.is_deleted";
        int rt = conn->execute("ALTER TABLE label ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE label ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column label.create_time";
        int rt = conn->execute("ALTER TABLE label ADD COLUMN create_time TIMESTAMP NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE label ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column label.update_time";
        int rt = conn->execute("ALTER TABLE label ADD COLUMN update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE label ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    return 0;
}

int LabelInfoDao::MigrateTableMySQL(chen::IDB::ptr conn) {
    auto data = conn->query("SHOW COLUMNS FROM label");
    if (!data) {
        ERROR(logger) << "SHOW COLUMNS FROM label errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    std::map<std::string, std::string> existing_cols;  // name -> type
    while (data->next()) {
        existing_cols[data->getString(0)] = data->getString(1);
    }

    {
        auto it = existing_cols.find("id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column label.id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE label MODIFY COLUMN `id` bigint NOT NULL DEFAULT 0");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN label.id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("user_id");
        if (it != existing_cols.end() && it->second != "bigint") {
            INFO(logger) << "Modifying column label.user_id " << it->second << " -> bigint";
            int rt = conn->execute("ALTER TABLE label MODIFY COLUMN `user_id` bigint NOT NULL DEFAULT 0");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN label.user_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("name");
        if (it != existing_cols.end() && it->second != "varchar(20)") {
            INFO(logger) << "Modifying column label.name " << it->second << " -> varchar(20)";
            int rt = conn->execute("ALTER TABLE label MODIFY COLUMN `name` varchar(20) NOT NULL DEFAULT ''");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN label.name failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("color");
        if (it != existing_cols.end() && it->second != "varchar(10)") {
            INFO(logger) << "Modifying column label.color " << it->second << " -> varchar(10)";
            int rt = conn->execute("ALTER TABLE label MODIFY COLUMN `color` varchar(10) NOT NULL DEFAULT ''");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN label.color failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("description");
        if (it != existing_cols.end() && it->second != "varchar(255)") {
            INFO(logger) << "Modifying column label.description " << it->second << " -> varchar(255)";
            int rt = conn->execute("ALTER TABLE label MODIFY COLUMN `description` varchar(255) NOT NULL DEFAULT ''");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN label.description failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("is_deleted");
        if (it != existing_cols.end() && it->second != "int") {
            INFO(logger) << "Modifying column label.is_deleted " << it->second << " -> int";
            int rt = conn->execute("ALTER TABLE label MODIFY COLUMN `is_deleted` int NOT NULL DEFAULT 0");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN label.is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("create_time");
        if (it != existing_cols.end() && it->second != "timestamp") {
            INFO(logger) << "Modifying column label.create_time " << it->second << " -> timestamp";
            int rt = conn->execute("ALTER TABLE label MODIFY COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN label.create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }
    {
        auto it = existing_cols.find("update_time");
        if (it != existing_cols.end() && it->second != "timestamp") {
            INFO(logger) << "Modifying column label.update_time " << it->second << " -> timestamp";
            int rt = conn->execute("ALTER TABLE label MODIFY COLUMN `update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00'");
            if (rt) {
                ERROR(logger) << "MODIFY COLUMN label.update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    for (auto& [name, _] : existing_cols) {
        (void)_;
        bool found = false;
        if (name == "id") found = true;
        if (name == "user_id") found = true;
        if (name == "name") found = true;
        if (name == "color") found = true;
        if (name == "description") found = true;
        if (name == "is_deleted") found = true;
        if (name == "create_time") found = true;
        if (name == "update_time") found = true;
        if (!found) {
            WARN(logger) << "Dropping column label." << name << " (not in schema, data will be lost)";
            int rt = conn->execute("ALTER TABLE label DROP COLUMN `" + name + "`");
            if (rt) {
                ERROR(logger) << "DROP COLUMN label." << name << " failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
            }
        }
    }

    if (existing_cols.find("user_id") == existing_cols.end()) {
        INFO(logger) << "Adding column label.user_id";
        int rt = conn->execute("ALTER TABLE label ADD COLUMN `user_id` bigint NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE label ADD COLUMN user_id failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("name") == existing_cols.end()) {
        INFO(logger) << "Adding column label.name";
        int rt = conn->execute("ALTER TABLE label ADD COLUMN `name` varchar(20) NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE label ADD COLUMN name failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("color") == existing_cols.end()) {
        INFO(logger) << "Adding column label.color";
        int rt = conn->execute("ALTER TABLE label ADD COLUMN `color` varchar(10) NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE label ADD COLUMN color failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("description") == existing_cols.end()) {
        INFO(logger) << "Adding column label.description";
        int rt = conn->execute("ALTER TABLE label ADD COLUMN `description` varchar(255) NOT NULL DEFAULT ''");
        if (rt) {
            ERROR(logger) << "ALTER TABLE label ADD COLUMN description failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("is_deleted") == existing_cols.end()) {
        INFO(logger) << "Adding column label.is_deleted";
        int rt = conn->execute("ALTER TABLE label ADD COLUMN `is_deleted` int NOT NULL DEFAULT 0");
        if (rt) {
            ERROR(logger) << "ALTER TABLE label ADD COLUMN is_deleted failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("create_time") == existing_cols.end()) {
        INFO(logger) << "Adding column label.create_time";
        int rt = conn->execute("ALTER TABLE label ADD COLUMN `create_time` timestamp NOT NULL DEFAULT current_timestamp");
        if (rt) {
            ERROR(logger) << "ALTER TABLE label ADD COLUMN create_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    if (existing_cols.find("update_time") == existing_cols.end()) {
        INFO(logger) << "Adding column label.update_time";
        int rt = conn->execute("ALTER TABLE label ADD COLUMN `update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00'");
        if (rt) {
            ERROR(logger) << "ALTER TABLE label ADD COLUMN update_time failed, errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        }
    }

    return 0;
}


} //namespace data
} //namespace blog
