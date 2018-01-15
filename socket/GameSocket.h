/**
 * @file GameSocket.h
 * @brief
 * @author zhu peng cheng
 * @version 1.0
 * @date 2017-12-29
 */

#ifndef _GAME_SOCKET_H
#define _GAME_SOCKET_H

#include "GameSocketType.h"

namespace GameSocketLib
{
    /**
     * @brief The base socket
     */
    class BaseSocket
    {
        public:
            /**
             * @brief 获取socket句柄
             *
             * @return
             */
            inline gsocket GetSocket() const
            {
                return socket_;
            }

            /**
             * @brief 获取当前端口
             *
             * @return
             */
            inline unsigned int GetLocalPort() const
            {
                return ntohs(local_info_.sin_port);
            }

            /**
             * @brief 获取当前地址
             *
             * @return
             */
            inline ipaddress GetLocalAddress() const
            {
                return local_info_.sin_addr.s_addr;
            }

            /**
             * @brief 关闭socket连接
             */
            void Close();

            /**
             * @brief 设置是否阻塞
             *
             * @param
             */
            void SetBlocking(bool is_blocking);

        protected:
            BaseSocket(gsocket socket = -1);    // 不能手动创建，通过子类进行创建

            gsocket socket_;                    // socket句柄
            struct sockaddr_in local_info_;     // socket信息
            bool blocking_;                     // 是否阻塞
    };


    class DataSocket;
    /**
     * @brief Listening socket
     */
    class ListeningSocket : public BaseSocket
    {
    public:
        ListeningSocket();

        /**
         * @brief 设置监听端口
         *
         * @param port 端口号
         */
        void Listen(unsigned int port);

        /**
         * @brief 等待客户端连接
         *
         * @return 连接socket
         */
        DataSocket Accept();

        /**
         * @brief 关闭连接
         */
        void Close();

    protected:
        bool is_listening_;                     // 是否监听
    };

    /**
     * @brief The socket has connected remote
     */
    class DataSocket : public BaseSocket
    {
    public:
        DataSocket(gsocket socket = -1);

        /**
         * @brief 获取远程连接ip
         *
         * @return
         */
        inline ipaddress GetRemoteAddress() const
        {
            return this->remote_info_.sin_addr.s_addr;
        }

        /**
         * @brief 获取运程连接端口
         *
         * @return
         */
        inline unsigned int GetRemotePort() const
        {
            return ntohs(this->remote_info_.sin_port);
        }

        /**
         * @brief 是否建立连接
         *
         * @return
         */
        inline bool IsConnected() const
        {
            return this->is_connected_;
        }

        /**
         * @brief 请求建立远程连接, 客户端使用
         *
         * @param ip 连接ip
         * @param port 连接端口
         */
        void Connect(ipaddress ip, unsigned int port);

        /**
         * @brief 发送数据
         *
         * @param buf 发送的数据
         * @param size 发送的大小
         *
         * @return 返回实际发送大小
         */
        int Send(const char * buf, unsigned int size);

        /**
         * @brief 接收数据
         *
         * @param buf 接收数据缓冲区
         * @param size 缓冲区大小
         *
         * @return 返回实际接收数据大小
         */
        int Receive(char * buf, unsigned int size);

        /**
         * @brief 关闭与远端的连接
         */
        void Close();

    protected:
        bool is_connected_;                     // 是否建立连接
        struct sockaddr_in remote_info_;       // 远程连接的socket信息
    };
}
#endif // _GAME_SOCKET_H
