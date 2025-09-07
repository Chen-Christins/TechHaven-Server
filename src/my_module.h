/**
 * @file my_module.h
 * @brief 项目模块
 * @author Christins
 * @date 2025-05-10
 * @copyright Apache 2.0
 */
#ifndef __BLOG_MY_MODULE_H__
#define __BLOG_MY_MODULE_H__

#include <chen/module.h>

namespace blog {

class MyModule : public chen::Module {
public:
    typedef std::shared_ptr<MyModule> ptr;
    MyModule();
    bool onLoad() override;
    bool onUnload() override;
    bool onServerReady() override;
    bool onServerUp() override;
};

}

#endif // __BLOG_MY_MODULE_H__