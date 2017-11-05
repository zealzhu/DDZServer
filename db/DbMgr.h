////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhou du
// Created： 2017/07/10
// Describe：数据库帮助类 主要实现增删改查
////////////////////////////////////////////////////////////////////////

#ifndef DBMGR_H
#define DBMGR_H

#include <map>
#include <vector>
#include <string>
#include <sstream>
//mysql driver
#include <mysql_connection.h>
//mysql execute
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <exception>

#include "../basic/Basic.h"

using namespace std;

////////////////////////////////////////////////////////////////
//数据库异常信息
/////////////////////////////////////////////////////////////////
struct CDbException
{
	CDbException(const string &sBuffer) :errorInfo(sBuffer) {};
	~CDbException() throw() {};
	string errorInfo;
};

////////////////////////////////////////////////////////////////
//数据库操作类 
//
/////////////////////////////////////////////////////////////////
class DbMgr
{
public:
	//定义字段类型，
	enum DBTYPE
	{
		DB_INT, //数字类型
		DB_STR, //字符串类型
		DB_FLOAT//浮点型
	};

	//数据记录
	typedef map<string, pair<DBTYPE, string> > RECORD_DATA;
	//查询数据库返回类型
	typedef vector<map<string, string> > DB_DATA;
public:
	//测试连接
	void TestConnect();

	static DbMgr *GetInstance();

	static void DestoryInstance();

	////////////////////////////////////////////////////////////////
	//函数介绍：插入记录. 
	//输入参数：
	//sTableName 表名
	//mpColumns 列名/值对
	//返回值： size_t 影响的行数
	/////////////////////////////////////////////////////////////////
	size_t InsertRecord(const string &strTableName, const RECORD_DATA &mpColumns);

	////////////////////////////////////////////////////////////////
	//函数介绍：构造Insert-SQL语句. 
	//输入参数：
	//sTableName 表名
	//mpColumns 列名/值对
	//返回值：string insert-SQL语句
	/////////////////////////////////////////////////////////////////
	string BuildInsertSQL(const string &strTableName, const RECORD_DATA &mpColumns);

	////////////////////////////////////////////////////////////////
	//函数介绍：删除记录. 
	//输入参数：
	// sTableName 表名
	// sCondition where子语句,例如:where A = B
	//返回值：size_t 影响的行数
	/////////////////////////////////////////////////////////////////
	size_t DeleteRecord(const string &strTableName, const string &strCondition = "");

	////////////////////////////////////////////////////////////////
	//函数介绍：更新记录. 
	//输入参数：
	//sTableName 表名
	//mpColumns 列名/值对
	//sCondition where子语句,例如:where A = B
	//返回值：size_t 影响的行数
	/////////////////////////////////////////////////////////////////
	size_t UpdateRecord(const string &strTableName, const RECORD_DATA &mpColumns, const string &strCondition);

	////////////////////////////////////////////////////////////////
	//函数介绍：构造Update-SQL语句. 
	//输入参数：
	//sTableName 表名
	//mpColumns 列名/值对
	//sCondition where子语句
	//返回值：string Update-SQL语句
	/////////////////////////////////////////////////////////////////
	string BuildUpdateSQL(const string &strTableName, const RECORD_DATA &mpColumns, const string &strCondition);

	////////////////////////////////////////////////////////////////
	//函数介绍：查询记录
	//输入参数：sSql sql语句
	//返回值：
	/////////////////////////////////////////////////////////////////
	DB_DATA QueryRecord(const string& strSql);

	////////////////////////////////////////////////////////////////

private:
	DbMgr() {};

	~DbMgr()
	{
		SAFE_DELETE(m_Instance);
	};

private:
	static DbMgr *m_Instance;
};


#endif