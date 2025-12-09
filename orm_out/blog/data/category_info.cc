#include "category_info.h"
#include "chen/log/log.h"

namespace blog {
namespace data {

static chen::Logger::ptr logger = LOG_NAME("orm");

CategoryInfo::CategoryInfo()
    :m_status()
    ,m_isDeleted()
    ,m_id()
    ,m_parentId()
    ,m_name()
    ,m_color()
    ,m_description()
    ,m_url()
    ,m_icon()
    ,m_createTime(time(0))
    ,m_updateTime() {
}

std::string CategoryInfo::toJsonString() const {
    Json::Value v;
    v["id"] = std::to_string(m_id);
    v["name"] = m_name;
    v["color"] = m_color;
    v["description"] = m_description;
    v["url"] = m_url;
    v["icon"] = m_icon;
    v["parent_id"] = std::to_string(m_parentId);
    v["status"] = m_status;
    v["is_deleted"] = m_isDeleted;
    v["create_time"] = chen::Time2Str(m_createTime);
    v["update_time"] = chen::Time2Str(m_updateTime);
    return chen::JsonUtil::ToString(v);
}

void CategoryInfo::setId(const int64_t& v) {
    m_id = v;
}

void CategoryInfo::setName(const std::string& v) {
    m_name = v;
}

void CategoryInfo::setColor(const std::string& v) {
    m_color = v;
}

void CategoryInfo::setDescription(const std::string& v) {
    m_description = v;
}

void CategoryInfo::setUrl(const std::string& v) {
    m_url = v;
}

void CategoryInfo::setIcon(const std::string& v) {
    m_icon = v;
}

void CategoryInfo::setParentId(const int64_t& v) {
    m_parentId = v;
}

void CategoryInfo::setStatus(const int32_t& v) {
    m_status = v;
}

void CategoryInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void CategoryInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void CategoryInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int CategoryInfoDao::Update(CategoryInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "update category set name = ?, color = ?, description = ?, url = ?, icon = ?, parent_id = ?, status = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_name);
    stmt->bindString(2, info->m_color);
    stmt->bindString(3, info->m_description);
    stmt->bindString(4, info->m_url);
    stmt->bindString(5, info->m_icon);
    stmt->bindInt64(6, info->m_parentId);
    stmt->bindInt32(7, info->m_status);
    stmt->bindInt32(8, info->m_isDeleted);
    stmt->bindTime(9, info->m_createTime);
    stmt->bindTime(10, info->m_updateTime);
    stmt->bindInt64(11, info->m_id);
    return stmt->execute();
}

int CategoryInfoDao::Insert(CategoryInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "insert into category (name, color, description, url, icon, parent_id, status, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, info->m_name);
    stmt->bindString(2, info->m_color);
    stmt->bindString(3, info->m_description);
    stmt->bindString(4, info->m_url);
    stmt->bindString(5, info->m_icon);
    stmt->bindInt64(6, info->m_parentId);
    stmt->bindInt32(7, info->m_status);
    stmt->bindInt32(8, info->m_isDeleted);
    stmt->bindTime(9, info->m_createTime);
    stmt->bindTime(10, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int CategoryInfoDao::InsertOrUpdate(CategoryInfo::ptr info, chen::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into category (id, name, color, description, url, icon, parent_id, status, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindString(2, info->m_name);
    stmt->bindString(3, info->m_color);
    stmt->bindString(4, info->m_description);
    stmt->bindString(5, info->m_url);
    stmt->bindString(6, info->m_icon);
    stmt->bindInt64(7, info->m_parentId);
    stmt->bindInt32(8, info->m_status);
    stmt->bindInt32(9, info->m_isDeleted);
    stmt->bindTime(10, info->m_createTime);
    stmt->bindTime(11, info->m_updateTime);
    return stmt->execute();
}

int CategoryInfoDao::Delete(CategoryInfo::ptr info, chen::IDB::ptr conn) {
    std::string sql = "delete from category where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int CategoryInfoDao::DeleteById( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "delete from category where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int CategoryInfoDao::DeleteByName( const std::string& name, chen::IDB::ptr conn) {
    std::string sql = "delete from category where name = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindString(1, name);
    return stmt->execute();
}

int CategoryInfoDao::QueryAll(std::vector<CategoryInfo::ptr>& results, chen::IDB::ptr conn) {
    std::string sql = "select id, name, color, description, url, icon, parent_id, status, is_deleted, create_time, update_time from category";
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
        CategoryInfo::ptr v(new CategoryInfo);
        v->m_id = rt->getInt64(0);
        v->m_name = rt->getString(1);
        v->m_color = rt->getString(2);
        v->m_description = rt->getString(3);
        v->m_url = rt->getString(4);
        v->m_icon = rt->getString(5);
        v->m_parentId = rt->getInt64(6);
        v->m_status = rt->getInt32(7);
        v->m_isDeleted = rt->getInt32(8);
        v->m_createTime = rt->getTime(9);
        v->m_updateTime = rt->getTime(10);
        results.push_back(v);
    }
    return 0;
}

CategoryInfo::ptr CategoryInfoDao::Query( const int64_t& id, chen::IDB::ptr conn) {
    std::string sql = "select id, name, color, description, url, icon, parent_id, status, is_deleted, create_time, update_time from category where id = ?";
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
    CategoryInfo::ptr v(new CategoryInfo);
    v->m_id = rt->getInt64(0);
    v->m_name = rt->getString(1);
    v->m_color = rt->getString(2);
    v->m_description = rt->getString(3);
    v->m_url = rt->getString(4);
    v->m_icon = rt->getString(5);
    v->m_parentId = rt->getInt64(6);
    v->m_status = rt->getInt32(7);
    v->m_isDeleted = rt->getInt32(8);
    v->m_createTime = rt->getTime(9);
    v->m_updateTime = rt->getTime(10);
    return v;
}

CategoryInfo::ptr CategoryInfoDao::QueryByName( const std::string& name, chen::IDB::ptr conn) {
    std::string sql = "select id, name, color, description, url, icon, parent_id, status, is_deleted, create_time, update_time from category where name = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        ERROR(logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindString(1, name);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    CategoryInfo::ptr v(new CategoryInfo);
    v->m_id = rt->getInt64(0);
    v->m_name = rt->getString(1);
    v->m_color = rt->getString(2);
    v->m_description = rt->getString(3);
    v->m_url = rt->getString(4);
    v->m_icon = rt->getString(5);
    v->m_parentId = rt->getInt64(6);
    v->m_status = rt->getInt32(7);
    v->m_isDeleted = rt->getInt32(8);
    v->m_createTime = rt->getTime(9);
    v->m_updateTime = rt->getTime(10);
    return v;
}

int CategoryInfoDao::CreateTableSQLite3(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE category("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL DEFAULT '',"
            "color TEXT NOT NULL DEFAULT '',"
            "description TEXT NOT NULL DEFAULT '',"
            "url TEXT NOT NULL DEFAULT '',"
            "icon TEXT NOT NULL DEFAULT '',"
            "parent_id INTEGER NOT NULL DEFAULT 0,"
            "status INTEGER NOT NULL DEFAULT 0,"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT current_timestamp,"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE UNIQUE INDEX category_name ON category(name);"
            );
}

int CategoryInfoDao::CreateTableMySQL(chen::IDB::ptr conn) {
    return conn->execute("CREATE TABLE category("
            "`id` bigint AUTO_INCREMENT,"
            "`name` varchar(50) NOT NULL DEFAULT '',"
            "`color` varchar(10) NOT NULL DEFAULT '',"
            "`description` varchar(255) NOT NULL DEFAULT '',"
            "`url` varchar(255) NOT NULL DEFAULT '',"
            "`icon` varchar(255) NOT NULL DEFAULT '',"
            "`parent_id` bigint NOT NULL DEFAULT 0,"
            "`status` int NOT NULL DEFAULT 0 COMMENT '0: 停用 1: 启用',"
            "`is_deleted` int NOT NULL DEFAULT 0,"
            "`create_time` timestamp NOT NULL DEFAULT current_timestamp,"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp ,"
            "PRIMARY KEY(`id`),"
            "UNIQUE KEY `category_name` (`name`))");
}
} //namespace data
} //namespace blog
