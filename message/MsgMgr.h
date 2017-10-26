////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhu
// Created： 2017/07/10
// Describe：消息管理器 - 生产者消费者模式
////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _MSG_MGR_H
#define _MSG_MGR_H

#include <google/protobuf/message.h>
#include <deque>
#include "../thread/ThreadLibMutex.h"

using namespace ThreadLib;

namespace zhu 
{
	typedef std::shared_ptr<google::protobuf::Message> MSG_PTR;
	typedef std::deque<MSG_PTR> MSG_DEQUE;
	typedef std::deque<MSG_PTR>::iterator MSG_ITER;

	// 消息队列最大容量
	const unsigned int MAX_DEQUE_MESSAGE_SIZE = 90000;

	class CMsgMgr
	{
	public:
		// 获取实例
		static CMsgMgr & getInstance() {
			static CMsgMgr instance;
			return instance;
		}

		// 判断发送消息队列是否为空
		bool responseMsgEmpty();

		// 判断接收消息队列是否为空
		bool receivedMsgEmpty();

		// 插入消息到发送队列中
		void insertResponseMsg(MSG_PTR pMsg);

		// 从发送队列中获取一条消息
		MSG_PTR getResponseMsg();

		// 插入一条消息到接收队列中
		void insertReceivedMsg(MSG_PTR pMsg);

		// 从接收队列中获取一条消息
		MSG_PTR getReceivedMsg();

	private:
		CMsgMgr() {
			init();
		}
		
		// 初始化消息队列
		void init();

	private:
		int					 m_iMaxQueueLen;		  // 最大消息数

		MSG_DEQUE			 m_receiveDeq;			  // 接收消息队列
		MSG_DEQUE			 m_sendDeq;				  // 发送消息队列

		ThreadLib::Mutex	 m_csReceive;			  // 接收临界区
		ThreadLib::Mutex	 m_csSend;				  // 发送临界区

#ifdef WIN32
		HANDLE               m_hRecDeqFullSemaphore;  // 消息接受队列满信号量
		HANDLE               m_hRecDeqEmptySemphore;  // 消息接受队列空信号量
		HANDLE				 m_hSendDeqFullSemphore;  // 消息发送队列满信号量
		HANDLE				 m_hSendDeqEmptySemphore; // 消息发送队列空信号量
#endif // WIN32


	};
}


#endif


