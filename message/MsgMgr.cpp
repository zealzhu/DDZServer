#include "MsgMgr.h"
#include "../basic/GameLog.h"
#include "../basic/ConfigMgr.h"
#include <string>

using namespace std;

void zhu::CMsgMgr::init() {
	// 从配置文件中读取队列最大消息数
	string strMsgQueueSize = basic::CConfigMgr::getConfigParam("msg.queue.size", "9999");
	m_iMaxQueueLen = atoi(strMsgQueueSize.c_str());
	if (0 == m_iMaxQueueLen)
	{
		// 设置默认值
		m_iMaxQueueLen = MAX_DEQUE_MESSAGE_SIZE;
	}

	// 创建接收队列信号量
	m_hRecDeqEmptySemphore = CreateSemaphore(NULL, m_iMaxQueueLen, m_iMaxQueueLen, NULL);
	m_hRecDeqFullSemaphore = CreateSemaphore(NULL, 0, m_iMaxQueueLen, NULL);
	// 创建发送队列信号量
	m_hSendDeqEmptySemphore = CreateSemaphore(NULL, m_iMaxQueueLen, m_iMaxQueueLen, NULL);
	m_hSendDeqFullSemphore = CreateSemaphore(NULL, 0, m_iMaxQueueLen, NULL);

	if (NULL == m_hRecDeqEmptySemphore ||
		NULL == m_hRecDeqFullSemaphore ||
		NULL == m_hSendDeqEmptySemphore ||
		NULL == m_hSendDeqFullSemphore)
	{
		logger_error("信号量创建失败，服务器无法启动!");
		exit(1);
	}
}

bool zhu::CMsgMgr::responseMsgEmpty()
{
	bool bFlag = false;

	// 对临界资源进行访问是互斥的
	m_csSend.Lock();

	if (m_sendDeq.empty())
		bFlag = true;

	m_csSend.Unlock();
	
	return bFlag;
}

bool zhu::CMsgMgr::receivedMsgEmpty()
{
	bool bFlag = false;

	// 对临界资源进行访问是互斥的
	m_csReceive.Lock();

	if (m_receiveDeq.empty())
		bFlag = true;

	m_csReceive.Unlock();

	return bFlag;
}

void zhu::CMsgMgr::insertResponseMsg(MSG_PTR pMsg)
{
	// 消息满则等待
	WaitForSingleObject(m_hSendDeqEmptySemphore, INFINITE);

	m_csSend.Lock();
	// 将消息插入队列
	m_sendDeq.emplace_back(pMsg);
	m_csSend.Unlock();

	// 满信号量+1
	ReleaseSemaphore(m_hSendDeqFullSemphore, 1, NULL);
}

zhu::MSG_PTR zhu::CMsgMgr::getResponseMsg()
{
	// 没有消息则等待
	WaitForSingleObject(m_hSendDeqFullSemphore, INFINITE);
	MSG_PTR msg = NULL;

	m_csSend.Lock();
	// 取出消息队列第一条消息
	if (!m_sendDeq.empty()) {
		MSG_ITER it = m_sendDeq.begin();
		msg = *it;
		m_sendDeq.pop_front();
	}
	m_csSend.Unlock();

	// 空信号量+1
	ReleaseSemaphore(m_hSendDeqEmptySemphore, 1, NULL);

	return msg;
}

void zhu::CMsgMgr::insertReceivedMsg(MSG_PTR pMsg)
{
	// 消息满则等待
	WaitForSingleObject(m_hRecDeqEmptySemphore, INFINITE);

	m_csReceive.Lock();
	m_receiveDeq.emplace_back(pMsg);
	m_csReceive.Unlock();

	// 满信号量+1
	ReleaseSemaphore(m_hRecDeqFullSemaphore, 1, NULL);
}

zhu::MSG_PTR zhu::CMsgMgr::getReceivedMsg()
{
	// 没有消息则等待
	WaitForSingleObject(m_hRecDeqFullSemaphore, INFINITE);
	MSG_PTR msg = NULL;

	m_csReceive.Lock();
	// 取出消息队列第一条消息
	if (!m_receiveDeq.empty()) {
		MSG_ITER it = m_receiveDeq.begin();
		msg = *it;
		m_receiveDeq.pop_front();
	}
	m_csReceive.Unlock();

	// 空信号量+1
	ReleaseSemaphore(m_hRecDeqEmptySemphore, 1, NULL);

	return msg;
}
