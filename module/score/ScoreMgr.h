////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhu
// Created： 2017/09/05
// Describe：分数管理类
////////////////////////////////////////////////////////////////////////

#ifndef _SCORE_MGR_H
#define _SCORE_MGR_H

// ========================================================================
//  Include Files
// ========================================================================
#include "../IModule.h"
#include "../../message/ndmsg.pb.h"
#include "../../protocol/Protobuf.h"
#include "../../socket/ConnectionManager.h"
#include "../../socket/SocketLibTypes.h"
#include "ScoreDao.h"
#include "msg/score.pb.h"
#include "../table/TableMgr.h"

using namespace std;
using namespace nd;

namespace zhu
{
	class CScoreMgr : public IModule, 
		public SocketLib::IConnectionEvent, public IPlayPokerEvent
	{
		// roomid <=> multiple
		typedef map<int, int> MULTIPLE_MAP;
		typedef map<int, int>::iterator MULTIPLE_MAP_ITER;
	public:
		CScoreMgr()
		{

		}

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

		virtual void ConnectionClosed(SocketLib::DataSocket socket) override;

		virtual void HandleMsg(std::shared_ptr<nd::SelfDescribingMessage> pMsg) override;

		virtual string GetModuleName() override
		{
			return "zhu.score";
		}

		// 出牌成功
		virtual void PlaySucces(int iRoomId, TABLE_PTR pTable, table::PLAY_TYPE type) override;

		// 开始出牌
		virtual void PlayStart(int iRoomId, TABLE_PTR pTable) override;

		// 出光牌
		virtual void PlayOver(int iRoomId, TABLE_PTR pTable, bool bLandlordOver) override;

		// 离开牌桌
		virtual void LeaveTable(int iRoomId, TABLE_PTR pTable) override;

		// 请求地主
		virtual void RequestLandlord(int iRoomId, TABLE_PTR pTable, bool bRequest)override;

	public:
		// 计算得分
		void SettleScores(int iSocket, string strAccount, int iMultiple);

		void GetCurrentMultiple(int iSocket, int iRoomId);

		void JudgeIsLandlord(int roomId, string strLandlordAccount, zhu::room::Seat * pSeat, bool bLandlordOver);

	private:
		std::list<TABLE_PTR> m_tableList;
		MULTIPLE_MAP m_mapMultiple;
	};

} // end namespace BasicLib


#endif
