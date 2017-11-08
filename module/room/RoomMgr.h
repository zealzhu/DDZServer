////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhu peng cheng
// Created： 2017/08/27
// Describe：房间管理类
////////////////////////////////////////////////////////////////////////
#ifndef _ROOM_MGR_H
#define _ROOM_MGR_H

// ========================================================================
//  Include Files
// ========================================================================
#include "../IModule.h"
#include "../../message/BaseMsg.pb.h"
#include "../../protocol/Protobuf.h"
#include "../../socket/ConnectionManager.h"
#include "../../socket/SocketLibTypes.h"
#include "msg/room.pb.h"

#define MAX_ROOM_NUMBER 5

using namespace std;

namespace zhu {
	typedef std::shared_ptr<zhu::room::Room> ROOM_PTR;
	typedef std::shared_ptr<zhu::room::Seat> SEAT_PTR;	
	typedef std::shared_ptr<zhu::room::EnterRoomReq> ENTER_ROOM_PTR;
	typedef std::shared_ptr<zhu::room::GetRoomReq> GET_ROOM_PTR;
	typedef std::shared_ptr<zhu::room::LeaveRoomReq> LEAVE_ROOM_PTR;
	typedef std::shared_ptr<zhu::room::CreateRoomReq> CREATE_ROOM_PTR;
	typedef std::shared_ptr<zhu::room::ReadyReq> READY_PTR;
	typedef std::shared_ptr<zhu::room::GameOverMsg> GAME_OVER_PTR;

	//房间状态改变事件
	class IRoomStatusChangeEvent
	{
	public:
		// 房间数量改变
		virtual void RoomNumberChange(const int iRoomNumber) = 0;

		// 房间创建
		virtual void RoomCreate(ROOM_PTR pRoom) = 0;

		// 房间销毁
		virtual void RoomDestory(ROOM_PTR pRoom) = 0;

		// 房间人数改变
		virtual void RoomUserNumberChange(ROOM_PTR pRoom, const int iUserNumber) = 0;

		//房间游戏开始
		virtual void RoomStart(ROOM_PTR pRoom) = 0;

		//房间游戏等待
		virtual void RoomOver(ROOM_PTR pRoom) = 0;
	};

	class CRoomMgr : public IModule, public SocketLib::IConnectionEvent
	{
		// RID <-> ROOM
		typedef std::map<int, ROOM_PTR > ROOM_MAP;
		typedef std::map<int, ROOM_PTR >::iterator ROOM_ITER;

		//房间状态监听集合
		typedef std::list< std::shared_ptr<IRoomStatusChangeEvent> > ROOM_STATUS_LISTENNER_LIST;
		typedef typename std::list< std::shared_ptr<IRoomStatusChangeEvent> >::iterator USER_STATUS_LISTENNER_LIST_ITR;
	public:
		CRoomMgr() {}

		virtual bool Init() override
		{
			return true;
		}

		virtual bool Start() override
		{
			return true;
		}

		virtual bool Stop() override
		{
			return true;
		}

		virtual bool Reload() override
		{
			return true;
		}

		virtual void HandleMsg(std::shared_ptr<zhu::SelfDescribingMessage> pMsg) override;

		virtual string GetModuleName() override
		{
			return "zhu.room";
		}

		// ------------------------------------------------------------------------
		// Description: 用户连接关闭
		// ------------------------------------------------------------------------
		virtual void ConnectionClosed(SocketLib::DataSocket sock) override;

		// ------------------------------------------------------------------------
		// Description: 添加房间监听者
		// ------------------------------------------------------------------------
		void AddListener(std::shared_ptr<IRoomStatusChangeEvent> pListenner)
		{
			m_listenerList.push_back(pListenner);
		}

		// ------------------------------------------------------------------------
		// Description: 通知所有房间监听者房间人数改变
		// ------------------------------------------------------------------------
		void NotifyRoomListennersUserCountChange(ROOM_PTR pRoom, const int iUserCount);

		// ------------------------------------------------------------------------
		// Description: 通知所有房间监听者房间游戏状态改变
		// ------------------------------------------------------------------------
		void NotifyRoomListennersGameStatuChange(ROOM_PTR pRoom, bool bStart);

		// ------------------------------------------------------------------------
		// Description: 通知所有房间监听者房间数量改变
		// ------------------------------------------------------------------------
		void NotifyRoomListennersRoomNumberChange(const int iRoomNumber);

		// ------------------------------------------------------------------------
		// Description: 通知所有房间监听者创建了一个房间
		// ------------------------------------------------------------------------
		void NotifyRoomListennersRoomCreate(ROOM_PTR pRoom);

	private:
		// 创建房间
		void Create(int iSocket, CREATE_ROOM_PTR pCreateRoomReq);

		// 进入房间
		void Enter(int iSocket, ENTER_ROOM_PTR pEnterRoomReq);

		// 离开房间
		void Leave(int iSocket, LEAVE_ROOM_PTR pLeaveRoomReq);

		// 获取房间
		void Get(int iSocket, GET_ROOM_PTR pGetRoomReq);

		// 准备
		void Ready(int iSocket, READY_PTR pReadyReq);

		// 发送进入房间消息
		void SendOtherPlayerStatuChange(ROOM_PTR pRoom, int iSeat);

		// 发送已准备消息
		void SendReadyToOtherPlayer(int iSocket, int iSeat, string strAccount, bool bReady);

		// 游戏开始
		void Start(int iSocket, int iRoomId);

		// 游戏结束
		void GameOver(int iSocket, GAME_OVER_PTR pGameOverMsg);

	private:
		ROOM_MAP m_mapRooms;								//房间列表

		ROOM_STATUS_LISTENNER_LIST m_listenerList;			//监听本模块列表

	private:
		//生成房间id
		int GenerateRoomId();			

		//创建座位
		void CreateSeat(ROOM_PTR pRoom);				

		//入座,返回第几个座位
		int Seating(ROOM_PTR pRoom, string strAccount, int iSocket);

		//检查用户是否在房间中，返回所在的位置
		zhu::room::Seat* CheckPlayerInRoom(ROOM_PTR pRoom, string strAccount);

		//遍历所有房间检查用户是否在房间中
		bool CheckPlayerInRoom(string strAccount);

		//检查房间是否要关闭，返回是否关闭
		bool CheckRoomShouldBeClose(ROOM_PTR pRoom);
											
		//获取房间人数
		int GetRoomPlayerNumber(ROOM_PTR pRoom);

		// 检查用户是否准备
		bool CheckAllPlayerIsReady(ROOM_PTR pRoom);

		// 发送准备给其他用户
		void SendReadyToOtherPlayer(ROOM_PTR pRoom, room::Seat* pSeat, string strAccount, bool bReady);

		// 发送游戏开始给用户
		void SendStartToAllPlayer(ROOM_PTR pRoom);
	};
}

#endif // !_ROOMMGR_H



