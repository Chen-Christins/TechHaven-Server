#include "my_module.h"
#include "log/log.h"

namespace chat {

static sylar::Logger::ptr logger = LOG_ROOT();

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