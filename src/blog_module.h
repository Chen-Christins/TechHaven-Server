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
#include <chen/tcp/tcp_server.h>

namespace blog {

class BlogModule : public chen::Module {
public:
    typedef std::shared_ptr<BlogModule> ptr;
    /**
     * @brief 构造函数
     */
    BlogModule();

    /**
     * @brief 模块加载
     * @return bool
     */
    bool onLoad() override;

    /**
     * @brief 模块卸载
     * @return bool
     */
    bool onUnload() override;

    /**
     * @brief 服务器就绪
     * @return bool
     */
    bool onServerReady() override;

    /**
     * @brief 服务器启动
     * @return bool
     */
    bool onServerUp() override;

private:

    /**
     * @brief 初始化数据库
     */
    bool initMySQL();

    /**
     * @brief 加载所有数据到内存
     */
    void loadAllData();

    /**
     * @brief 注册Servlet
     */
    void registerServlets(std::vector<chen::TcpServer::ptr>& servers);

    /**
     * @brief 注册WebSocket Servlet
     */
    void registerWSServlets(std::vector<chen::TcpServer::ptr>& servers);
};

}

#endif // __BLOG_MY_MODULE_H__