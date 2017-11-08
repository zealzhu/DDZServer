////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhu peng cheng
// Created： 2017/08/27
// Describe：桌子类
////////////////////////////////////////////////////////////////////////
#ifndef _TABLE_H
#define _TABLE_H

#include "msg/table.pb.h"
#include "../room/msg/room.pb.h"
#include <queue>

using namespace std;
using namespace zhu::table;

namespace zhu {
	enum Player {
		PLAYER_1,
		PLAYER_2,
		PLAYER_3,
		NO_PLAYER,
	};

	struct RequestLandlordInfo {
		Player player;				//请求玩家
		RequestLandlordType type;	//请求类型
		bool bCall;					//是否叫地主
		bool bRush;					//是否抢地主
	};

	typedef shared_ptr<Poker> POKER_PTR;
	
	typedef vector<int> INT_VECTOR;
	typedef vector<int>::iterator INT_VECTOR_ITER;

	typedef shared_ptr<INT_VECTOR> INT_VECTOR_PTR;

	class CTable
	{	
		// 扑克牌id <=> 扑克
		typedef map<int, POKER_PTR> POKER_MAP;
		typedef map<int, POKER_PTR>::iterator POKER_MAP_ITER;

		// 玩家 <=> 拥有扑克id
		typedef map<Player, INT_VECTOR_PTR> PLAYER_POCKER_MAP;
		typedef map<Player, INT_VECTOR_PTR>::iterator PLAYER_POCKER_MAP_ITER;

		// 玩家 <=> 对应座位
		typedef map<Player, room::Seat*> PLAYER_SEAT_MAP;
		typedef map<Player, room::Seat*>::iterator PLAYER_SEAT_MAP_ITER;

		typedef queue<RequestLandlordInfo> REQUEST_LANDLORD_QUEUE;
	public:
		CTable();

		void AddPlayer(Player player, room::Seat* pSeat);

		// 初始化牌
		void Init();

		// 洗牌
		void Shuffle();

		// 分牌
		void Deal();

		// 分发地主牌
		void DispatchLandlordPocker(Player player);

		// 出牌
		void Play(INT_VECTOR & vecPocketsId);

		// 获取玩家拥有牌数
		int GetPlayerPockerNumber(Player player);

		// 通过扑克id获取扑克
		POKER_PTR GetPockerById(int iPockerId);

		// 获取指定玩家的牌
		INT_VECTOR_PTR GetPlayerPockerId(Player player);

		// 获取地主牌
		void GetLandlordPocker(INT_VECTOR_PTR pVecLandlordPockers);

		// 通过账号获取玩家
		Player GetPlayerByAccount(string strAccount);

		// 通过玩家获取账号
		string GetAccountByPlayer(Player player);

		// 通过玩家获取座位
		room::Seat* GetSeatByPlayer(Player player);

		// 获取当前请求的账号
		string GetCurrentRequestPlayerAccount();

		Player GetCurrentRequestPlayer();

		// 获取地主账号
		string GetLandlordPlayerAccount();

		// 请求地主
		int RequestLandlord(Player player, bool bCall);

		// 处理请求地主
		int HandlerRequestLandlord();

		// 获取当前地主请求信息
		RequestLandlordInfo & GetCurrentRequestLandlordInfo();

		// 检查地主牌花色是否在地主牌中
		bool CheckLandlordSuitInLandlordPoker(POKER_SUIT suit);

		// 排序
		void Sort(Player player, int iLeft, int iRight);

		// 快排
		void Sort(INT_VECTOR * pVecPockersId, int iLeft, int iRight);

		// 与最后一次出牌比较类型
		bool ComparePlayType(table::PLAY_TYPE type);

		// 与最后一次出牌比较大小
		bool ComparePlayPokers(table::PLAY_TYPE type, const google::protobuf::RepeatedPtrField< ::zhu::table::Poker > & pokers);

		// 移除手牌
		void RemovePokers(Player player, const google::protobuf::RepeatedPtrField< ::zhu::table::Poker > & pokers);				

		// 切换为下一个用户请求
		void ChangeToNextPlayerRequest();

		// 判断能否不出,当前请求玩家与第一次请求玩家不同才能不出
		bool CanNoPlay() {
			return m_currentRequsetPlayer != m_lastPlayPlayer;
		}

		// 清空工作
		void Clear();

		// 移除所有座位
		void ClearSeat();

	private:
		// 从扑克vector中分发扑克给玩家
		void DispatchToPlayer(int index, Player player);

	private:
		POKER_MAP m_mapPokers;							// 扑克map

		INT_VECTOR m_vecPockersOrder;					// 扑克牌堆

		PLAYER_POCKER_MAP m_mapPlayerPockers;			// 玩家拥有扑克

		PLAYER_SEAT_MAP m_mapPlayerSeats;				// 玩家对应座位
														   
		Player m_landlord;								// 地主

		Player m_currentRequsetPlayer;					// 当前请求用户

		REQUEST_LANDLORD_QUEUE m_queRequestLandlord;    // 请求地主队列

		INT_VECTOR m_vecLastPlayPokersValue;			// 最后一次出的牌值

		table::PLAY_TYPE m_lastPlayType;				// 最后一次出牌类型

		Player m_lastPlayPlayer;						// 最后一次出牌玩家
	};
}

#endif

