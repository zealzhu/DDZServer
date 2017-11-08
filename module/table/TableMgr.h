////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhu peng cheng
// Created： 2017/08/30
// Describe：桌子管理类
////////////////////////////////////////////////////////////////////////
#ifndef _TABLE_MGR_H
#define _TABLE_MGR_H

// ========================================================================
//  Include Files
// ========================================================================
#include "../IModule.h"
#include "../../message/BaseMsg.pb.h"
#include "../../protocol/Protobuf.h"
#include "../../socket/ConnectionManager.h"
#include "../../socket/SocketLibTypes.h"
#include "../room/RoomMgr.h"
#include "Table.h"

using namespace std;

namespace zhu {

	typedef shared_ptr<CTable> TABLE_PTR;
	typedef shared_ptr<RequestLandlordReq> REQUEST_LANDLORD_REQ;
	typedef shared_ptr<PlayReq> PLAY_REQ;

	// 出牌事件
	class IPlayPokerEvent {
	public:
		// 请求地主
		virtual void RequestLandlord(int iRoomId, TABLE_PTR pTable, bool bRequest) = 0;

		// 出牌成功
		virtual void PlaySucces(int iRoomId, TABLE_PTR pTable, table::PLAY_TYPE type) = 0;

		// 开始出牌
		virtual void PlayStart(int iRoomId, TABLE_PTR pTable) = 0;

		// 出光牌
		virtual void PlayOver(int iRoomId, TABLE_PTR pTable, bool bLandlordOver) = 0;

		// 离开牌桌
		virtual void LeaveTable(int iRoomId, TABLE_PTR pTable) = 0;
	};

	class CTableMgr : public IModule, 
		public SocketLib::IConnectionEvent,
		public IRoomStatusChangeEvent
	{
		// roomid <=> Table
		typedef map<ROOM_PTR, TABLE_PTR> TABLE_MAP;
		typedef map<ROOM_PTR, TABLE_PTR>::iterator TABLE_MAP_ITER;

		//出牌状态监听集合
		typedef std::list< std::shared_ptr<IPlayPokerEvent> > PLAY_POKER_LISTENNER_LIST;
		typedef typename std::list< std::shared_ptr<IPlayPokerEvent> >::iterator PLAY_POKER_LISTENNER_LIST_ITR;
	public:
		CTableMgr() {}

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
			return "zhu.table";
		}

	public:
		// ------------------------------------------------------------------------
		// Description: 用户连接关闭
		// ------------------------------------------------------------------------
		virtual void ConnectionClosed(SocketLib::DataSocket sock) override;

		// ------------------------------------------------------------------------
		// Description: 房间数量改变
		// ------------------------------------------------------------------------
		virtual void RoomNumberChange(const int iRoomNumber) override;

		// ------------------------------------------------------------------------
		// Description: 房间销毁
		// ------------------------------------------------------------------------
		virtual void RoomDestory(ROOM_PTR pRoom) override;

		// ------------------------------------------------------------------------
		// Description: 房间人数改变
		// ------------------------------------------------------------------------
		virtual void RoomUserNumberChange(ROOM_PTR pRoom, const int iUserNumber) override;

		// ------------------------------------------------------------------------
		// Description: 房间游戏开始
		// ------------------------------------------------------------------------
		virtual void RoomStart(ROOM_PTR pRoom) override;

		// ------------------------------------------------------------------------
		// Description: 房间游戏等待
		// ------------------------------------------------------------------------
		virtual void RoomOver(ROOM_PTR pRoom) override;

		// ------------------------------------------------------------------------
		// Description: 房间创建
		// ------------------------------------------------------------------------
		virtual void RoomCreate(ROOM_PTR pRoom) override;

		void AddListener(std::shared_ptr<IPlayPokerEvent> pListenner)
		{
			m_listenerList.push_back(pListenner);
		}
		
		void NotifyListennersRequestLandlord(int iRoomId, TABLE_PTR pTable, bool bRequest);

		void NotifyListennersPlayStart(int iRoomId, TABLE_PTR pTable);

		void NotifyListennersPlayOver(int iRoomId, TABLE_PTR pTable, bool bLandlordOver);

		void NotifyListennersPlaySuccess(int iRoomId, TABLE_PTR pTable, table::PLAY_TYPE type);

		void NotifyListennersLeaveTable(int iRoomId, TABLE_PTR pTable);
	private:
		// 初始化牌桌
		void InitTable(TABLE_PTR pTable);

		// 出牌
		void Play(int iSocket, PLAY_REQ);

		// 发牌
		void Deal(TABLE_PTR pTable, string strAccount, Player player);

		// 显示地主牌
		void ShowLandlordPoker(Player player, TABLE_PTR pTable);

		// 显示当前牌
		void ShowCurrentPoker(Player player, TABLE_PTR pTable);

		// 请求地主
		void RequestLandlord(int iSocket, REQUEST_LANDLORD_REQ pMsg);

		// 发送响应消息给其他用户
		void SendResponseToOtherPlayer(std::shared_ptr<zhu::SelfDescribingMessage>, 
			Player callPlayer, TABLE_PTR pTable);


		TABLE_PTR GetTable(int iRoomId);
		ROOM_PTR GetRoom(int iRoomId);

	private:
		TABLE_MAP m_mapTables;
		PLAY_POKER_LISTENNER_LIST m_listenerList;
	};
}

#endif

