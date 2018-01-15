#ifndef _GAME_DB_EXCEPTION_H
#define _GAME_DB_EXCEPTION_H
#include <exception>

namespace GameDB
{

enum ErrorCode
{
    kNoAvailiableConnection,
    kInsertError,
    kUpdateError,
};

class Exception : std::exception
{
public:
    Exception(ErrorCode code) : error_code_(code) {}

    inline const char * ErrorMessage() const
    {
        switch(this->error_code_)
        {
        case ErrorCode::kNoAvailiableConnection:
            return "无可用连接";
        case ErrorCode::kInsertError:
            return "插入错误";
        case ErrorCode::kUpdateError:
            return "更新错误";
        default:
            return "未知错误";
        }
    }

    inline ErrorCode GetCode() const { return this->error_code_; }

private:
    ErrorCode error_code_;
};

} // namespace GameDB

#endif // _GAME_DB_EXCEPTION_H
