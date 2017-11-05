////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhou du
// Created： 2017/07/10
// Describe：
////////////////////////////////////////////////////////////////////////

#include "basic/ConfigMgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "basic/Basic.h"
#include "ConnectionPool.h"
#include "thread/ThreadLibFunctions.h"
#include "basiclib/BasicLibTime.h"

using namespace std;
using namespace sql;
using namespace basic;

CConnectionPool::CConnectionPool(SQLString name, SQLString pwd, SQLString url, SQLString dbName, int maxSize) :
	m_strUserName(name), m_strPassword(pwd), m_strUrl(url), m_strDbName(dbName), m_iMaxPoolSize(maxSize),
	threadFinished(false)
{
	//得到mysql驱动
	try {
		m_pDriver = get_driver_instance();
	}
	catch (sql::SQLException&e)
	{
		logger_error("驱动连接出错, {}", e.what());
	}
	catch (std::runtime_error&e)
	{
		logger_error("运行出错, {}", e.what());
	}

	//开始初始化大小一半
	InitConnectPool(maxSize / 2);
	m_iCurPoolSize = maxSize / 2;
	m_idleConCheckThread = std::thread(&CConnectionPool::ShrinkConnectNum, this);
}
CConnectionPool::CConnectionPool()
{
}

CConnectionPool::~CConnectionPool()
{
	threadFinished = true;
	if (m_idleConCheckThread.joinable())
	{
		m_idleConCheckThread.join();
	}
	DestoryPool();
}

CConnectionPool::CConnectionPool(const CConnectionPool& connectionPool) {

}

CConnectionPool& CConnectionPool::Instance() {
	static string strUrl = "tcp://" + CConfigMgr::getConfigParam("db.ip", "127.0.0.1") + ":" + CConfigMgr::getConfigParam("db.port", "3306")
		+ "/" + CConfigMgr::getConfigParam("db.name", "game");
	static string strDbName = CConfigMgr::getConfigParam("db.name", "game");
	static string strAccount = CConfigMgr::getConfigParam("db.account", "root");
	static string strPassword = CConfigMgr::getConfigParam("db.password", "no080740");
	static string strMaxSize = CConfigMgr::getConfigParam("db.maxpoolsize", "10");
	static CConnectionPool instance =
		CConnectionPool(SQLString(strAccount.c_str()), SQLString(strPassword.c_str()), SQLString(strUrl.c_str()), SQLString(strDbName.c_str()), atoi(strMaxSize.c_str()));
	return instance;

}

CConnectionPool& CConnectionPool::operator=(const CConnectionPool& connectionPool) {

	return *this;
}

//得到连接池大小
int CConnectionPool::GetAvailableConSize()
{
	return m_ConList.size();
}

void CConnectionPool::HandleException(std::shared_ptr<sql::Connection> pCon, sql::SQLException& e)
{
	logger_error("sql error {} {}", e.getErrorCode(), e.what());
	// 如果是以下错误（2006 mysql server has gone away或者2013 Lost connection to MySQL server during query），	
	if (0 == e.getErrorCode() || 2006 == e.getErrorCode() || 2013 == e.getErrorCode())
	{
		m_Lock.Lock();
		// 重新创建一个连接，超出作用域原来的连接自动关闭，一增一减，连接池数量保持不变
		logger_info("重新获取一个连接，原来的连接超过作用域自动关闭");
		AddConn(1);
		m_Lock.Unlock();
	}
	else //普通错误（如主键冲突等），归还数据库连接
	{
		CConnectionPool::Instance().ReturnConnect(pCon);
	}
}

void CConnectionPool::ShrinkConnectNum()
{
	int iLastConNum = GetAvailableConSize();
	BasicLib::sint64 lastCheckTime = 0;
	BasicLib::sint64 currentTime = 0;
	int iCheckInterval = atoi(CConfigMgr::getConfigParam("db.idle.check.interval", "10").c_str());
	while (!threadFinished)
	{
		currentTime = BasicLib::GetTimeS();
		if (currentTime - lastCheckTime > iCheckInterval)
		{
			lastCheckTime = currentTime;
			
			//每隔10秒检查一次
			m_Lock.Lock();
			int availableConSize = GetAvailableConSize();
			logger_trace("前一次可用连接数：{}，当前可用连接数：{}", iLastConNum, availableConSize);
			if (availableConSize > m_iMaxPoolSize / 2 && iLastConNum == availableConSize)
			{
				DestoryOneConn();
				--m_iCurPoolSize;
			}
			iLastConNum = GetAvailableConSize();
			m_Lock.Unlock();
		}
		ThreadLib::YieldThread(1000);
	}
}

