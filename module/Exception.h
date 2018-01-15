/**
 * @file Exception.h
 * @brief 业务模块异常
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-15
 */
#ifndef _GAME_MODULE_EXCEPTION_H
#define _GAME_MODULE_EXCEPTION_H
#include <exception>

namespace GameModule
{

class Exception : std::exception
{
public:
    Exception(std::string msg) : msg_(msg) {}

    inline const std::string GetMessage() const
    {
        return msg_;
    }

private:
    std::string msg_;
};

} // namespace GameModule

#endif // _GAME_MODULE_EXCEPTION_H

