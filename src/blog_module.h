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
#include <chen/http/http_server.h>
#include <chen/http/ws_server.h>

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
     * @brief 定时器回调
     */
    void onTick() override;

    /**
     * @brief 设置定时器间隔
     * @return uint64_t 
     */
    uint64_t getTickIntervalMs() override;

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

    /**
     * @brief 热重载排空阶段（蓝绿部署）：关闭 WS 连接、停止定时器
     * @return bool
     */
    bool onDrain() override;

    /**
     * @brief 热重载排空完成（蓝绿部署）：释放非 dispatch 资源
     * @return bool
     */
    bool onGracefulUnload() override;

private:

    /**
     * @brief 初始化数据库
     */
    bool initMySQL();

    /**
     * @brief 注册Servlet
     */
    void registerServlets();

    /**
     * @brief 注册WebSocket Servlet
     */
    void registerWSServlets();

    /**
     * @brief 清空所有已注册的 HTTP Servlet 路由
     */
    void unregisterServlets();

    /**
     * @brief 清空所有已注册的 WebSocket Servlet 路由
     */
    void unregisterWSServlets();

private:
    /// 持有的 HTTP Server 列表（用于 onUnload 中注销 Servlet）
    std::vector<chen::http::HttpServer::ptr> m_httpServers;
    /// 持有的 WebSocket Server 列表（用于 onUnload 中注销 WS Servlet）
    std::vector<chen::http::WSServer::ptr> m_wsServers;
};

}

#endif // __BLOG_MY_MODULE_H__