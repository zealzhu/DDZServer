/**
 * @file ConnectionManager.cpp
 * @brief 连接管理器
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-05
 */
#include "ConnectionManager.h"
#include "Connection.h"

#include <protocol/Protocol.h>
#include <tools/GameLog.h>

namespace GameSocketLib
{
ConnectionManager::ConnectionManager()
{}

void ConnectionManager::AddConnection(DataSocket & sock)
{
    Connection conn(sock);

    if(AvailableConnections() == 0)
    {
        logger_warn("连接已达到最大数量 {}, 不再接受新连接", MAX);
        // 关闭连接
        conn.CloseSocket();
        return;
    }

    // 设置为非阻塞模式
    conn.SetBlocking(false);
    this->socket_set_.AddSocket(conn.GetSocket());
    this->connection_list_.emplace_back( conn );
    logger_info("connection {}:{} 已连接 sock {}", conn.GetRemoteAddress(), conn.GetRemotePort(), conn.GetSocket());
}

void ConnectionManager::SendMsg(gsocket sock, void * msg)
{
    auto it = std::find_if(this->connection_list_.begin(), this->connection_list_.end(),
            [sock](const Connection & conn){
                return conn.GetSocket() == sock;
            });
    if(it == this->connection_list_.end())
        return;

    google::protobuf::Message * protobuf_msg = (google::protobuf::Message *)msg;
    ProtocolLib::Protocol::Encode(*protobuf_msg, it->GetBufferData());
}

void ConnectionManager::Recv()
{
    int size = 0;
    if(TotalConnections() > 0)
    {
        // 轮询是否有数据
        size = this->socket_set_.Poll();
    }

    if(size > 0)
    {
        auto it = this->connection_list_.begin();
        decltype(it) conn_it;

        while(it != this->connection_list_.end())
        {
            conn_it = it++;
            if(this->socket_set_.HasActivity(conn_it->GetSocket()))
            {
                try
                {
                    conn_it->Receive();
                }
                catch(...)
                {
                    // 任何异常都进行关闭连接
                    conn_it->SetShouldClose();
                    this->Close(conn_it);
                }
            }
        }
    }
}

void ConnectionManager::Send()
{
    auto it = this->connection_list_.begin();
    decltype(it) conn_it;

    while(it != this->connection_list_.end())
    {
        conn_it = it++;
        if(this->socket_set_.HasActivity(conn_it->GetSocket()))
        {
        try
        {
            conn_it->SendBuffer();
        }
        catch(...)
        {
            // 任何异常都进行关闭连接
            conn_it->SetShouldClose();
            this->Close(conn_it);
        }
        }
    }
}

void ConnectionManager::Close(CONNECTION_ITER iter)
{
    logger_info("connection {}:{} 关闭, sock {}", iter->GetRemoteAddress(), iter->GetRemotePort(), iter->GetSocket());
    iter->CloseSocket();
    this->connection_list_.erase(iter);
    this->socket_set_.RemoveSocket(iter->GetSocket());
}

void ConnectionManager::CloseConnections()
{
    auto it = this->connection_list_.begin();
    decltype(it) close_it;

    while(it != this->connection_list_.end())
    {
        close_it = it++;
        // 判断是否需要关闭进行关闭
        if(close_it->GetShouldClose() == true)
        {
            this->Close(close_it);
        }
    }
}
}
