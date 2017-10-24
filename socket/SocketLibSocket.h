// MUD Programming
// Ron Penton
// (C)2003
// SocketLibSocket.h - This file contains the definitions for the three
// socket classes: Basic, Data, and Listening.


#ifndef SOCKETLIBSOCKET_H
#define SOCKETLIBSOCKET_H

// ========================================================================
//  Include Files
// ========================================================================
#include "SocketLibTypes.h"
#include "SocketLibErrors.h"


namespace SocketLib
{

    // ========================================================================
    // Class:       Socket
    // Purpose:     Socket基类，提供用户基本的方法，如获取端口，ip，关闭连接等
    //              基本操作
    // ========================================================================
    class Socket
    {
    public:
        // ====================================================================
        // Function:    GetSock
        // Purpose:     获取socket
        // ====================================================================
        inline sock GetSock() const
        {
            return m_sock;
        }


        // ====================================================================
        // Function:    GetLocalPort
        // Purpose:     获取本地监听端口
        // ====================================================================
        inline port GetLocalPort() const
        {
            return ntohs( m_localinfo.sin_port );
        }

        // ====================================================================
        // Function:    GetLocalAddress
        // Purpose:     获取本地监听地址
        // ====================================================================
        inline ipaddress GetLocalAddress() const
        {
            return m_localinfo.sin_addr.s_addr;
        }

        // ====================================================================
        // Function:    Close
        // Purpose:     关闭连接
        // ====================================================================
        void Close();

        // ====================================================================
        // Function:    SetBlocking
        // Purpose:     设置连接是否阻塞模式
        // ====================================================================
        void SetBlocking( bool p_blockmode );

  

    protected:
        // ====================================================================
        // Function:    Socket
        // Purpose:     隐藏该构造函数，防止用户直接创建该类，需使用子类进行创建
        // ====================================================================
        Socket( sock p_socket = -1 );


        sock m_sock;                    // this is the underlying representation
                                        // of the actual socket.

        struct sockaddr_in m_localinfo; // structure containing information
                                        // about the local connection

        bool m_isblocking;              // this tells whether the socket is
                                        // blocking or not.
    };




    // ========================================================================
    // Class:       DataSocket
    // Purpose:     A variation of the BasicSocket base class that handles
    //              TCP/IP data connections.
    // ========================================================================
    class DataSocket : public Socket
    {
    public:
        // ====================================================================
        // Function:    DataSocket
        // Purpose:     Constructs the data socket with optional values
        // ====================================================================
        DataSocket( sock p_socket = -1 );

        // ====================================================================
        // Function:    GetRemoteAddress
        // Purpose:     获取远程ip地址
        // ====================================================================
        inline ipaddress GetRemoteAddress() const
        {
            return m_remoteinfo.sin_addr.s_addr;
        }


        // ====================================================================
        // Function:    GetRemotePort
        // Purpose:     获取远程端口
        // ====================================================================
        inline port GetRemotePort() const
        {
            return ntohs( m_remoteinfo.sin_port );
        }


        // ====================================================================
        // Function:    IsConnected
        // Purpose:     当前socket是否已连接
        // ====================================================================
        inline bool IsConnected() const
        {
            return m_connected;
        }


        // ====================================================================
        // Function:    Connect
        // Purpose:     将当前socket连接到另一个socket，如果当前socket已连接将会
		//				失败
        // ====================================================================
        void Connect( ipaddress p_addr, port p_port );

        // ====================================================================
        // Function:    Send
        // Purpose:     发送数据
        // ====================================================================
        int Send( const char* p_buffer, int p_size );

        // ====================================================================
        // Function:    Receive
        // Purpose:     接收数据
        // ====================================================================
        int Receive( char* p_buffer, int p_size );

        // ====================================================================
        // Function:    Close
        // Purpose:     关闭连接
        // ====================================================================
        void Close();

    protected:

        bool m_connected;               // is the socket connected?

        struct sockaddr_in m_remoteinfo;// structure containing information
        // about the remote connection

    };



    // ========================================================================
    // Class:       ListeningSocket
    // Purpose:     监听socket，用于监听客户端连接
    // ========================================================================
    class ListeningSocket : public Socket
    {
    public:

        // ====================================================================
        // Function:    ListeningSocket
        // Purpose:     Constructor. Constructs the socket with initial values
        // ====================================================================
        ListeningSocket();

        // ====================================================================
        // Function:    Listen
        // Purpose:     监听指定端口
        // p_port:      This is the port that the socket will listen on.
        // ====================================================================
        void Listen( port p_port );

        // ====================================================================
        // Function:    Accept
        // Purpose:     This is a blocking function that will accept an 
        //              incomming connection and return a data socket with info
        //              about the new connection.
        // ====================================================================
        DataSocket Accept();

        // ====================================================================
        // Function:    IsListening
        // Purpose:     获取是否监听
        // ====================================================================
        inline bool IsListening() const
        {
            return m_listening;
        }


        // ====================================================================
        // Function:    Close
        // Purpose:     关闭socket
        // ====================================================================
        void Close();

    protected:
        bool m_listening;               // is the socket listening?

    };


}   // end namespace SocketLib


#endif