//增加连接
void CConnectionPool::AddConn(int size)
{
	int i = 0;
	int iSuccessCount = 0;
	logger_info("准备新增数据库连接数量： {}", size);
	for (; i < size; ++i)
	{
		//创建连接
		sql::Connection *conn;
		try {
			conn = m_pDriver->connect(m_strUrl, m_strUserName, m_strPassword);
			//bool myTrue = true;
			//conn->setClientOption("OPT_RECONNECT", &myTrue);
			std::shared_ptr<sql::Connection> sp(conn,
				[](sql::Connection *conn) {
				SAFE_DELETE(conn)
			});
			m_ConList.emplace_back(std::move(sp));
			++iSuccessCount;
		}
		catch (sql::SQLException&e)
		{
			logger_error("驱动连接出错, {}", e.what());
		}
		catch (std::runtime_error&e)
		{
			logger_error("运行出错, {}", e.what());
		}
	}

	logger_info("创建数据库连接成功。数据库url: {}，用户名: {}, 新增连接数量： {}",  m_strUrl.asStdString(),
		m_strUserName.asStdString(), iSuccessCount);
	logger_info("可用连接数： {}", GetAvailableConSize());
}
//初始化连接池
void CConnectionPool::InitConnectPool(int initialSize)
{
	//加锁，增添一个连接
	m_Lock.Lock();
	AddConn(initialSize);	
	m_Lock.Unlock();
}
//销毁一个连接
void CConnectionPool::DestoryOneConn()
{
	m_ConList.pop_front();
	logger_info("关闭一个数据库连接, 可用连接数{}，当前连接池大小：{}", GetAvailableConSize(), m_iCurPoolSize);
}
//销毁整个连接池
void CConnectionPool::DestoryPool()
{
	logger_info("关闭所有数据库连接");
	for (auto &conn : m_ConList)
	{
		//依次转移所有权，出作用域时，关闭连接，出作用域时智能指针自动释放
		std::shared_ptr<sql::Connection> &&sp = std::move(m_ConList.front());
	}
}
//扩大连接池
void CConnectionPool::ExpandPool(int size)
{
	m_Lock.Lock();
	AddConn(size);
	m_iMaxPoolSize += size;
	m_iCurPoolSize += size;
	logger_info("增加{}个数据库连接, 可用连接数{}，当前连接池大小：{}", size, GetAvailableConSize(), m_iCurPoolSize);
	m_Lock.Unlock();
}
//缩小连接池
void CConnectionPool::ReducePool(int size)
{
	m_Lock.Lock();
	//减小的大小不能超过存储的大小
	if (size > m_iMaxPoolSize)
	{
		return;
	}
	for (int i = 0; i < size; i++)
	{
		//sp point new object, old object release
		DestoryOneConn();
	}
	m_iMaxPoolSize -= size;
	m_iCurPoolSize -= size;
	logger_trace("减少{}个数据库连接, 可用连接数{}，当前连接池大小：{}", size, GetAvailableConSize(), m_iCurPoolSize);
	m_Lock.Unlock();
}

//得到一个连接
std::shared_ptr<sql::Connection> CConnectionPool::GetConnect()
{
	m_Lock.Lock();
	// 如果数据库连接用完
	if (GetAvailableConSize() == 0)
	{
		// 新建连接
		if (m_iCurPoolSize < m_iMaxPoolSize)
		{
			logger_info("新建1个数据库连接");
			AddConn(1);
			++m_iCurPoolSize;
		}
		// 等待可用连接
		else
		{
			int iRetryCount = 0;
			while (GetAvailableConSize() == 0 && iRetryCount < 10)
			{
				++iRetryCount;
				logger_info("数据库连接达到最大数量，等待可用数据库连接");
				m_Lock.Unlock();
				ThreadLib::YieldThread(50 * iRetryCount);
				m_Lock.Lock();
			}
			if (iRetryCount >= 10)
			{
				logger_info("等待可用数据库连接超过10次，放弃");
				throw zhu::Exception("等待可用数据库连接超过10次，放弃");
			}
		}
	}
	std::shared_ptr<sql::Connection> sp = m_ConList.front();
	m_ConList.pop_front();
	logger_trace("获取一个数据库连接, 可用连接数{}，当前连接池大小：{}", GetAvailableConSize(), m_iCurPoolSize);
	m_Lock.Unlock();
	return sp;
}
//归还一个连接
void CConnectionPool::ReturnConnect(std::shared_ptr<sql::Connection> &ret)
{
	m_Lock.Lock();
	m_ConList.emplace_back(ret);
	logger_trace("归还一个数据库连接, 可用连接数{}，当前连接池大小：{}", GetAvailableConSize(), m_iCurPoolSize);
	m_Lock.Unlock();
}
