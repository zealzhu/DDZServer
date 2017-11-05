////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhu
// Created： 2017/10/24
// Describe：服务器入口
////////////////////////////////////////////////////////////////////////
#include "basic/basic.h"
#include "basic/ConfigMgr.h"
#include "protocol/Protobuf.h"
#include "socket/ListeningManager.h"
#include "socket/SocketLib.h"
#include <string>
#include "message/MsgMgr.h"
#include "thread/ThreadLib.h"
#include "module/ModuleMgr.h"
#include "db/ConnectionPool.h"

using namespace SocketLib;
using namespace zhu;
using namespace std;
using namespace basic;
using namespace ThreadLib;

bool done = false;

// Socket处理函数
template<typename protocol>
void SocketThreadHandler(void* pData) 
{
	ConnectionManager<protocol>* pConnectionManager = (ConnectionManager<protocol>*)(pData);
	while (!done)
	{
		try
		{
			// 1、检测连接的socket是否接收到新数据，有的话接收数据并放入消息队列
			// 2、将缓存的数据通过socket发送出去
			// 3、关闭清理无效的socket
			pConnectionManager->Manage();

			ThreadLib::YieldThread();
		}
		catch (SocketLib::Exception& e)
		{
			// catch socket errors
			logger_error("socket error {}", e.PrintError());
		}
		catch (std::exception& e)
		{
			// catch standard errors
			logger_error("standard error {}", e.what());
		}
		catch (...)
		{
			logger_error("connectionManager->Manage() error");
		}
	}
}

// 监听新连接函数
template<typename protocol>
void NewConnectionThreadHandler(void* pData) {
	ListeningManager<protocol>* pListener = (ListeningManager<protocol>*)(pData);
	while (!done)
	{
		try
		{
			pListener->Listen();

			ThreadLib::YieldThread();
		}
		catch (SocketLib::Exception& e)
		{
			// catch socket errors
			logger_error("socket error {}", e.PrintError());
		}
		catch (std::exception& e)
		{
			// catch standard errors
			logger_error("standard error {}", e.what());
		}
		catch (...)
		{
			logger_error("handle new connection error");
		}

	}
}

// 消息接收队列处理函数
template<typename protocol>
void RecvMsgQueueThreadHandler(void* pData) {
	while (!done)
	{
		try
		{
			// 判断有没有待处理的消息数据
			if (CMsgMgr::getInstance().receivedMsgEmpty())
			{
				ThreadLib::YieldThread();
				continue;
			}
			MSG_PTR pMsg = CMsgMgr::getInstance().getReceivedMsg();
			if (NULL == pMsg.get())
			{
				logger_error("ReceivedMsg is illegal");
				return;
			}
			logger_debug("begin handle received msg, type {}", pMsg->GetTypeName());
			// 游戏业务逻辑处理
			CModuleMgr<CProtobuf>::Instance().DispatchMsg(pMsg);
		}
		//catch (sql::SQLException&e)
		//{
		//	logger_error("{}", e.what());
		//}
		catch (std::exception& e)
		{
			// catch standard errors
			logger_error("standard error {}", e.what());
		}
		catch (...) {
			logger_error("handle receive msg error");
		}
	}
}

// 消息发送队列处理函数
template<typename protocol>
void SendMsgQueueThreadHandler(void* pData) {
	ConnectionManager<protocol>* pConnectionManager = (ConnectionManager<protocol>*)(pData);
	while (!done)
	{
		try
		{
			// 判断有没有待处理的消息数据
			if (CMsgMgr::getInstance().responseMsgEmpty())
			{
				ThreadLib::YieldThread();
				continue;
			}
			MSG_PTR pMsg = CMsgMgr::getInstance().getResponseMsg();
			std::shared_ptr<zhu::SelfDescribingMessage> pRespMsg = dynamic_pointer_cast<zhu::SelfDescribingMessage>(pMsg);
			if (NULL == pRespMsg.get())
			{
				logger_error("dynamic_pointer_cast msg failed, msg type {}", pMsg->GetTypeName());
				continue;
			}
			for (int i = 0; i < pRespMsg->socket_size(); i++)
			{
				pConnectionManager->SendMsg(pRespMsg->socket(i), pRespMsg.get());
			}
		}
		catch (SocketLib::Exception& e)
		{
			// catch socket errors
			logger_error("socket error {}", e.PrintError());
		}
		catch (std::exception& e)
		{
			// catch standard errors
			logger_error("standard error {}", e.what());
		}
		catch (...)
		{
			logger_error("Send Msg Thread error");
		}
	}
}

