/**
 * @file Server.cpp
 * @brief
 * @author zhu peng cheng
 * @version 1.0
 * @date 2017-12-29
 */

#include "tools/ConfigManager.h"
#include "tools/GameLog.h"
#include "protocol/Protocol.h"
#include "socket/GameSocketLib.h"
#include "msg/MessageManager.h"
#include "module/ModuleManager.h"
#include "db/DBConnectionPool.h"
#include "MainCommand.h"

#include <iostream>
#include <thread>

bool done = false;

/**
 * @brief 用来处理接收的数据
 */
void ReceiveThreadTask()
{
    logger_debug("启动接收线程");

    auto & msg_manager = MessageManager::Instance();
    while(!done)
    {
        try
        {
            if(msg_manager.IsReceiveDequeEmpty())
            {
                std::this_thread::yield();
                continue;
            }

            auto msg = msg_manager.GetReceiveMessage();

            if(nullptr == msg.get())
            {
                logger_error("Receive msg is nullptr");
                return;
            }
            logger_debug("开始处理接收的消息, 类型：{}", msg->GetTypeName());

            // 传递给业务处理模块进行处理
            GameModule::ModuleManager::Instance().DispatchMessage(msg);
        }
        catch(...)
        {
            logger_error("Receive error");
        }
    }
    logger_debug("接收线程退出");
}

/**
 * @brief 用来处理要发送的数据
 */
void SendThreadTask(GameSocketLib::ConnectionManager * conn_manager)
{
    logger_debug("启动发送线程");

    auto & msg_manager = MessageManager::Instance();
    while(!done)
    {
        try
        {
            if(msg_manager.IsResponseDequeEmpty())
            {
                std::this_thread::yield();
                continue;
            }

            auto msg = msg_manager.GetResponseMessage();
            // 消息指针强转
            std::shared_ptr<zhu::SelfDescribingMessage> resp_msg = std::dynamic_pointer_cast<zhu::SelfDescribingMessage>(msg);

            if(nullptr == msg.get())
            {
                 logger_error("消息指针转换失败");
                 continue;
            }

            // 发送给所有要发送的连接
            for(int i = 0; i < resp_msg->socket_size(); i++)
            {
                conn_manager->SendMsg(resp_msg->socket(i), msg.get());
            }
        }
        catch(...)
        {
            logger_error("发送消息出错");
        }
    }
    logger_debug("发送线程退出");
}

/**
 * @brief 用来接收连接
 */
void NewConnectThreadTask(GameSocketLib::ListeningManager * listen_manager)
{
    logger_debug("启动连接线程");

    while(!done)
    {
        try
        {
            listen_manager->Listen();
            std::this_thread::yield();
        }
        catch(...)
        {
            logger_error("监听新连接出错");
        }
    }
    logger_debug("连接线程退出");
}

/**
 * @brief 用来处理已连接的socket
 */
void SocketManagerThreadTask(GameSocketLib::ConnectionManager * conn_manager)
{
    logger_debug("启动连接管理线程");
    while(!done)
    {
        try
        {
            conn_manager->Manager();
            std::this_thread::yield();
        }
        catch(...)
        {
            logger_error("ConnectionManager::Manager error");
        }
    }
    logger_debug("连接管理线程退出");
}


/**
 * @brief Main fucntion
 *
 * @param argc argument count
 * @param argv[] arument params
 *
 * @return exit value
 */
int main(int argc, char * argv[])
{
    // 初始化日志
    GameTools::GameLog::Init("log/server.log", 10, 10);

    // 初始化消息队列
    MessageManager::Instance().Init();

    // 创建监听以及连接管理器
    GameSocketLib::ConnectionManager connection_manager;
    GameSocketLib::ListeningManager listen_manager;

    // 监听端口
    unsigned int port = atoi(GameTools::ConfigManager::GetConfigParam("net.port", "9999").c_str());
    listen_manager.AddPort(port);
    listen_manager.SetConnectionManager(&connection_manager);

    // 初始化模块管理器
    auto & module_manager = GameModule::ModuleManager::Instance();
    module_manager.SetConnectionManager(connection_manager);
    module_manager.InitModules();

    // 初始化数据库连接池
    GameDB::DBConnectionPool::Instance();
    GameDB::DBHelp::Instance();

    // 创建服务线程
    std::vector<std::thread> threads;
    threads.push_back(std::thread(NewConnectThreadTask, &listen_manager));
    threads.push_back(std::thread(SocketManagerThreadTask, &connection_manager));
    threads.push_back(std::thread(ReceiveThreadTask));
    threads.push_back(std::thread(SendThreadTask, &connection_manager));

    // 主线程用来接收输入
    while(Command::HandleConsoleInput())
    {
        std::this_thread::yield();
    }

    // 线程退出标志
    done = true;

    // 等待线程退出
    for(auto & thread : threads)
    {
        thread.join();
    }
    return 0;
}
