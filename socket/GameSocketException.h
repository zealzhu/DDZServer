/**
 * @file GameSocketException.h
 * @brief
 * @author zhu peng cheng
 * @version 1.0
 * @date 2017-12-29
 */
#ifndef _GAME_SOCKET_EXPECTION_H
#define _GAME_SOCKET_EXPECTION_H

#include "GameSocketType.h"
#include <exception>
#include <string.h>           // strerror()

namespace GameSocketLib
{

enum SocketError
{
    kHasConnected = 500,        // 已连接
    kNotConnected,              // 未连接
    kSocketLimitReached,        // socket连接上限
};

class GameSocketException : public std::exception
{
public:

    GameSocketException()
    {
        error_code_ = SocketError(errno);
    }

    GameSocketException(SocketError err_code) : error_code_(err_code)
    {}

    inline int ErrorCode() const
    {
        return this->error_code_;
    }

    inline const char * PrintError() const
    {
        SocketError err_code = SocketError(error_code_);
        switch(err_code)
        {
        case kHasConnected:
            return "Has connected.";
        case kNotConnected:
            return "Not connected.";
        default:
            return strerror(this->error_code_);
        }
    }

protected:
    SocketError error_code_;
};

}
#endif //_GAME_SOCKET_EXPECTION_H

