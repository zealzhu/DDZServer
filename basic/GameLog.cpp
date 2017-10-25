#include "GameLog.h"
#include "FileUtil.h"
#include <vector>
#include "ConfigMgr.h"
#include <iostream>

std::unique_ptr<CGameLog> logger;

bool CGameLog::initLogger(const std::string & fileName, int sizeMb, int numFiles)
{
	// 日志目录不存在则先创建目录
	if (!basic::CFileUtil::isDirExist("./log"))
		basic::CFileUtil::createDir("./log");

	if (logger)
		return true;

	// 创建CGamelog
	logger.reset(new CGameLog);

	try {
		// 指向实际输出的目录
		std::vector<spdlog::sink_ptr> sinks;
		
		// 控制台输出日志开关
		string log2Console = basic::CConfigMgr::getConfigParam("log.enableconsole", "true");
		if (log2Console == "true") {
#ifdef WIN32
			sinks.push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>());
#else
			sinks.push_back(std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>());
#endif
		}

		// 输出到文件
		sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>
			(fileName, sizeMb * 1024L * 1024, numFiles));

		// 创建logger
		auto spdLogger = std::make_shared<spdlog::logger>("server", begin(sinks), end(sinks));

		// 设置日期格式
		spdLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%l] %v");

		// 将该spdLogger设置到logger
		logger->setSpdLogger(spdLogger);

		// 注册logger
		spdlog::register_logger(logger->getSpdlogger());

		// 同步输出开关
		string logAsync = basic::CConfigMgr::getConfigParam("log.async", "false");
		if (logAsync == "true")
		{
			string q_size = basic::CConfigMgr::getConfigParam("log.q_size", "4096");
			spdlog::set_async_mode(atoi(q_size.c_str()));
		}

		// 设置输出等级
		string logLevel = basic::CConfigMgr::getConfigParam("log.level", "error");
		setLogLevel(logLevel);
	}
	catch (const spdlog::spdlog_ex& ex) {
		std::cout << "logger initialization failed: " << ex.what() << std::endl;
		return false;
	}
	return true;
}

void CGameLog::setLogLevel(std::string & level)
{
	if (!logger) return;

	spdlog::level::level_enum spdlevel = spdlog::level::level_enum::err;
	if (level == "trace") {
		spdlevel = spdlog::level::level_enum::trace;
	}
	else if (level == "debug") {
		spdlevel = spdlog::level::level_enum::debug;
	}
	else if (level == "info") {
		spdlevel = spdlog::level::level_enum::info;
	}
	else if (level == "warn") {
		spdlevel = spdlog::level::level_enum::warn;
	}
	else if (level == "err") {
		spdlevel = spdlog::level::level_enum::err;
	}
	else if (level == "critical") {
		spdlevel = spdlog::level::level_enum::critical;
	}
	else if (level == "off") {
		spdlevel = spdlog::level::level_enum::off;
	}

	logger->getSpdlogger()->set_level(spdlevel);

	// debug时，立即输出日志
#ifdef _DEBUG
#else
	logger->getSpdlogger().get()->flush_on(spdlevel);
#endif // _DEBUG
}

std::shared_ptr<spdlog::logger>& CGameLog::getSpdlogger()
{
	if (!m_spdLogger) {
		// 未初始化spdlog
		std::cout << "logger not initialized" << std::endl;
		assert(0);
	}
	return m_spdLogger;
}

CGameLog::~CGameLog()
{
	std::cout << "tear down CGameLog" << std::endl;
	string logAsync = basic::CConfigMgr::getConfigParam("log.async", "false");
	if (logAsync == "true")
	{
		m_spdLogger->flush();
	}
}

CGameLog::CGameLog() {}
