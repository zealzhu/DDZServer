/**
* @file GameSocketType
* @brief Define socket type
* @author zhu peng cheng
* @version 1.0
* @date 2017-12-29
*/
#ifndef _GAME_SOCKET_TYPE_H
#define _GAME_SOCKET_TYPE_H

#include <unistd.h>         // gethostname()
#include <sys/socket.h>     // contain socket data types and functions
#include <sys/types.h>      // contain all basic data types and typedefs
#include <errno.h>          // errno
#include <netdb.h>          // gethostbyname()
#include <fcntl.h>          // file control
#include <arpa/inet.h>      // ntohl() htonl()

namespace GameSocketLib
{
    typedef int gsocket;                // define socket type

    typedef unsigned long ipaddress;    // define ip address type
}

#endif //_GAME_SOCKET_TYPE_H
