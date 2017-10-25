////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhu
// Created： 2017/10/25
// Describe：游戏日志类
////////////////////////////////////////////////////////////////////////
#ifndef _GAME_LOG_H
#define _GAME_LOG_H

#include <spdlog/spdlog.h>

#define logger_trace(fmt, ...)	 logger->getSpdlogger()->trace("{}::{}()#{}: " fmt, __FILENAME__ , __FUNCTION__, __LINE__, __VA_ARGS__)
#define logger_debug(fmt, ...)	 logger->getSpdlogger()->debug("{}::{}()#{}: " fmt, __FILENAME__ , __FUNCTION__, __LINE__, __VA_ARGS__)
#define logger_info(fmt, ...)	 logger->getSpdlogger()->info("{}::{}()#{}: " fmt, __FILENAME__ , __FUNCTION__, __LINE__, __VA_ARGS__)
#define logger_warn(fmt, ...)	 logger->getSpdlogger()->warn("{}::{}()#{}: " fmt, __FILENAME__ , __FUNCTION__, __LINE__, __VA_ARGS__)
#define logger_error(fmt, ...)	 logger->getSpdlogger()->error("{}::{}()#{}: " fmt, __FILENAME__ , __FUNCTION__, __LINE__, __VA_ARGS__)
#define logger_critical(fmt, ...) logger->getSpdlogger()->critical("{}::{}()#{}: " fmt, __FILENAME__ , __FUNCTION__, __LINE__, __VA_ARGS__)

#ifdef _DEBUG
#define __FILENAME__ (strrchr("\\" __FILE__,'\\')+1)
#else
#define __FILENAME__ (strrchr("/" __FILE__,'/')+1)
#endif

class CGameLog
{
public:
	// 初始化
	static bool initLogger(const std::string & fileName, int sizeMb, int numFiles);
	
	// 设置日志级别
	static void setLogLevel(std::string & level);

	std::shared_ptr<spdlog::logger> & getSpdlogger();

	void setSpdLogger(std::shared_ptr<spdlog::logger> & spdLogger) {
		this->m_spdLogger = spdLogger;
	}

	virtual ~CGameLog();
private:
	CGameLog();

	// assignment is private.
	CGameLog& operator=(CGameLog const&) {};

private:
	std::shared_ptr<spdlog::logger> m_spdLogger;
};

extern std::unique_ptr<CGameLog> logger;

#endif // !_GAME_LOG_H



