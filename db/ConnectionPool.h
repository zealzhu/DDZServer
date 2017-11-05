////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhou du
// Created： 2017/07/10
// Describe：
////////////////////////////////////////////////////////////////////////

#ifndef CONNECTIONPOOL_H
#define CONNECTIONPOOL_H

#include "../thread/ThreadLibMutex.h"

//c++
#include <iostream>
#include <string>
#include <list>
#include <memory>
#include <functional>
//mysql driver
#include <mysql_driver.h>
#include <mysql_connection.h>
//mysql execute
#include <cppconn/driver.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <exception>
//thread mutex
#include <mutex>

using namespace sql;
using delFunc = std::function<void(sql::Connection*)>;

/**
 * 数据库连接池
 */
class CConnectionPool
{

public:
	//获取唯一实例
	static CConnectionPool& Instance();	

	//得到一条连接
	auto GetConnect()->std::shared_ptr<sql::Connection>;

	//归还一条连接
	auto ReturnConnect(std::shared_ptr<sql::Connection> &ret)->void;

	//获取可用连接数
	auto GetAvailableConSize()->int;

	void HandleException(std::shared_ptr<sql::Connection> pCon, sql::SQLException& e);
private:
	CConnectionPool(SQLString name, SQLString pwd, SQLString url, SQLString dbName, int maxSize);
	//初始化连接池
	auto InitConnectPool(int initialSize)->void;
	//销毁连接池
	auto DestoryPool()->void;
	//销毁一个连接
	auto DestoryOneConn()->void;
	//扩大数据库连接池
	auto ExpandPool(int size)->void;
	//缩小数据库连接池
	auto ReducePool(int size)->void;
	//add conn
	auto AddConn(int size)->void;
	//减少连接数
	void ShrinkConnectNum();

	CConnectionPool();
	virtual ~CConnectionPool();
	CConnectionPool(const CConnectionPool& connectionPool);
	CConnectionPool& operator=(const CConnectionPool& connectionPool);

	const SQLString m_strUserName; //帐号
	const SQLString m_strPassword; //密码
	const SQLString m_strUrl;      //连接url
	const SQLString m_strDbName;   //数据库名
	int m_iMaxPoolSize;       //pool size 连接池中定义的最大数据库连接数
	int m_iCurPoolSize;//当前已建立的数据库连接数量  
	//存放所有连接
	std::list<std::shared_ptr<sql::Connection>> m_ConList;
	ThreadLib::Mutex m_Lock;//锁
	Driver *m_pDriver;//mysql driver
	bool threadFinished;
	std::thread m_idleConCheckThread;//定期检查是否需要关闭空闲数据库连接线程
};
#endif // _MYSQL_CONNECTION_
