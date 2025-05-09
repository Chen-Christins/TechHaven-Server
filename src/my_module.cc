#include "my_module.h"
#include "log/log.h"
#include "db/sqlite3.h"
#include "config/config.h"
#include "blog/data/user_info.h"

namespace chat {

static sylar::Logger::ptr logger = LOG_ROOT();
static sylar::ConfigVar<std::string>::ptr sqlite3_db_name = 
    sylar::Config::Lookup("sqlite3.db_name", std::string("blog.db"), "sqlite3 db file name");

MyModule::MyModule()
    :sylar::Module("Blog", "1.0", "") {
}

bool MyModule::onLoad() {
    INFO(logger) << "onLoad";
    return true;
}

bool MyModule::onUnload() {
    INFO(logger) << "onUnload";
    return true;
}

bool MyModule::onServerReady() {
    INFO(logger) << "onServerReady";

    auto work_path = sylar::Config::Lookup<std::string>("server.work_path");
    auto db_path = work_path->getValue() + "/" + sqlite3_db_name->getValue();

    sylar::SQLite3::ptr db;
    db = sylar::SQLite3::Create(db_path, sylar::SQLite3::READWRITE);
    if (!db) {
        INFO(logger) << "init database";
        db = sylar::SQLite3::Create(db_path);
        if (!db) {
            INFO(logger) << "open database db=" << db_path
                << " failed";
            return false;
        }
        if (blog::data::UserInfoDao::CreateTableSQLite3(db)) {
            ERROR(logger) << "create table user failed";
            return false;
        }
        INFO(logger) << "init database end";
    }

    return true;
}

bool MyModule::onServerUp() {
    INFO(logger) << "onServerUp";
    return true;
}

}

extern "C" {

sylar::Module* CreateModule() {
    sylar::Module* module = new chat::MyModule;
    INFO(chat::logger) << "CreateModule " << module;
    return module;
}

void DestoryModule(sylar::Module* module) {
    INFO(chat::logger) << "DestoryModule " << module;
    delete module;
}

}