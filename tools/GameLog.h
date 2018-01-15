/**
 * @file GameLog.h
 * @brief 游戏日志类
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-03
 */
#ifndef _GAME_LOG_H
#define _GAME_LOG_H

#include <spdlog/spdlog.h>

// {文件}::{函数}::{行}#{输出信息}
// ## 用来去除空参数时前面的逗号
#define logger_trace(fmt, ...) GameTools::logger->GetLogger()->trace("{}::{}()#{}: " fmt, __FILE__ , __FUNCTION__, __LINE__, ## __VA_ARGS__)
#define logger_debug(fmt, ...) GameTools::logger->GetLogger()->debug("{}::{}()#{}: " fmt, __FILE__ , __FUNCTION__, __LINE__, ## __VA_ARGS__)
#define logger_info(fmt, ...) GameTools::logger->GetLogger()->info("{}::{}()#{}: " fmt, __FILE__ , __FUNCTION__, __LINE__, ## __VA_ARGS__)
#define logger_warn(fmt, ...) GameTools::logger->GetLogger()->warn("{}::{}()#{}: " fmt, __FILE__ , __FUNCTION__, __LINE__, ## __VA_ARGS__)
#define logger_error(fmt, ...) GameTools::logger->GetLogger()->error("{}::{}()#{}: " fmt, __FILE__ , __FUNCTION__, __LINE__, ## __VA_ARGS__)
#define logger_critical(fmt, ...) GameTools::logger->GetLogger()->critical("{}::{}()#{}: " fmt, __FILE__ , __FUNCTION__, __LINE__, ## __VA_ARGS__)

namespace GameTools
{

/**
 * @brief 游戏日志类
 */
class GameLog
{
public:

    static bool Init(const std::string & file_name, int size_mb, int number_files);

    static void SetLevel(const std::string & level);

    void SetLogger(std::shared_ptr<spdlog::logger> & logger)
    {
        this->logger_ = logger;
    }

    std::shared_ptr<spdlog::logger> GetLogger();

    ~GameLog();

private:
    GameLog();
    GameLog & operator=(GameLog const &) = delete;

    std::shared_ptr<spdlog::logger> logger_;
};

extern std::unique_ptr<GameLog> logger;

} // namespace GameTools

#endif