template<typename protocol>
void CreateGameServiceThread(ThreadLib::ThreadID* phandle, ListeningManager<typename protocol>& listener,
	ConnectionManager<typename protocol>& connectionManager)
{
	// 创建Socket处理线程
	*phandle = ThreadLib::Create(SocketThreadHandler<protocol>, (void*)&connectionManager);
	phandle++;

	// 创建监听新连接线程
	*phandle = ThreadLib::Create(NewConnectionThreadHandler<protocol>, (void*)&listener);
	phandle++;

	// 创建消息接收队列处理线程
	*phandle = ThreadLib::Create(RecvMsgQueueThreadHandler<protocol>, nullptr);
	phandle++;

	// 创建消息发送队列处理线程
	*phandle = ThreadLib::Create(SendMsgQueueThreadHandler<protocol>, (void*)&connectionManager);
	phandle++;
}

// 配置控制台输出
void ConfigConsole()
{
#ifdef WIN32
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if (hStdin == INVALID_HANDLE_VALUE)
	{
		logger_warn("GetStdHandle failed");
	}
	DWORD fdwSaveOldMode;
	if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
	{
		logger_warn("GetConsoleMode failed");
	}
	if (!SetConsoleMode(hStdin, fdwSaveOldMode & ~ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT))
	{
		logger_warn("SetConsoleMode failed");
	}
#endif
}

int main() 
{
	// 保存线程ID数组
	// 使用四个线程，一个连接监听线程，一个连接管理线程，一个发消息线程，一个收消息线程
	ThreadLib::ThreadID threadHandles[4];

	// 初始化日志文件
	CGameLog::initLogger("log/game.log", 10, 10);

	try {
		ConfigConsole();

		logger_info("服务端初始化");

		//初始化mysql连接
		CConnectionPool::Instance();

		// 创建监听管理器
		ListeningManager<CProtobuf> listener;
		string strPort = CConfigMgr::getConfigParam("port", "9876");
		listener.AddPort(atoi(strPort.c_str()));

		// 创建连接管理器
		string strMaxDatarate = CConfigMgr::getConfigParam("maxdatarate", "1024");
		string strSenTimeout = CConfigMgr::getConfigParam("sentimeout", "60");
		string strMaxBuffered = CConfigMgr::getConfigParam("maxbuffered", "65536");
		ConnectionManager<CProtobuf> connectionManager(
			atoi(strMaxDatarate.c_str()), atoi(strSenTimeout.c_str()), atoi(strMaxBuffered.c_str()));
		listener.SetConnectionManager(&connectionManager);

		// 创建消息管理器实例
		CMsgMgr::getInstance();

		// 初始化模块管理器
		CModuleMgr<CProtobuf>::Instance().SetConnectionManager(&connectionManager);
		CModuleMgr<CProtobuf>::Instance().InitModules();

		// 创建线程
		CreateGameServiceThread(threadHandles, listener, connectionManager);

		logger_info("服务器启动成功");

		// 接收控制台输入
		const int BUF_SIZE = 128;
		char inputBuf[BUF_SIZE];
		while (!done)
		{
			cin.getline(inputBuf, BUF_SIZE);
			if (0 == strlen(inputBuf))
			{
				continue;
			}

			if (_strcmpi(inputBuf, "quit") == 0 || _strcmpi(inputBuf, "exit") == 0)
			{
				done = true;
				break;
			}
		}
	}
	catch (zhu::Exception e) {

	}
	

	return 0;
}
