/**
 * @file MainCommand.h
 * @brief 主线程命令
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-12
 */
#ifndef _MAIN_COMMAND_H
#define _MAIN_COMMAND_H

#include "tools/GameLog.h"
#include "db/DBHelp.h"
#include <iostream>

namespace Command
{
/**
 * @brief 输出命令
 */
void PrintCommand();

/**
 * @brief 查询sql并打印查询数据
 *
 * @param sql
 */
void SearchSQL(const std::string & sql);

void SetLogLevel(const std::string & level)
{
    GameTools::GameLog::SetLevel(level);
}

bool HandleConsoleInput()
{
    std::string input;
    std::getline(cin, input);

    if(input == "exit")
        return false;
    else if(input == "search")
    {
        std::cout << "输入查询语句：" << std::endl;
        std::getline(cin, input);
        SearchSQL(input);
    }
    else if(input == "set log level")
    {
        std::cout << "输入日志等级：" << std::endl;
        std::getline(cin, input);
        SetLogLevel(input);
    }
    else if(input == "/help")
    {
        PrintCommand();
    }
    else
    {
        std::cout << "没有找到命令，输入 /help 查看帮助" << std::endl;
    }

    return true;
}

void PrintCommand()
{
    std::cout << "Command:" << std::endl;
    std::cout << "\texit\t退出" << std::endl;
    std::cout << "\tsearch\t查询数据库" << std::endl;
    std::cout << "\tset log level\t设置日志等级" << std::endl;
    std::cout << "\t/help\t查看帮助" << std::endl;
}

void SearchSQL(const std::string & sql)
{
    GameDB::QueryData data = GameDB::DBHelp::Instance().QueryRecord(sql);

    if(data.size() == 0)
    {
        std::cout << "无记录" << std::endl;
        return;
    }

    // 打印列名
    for(auto it = data[0].begin(); it != data[0].end(); ++it)
    {
        std::cout << it->first << "\t";
    }
    std::cout << std::endl;
    // 打印行
    for(auto & rows : data)
    {
        for(auto it = rows.begin(); it != rows.end(); ++it)
        {
            std::cout << it->second << "\t";
        }
        std::cout << std::endl;
    }
}

}

#endif // _MAIN_COMMAND_H
