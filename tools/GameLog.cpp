/**
 * @file GameLog.cpp
 * @brief 游戏日志类
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-03
 */
#include "GameLog.h"
#include "ConfigManager.h"
#include "FileUtils.h"

namespace GameTools
{

std::unique_ptr<GameLog> logger;

GameLog::GameLog()
{

}

GameLog::~GameLog()
{
    std::cout << "clean spdlog" << std::endl;
    std::string async = ConfigManager::GetConfigParam("log.async", "false");
    // 如果是异步的立即刷新缓冲区
    if(async == "true")
    {
        this->logger_->flush();
    }
}

bool GameLog::Init(const std::string & file_name, int size_mb, int number_files)
{
    // 检测日志目录
    if(!FileUtils::IsDirExist("log"))
    {
        FileUtils::CreateDir("log");
    }

    // 检测是否已经有日志实例说明已经初始化过了
    if(logger)
        return true;

    // 创建新的日志实例
    logger.reset(new GameLog);

    try
    {
        std::vector<spdlog::sink_ptr> sinks;
        // 输出到文件
        sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>
                (file_name, size_mb * 1024L * 1024, number_files));
        // 输出到控制台
        if(ConfigManager::GetConfigParam("log.enableconsole", "true") == "true")
        {
            // 直接控制台没有颜色
            // auto stdout_sink = spdlog::sinks::stdout_sink_mt::instance();
            auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
            sinks.push_back(color_sink);
        }

        // 创建spdlog
        auto combined_logger = std::make_shared<spdlog::logger>("server", begin(sinks), end(sinks));

        // 异步
        if(ConfigManager::GetConfigParam("log.async", "false") == "true")
        {
            std::string size = ConfigManager::GetConfigParam("log.queue_size", "4096");
            spdlog::set_async_mode(atoi(size.c_str()));
        }

        // [时间(到ms)] [线程号] [日志等级] 输出内容
        spdlog::set_pattern("[%Y-%m-%d.%e %H:%M:%S] [%t] [%l] %v");

        spdlog::register_logger(combined_logger);

        logger->SetLogger(combined_logger);

        // 读取设置日志等级
        SetLevel(ConfigManager::GetConfigParam("log.level", "trace"));

        return true;
    }
    catch(const spdlog::spdlog_ex & ex)
    {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
        return false;
    }
}

void GameLog::SetLevel(const std::string & level)
{
    if(!logger)
        return;

    // 日志等级
    spdlog::level::level_enum spd_level = spdlog::level::level_enum::err;

    if(level == "trace")
    {
        spd_level = spdlog::level::level_enum::trace;
    }
    else if(level == "debug")
    {
        spd_level = spdlog::level::level_enum::debug;
    }
    else if(level == "info")
    {
        spd_level = spdlog::level::level_enum::info;
    }
    else if(level == "warn")
    {
        spd_level = spdlog::level::level_enum::warn;
    }
    else if(level == "error")
    {
        spd_level = spdlog::level::level_enum::err;
    }
    else if(level == "critical")
    {
        spd_level = spdlog::level::level_enum::critical;
    }
    else if(level == "off")
    {
        spd_level = spdlog::level::level_enum::off;
    }

    logger->GetLogger()->set_level(spd_level);

    logger->GetLogger()->flush_on(spd_level);
}

std::shared_ptr<spdlog::logger> GameLog::GetLogger()
{
    if(!this->logger_)
    {
        std::cout << "Not initialization spdlog" << std::endl;
        assert(0);
    }
    return this->logger_;
}

}
