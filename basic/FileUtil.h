////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhu
// Created： 2017/10/25
// Describe：文件帮组类
////////////////////////////////////////////////////////////////////////
#ifndef _FILE_UTIL_H
#define _FILE_UTIL_H

#include "inih/cpp/INIReader.h"
#include <direct.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

namespace basic 
{

	class CFileUtil 
	{
	public:
		// 获取本地配置表
		static std::map<string, string> getConfigMap(const string & filePath) {
			map<string, string> configMap;
			INIReader reader(filePath.c_str());

			// 无法加载
			if (reader.ParseError() < 0) {
				std::cout << "Can't load '" << filePath.c_str() << "'" << endl;
				return configMap;
			}

			// 加载配置
			configMap["port"] = reader.Get("net", "port", "9876");
			configMap["maxdatarate"] = reader.Get("net", "maxdatarate", "1024");
			configMap["sentimeout"] = reader.Get("net", "sentimeout", "60");
			configMap["maxbuffered"] = reader.Get("net", "maxbuffered", "65536");

			configMap["msg.queue.size"] = reader.Get("server", "msg.queue.size", "9999");

			configMap["db.ip"] = reader.Get("db", "db.ip", "127.0.0.1");
			configMap["db.port"] = reader.Get("db", "db.port", "3306");
			configMap["db.name"] = reader.Get("db", "db.name", "ddz");
			configMap["db.maxpoolsize"] = reader.Get("db", "db.maxpoolsize", "10");
			configMap["db.idle.check.interval"] = reader.Get("db", "db.idle.check.interval", "10");

			configMap["log.level"] = reader.Get("log", "log.level", "error");
			configMap["log.enableconsole"] = reader.Get("log", "log.enableconsole", "false");
			configMap["log.async"] = reader.Get("log", "log.async", "false");

			configMap["memory.leak.detect"] = reader.Get("memory", "memory.leak.detect", "false");
			configMap["memory.leak.output"] = reader.Get("memory", "memory.leak.output", "DEBUG");

			return configMap;
		}

		// 检查目录是否存在
		static bool isDirExist(string pathName) {
			struct stat info;

			if (stat(pathName.c_str(), &info) != 0)
				return false;

			if (info.st_mode & S_IFDIR)
				return true;
			else
				return false;
		}

		// 创建目录
		static void createDir(string strPath) {
#if defined(_WIN32)
			_mkdir(strPath.c_str());
#else 
			mkdir(strPath.c_str(), 0777); // notice that 777 is different than 0777
#endif
		}
	};
}

#endif