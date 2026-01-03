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

class BlogModule : public chen::Module {
public:
    typedef std::shared_ptr<BlogModule> ptr;
    BlogModule();
    bool onLoad() override;
    bool onUnload() override;
    bool onServerReady() override;
    bool onServerUp() override;
};

}

#endif // __BLOG_MY_MODULE_H__