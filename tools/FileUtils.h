/**
 * @file FileUtils.h
 * @brief 文件工具
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-03
 */
#ifndef _FILE_UTILS_H
#define _FILE_UTILS_H

#include <INIReader.h>
#include <map>
#include <iostream>
#include <sys/stat.h> // stat()
//#include <sys/type.h> // S_IFDIR
//#include <


namespace GameTools
{

class FileUtils
{
public:
    /**
     * @brief 获取配置map
     *
     * @param file_path 配置文件路径
     *
     * @return
     */
    static std::map<std::string, std::string> GetConfigMap(const std::string & file_path)
    {
        std::map<std::string, std::string> config_map;

        // 打开配置文件
        INIReader reader(file_path);
        if(reader.ParseError() < 0)
        {
            std::cout << "Can't load " << file_path << std::endl;
            return config_map;
        }

        // 网络配置
        config_map["net.port"] = reader.Get("net", "port", "9999");     // 端口

        // 消息队列
        config_map["msg.maxsize"] = reader.Get("msg", "maxsize", "9999");               // 消息队列中最大消息数

        // 数据库配置
        config_map["db.ip"] = reader.Get("db", "ip", "127.0.0.1");                      // 数据库地址
        config_map["db.port"] = reader.Get("db", "port", "3306");                       // 数据库端口
        config_map["db.scheme"] = reader.Get("db", "name", "ddz");                      // 数据库名
        config_map["db.user"] = reader.Get("db", "user", "root");                       // 数据库用户名
        config_map["db.password"] = reader.Get("db", "password", "no080740");           // 数据库密码
        config_map["db.maxpoolsize"] = reader.Get("db", "maxpoolsize", "10");           // 数据库池最大容量
        config_map["db.reconnect"] = reader.Get("db", "reconnect", "true");             // 是否自动重连
        config_map["db.checkinterval"] = reader.Get("db", "checkinterval", "10");       // 检查连接间隔

        // 日志配置
        config_map["log.level"] = reader.Get("log", "level", "trace");                  // 日志输出等级
        config_map["log.enableconsole"] = reader.Get("log", "enableconsole", "true");   // 控制台输出
        config_map["log.async"] = reader.Get("log", "async", "false");                  // 异步
        config_map["log.queue_size"] = reader.Get("log", "queue_size", "4096");         // 异步输出队列大小

        return config_map;
    }

    /**
     * @brief 检查目录是否存在
     *
     * @param dir_path 目录路径
     *
     * @return
     */
    static bool IsDirExist(const std::string & dir_path)
    {
        struct stat info;

        // 读取文件信息
        if(stat(dir_path.c_str(), &info) != 0)
            return false;

        // 检查是否目录
        if(info.st_mode & S_IFDIR)
            return true;

        return false;
    }

    static void CreateDir(const std::string & dir_path)
    {
        mkdir(dir_path.c_str(), 0777);
    }

};
}

#endif // _FILE_UTILS_H
