////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhou du
// Created： 2017/07/10
// Describe：用户管理类
////////////////////////////////////////////////////////////////////////

#ifndef CUSERMGR_H
#define CUSERMGR_H

// ========================================================================
//  Include Files
// ========================================================================
#include "../IModule.h"
#include "../../message/BaseMsg.pb.h"
#include "../../protocol/Protobuf.h"
#include "../../socket/ConnectionManager.h"
#include "../../socket/SocketLibTypes.h"
#include "UserDao.h"
#include "msg/player.pb.h"
#include "../room/RoomMgr.h"

using namespace std;

namespace zhu
{
	class IUserStatusChangeEvent
	{
	public:

		virtual void UserOnline(const string& strAccount) = 0;

		virtual void UserOffline(const string& strAccount) = 0;

	};

    class CUserMgr : public IModule, public SocketLib::IConnectionEvent
    {
		// UID <-> PLAYER
		typedef std::map<int, PLAYER_PTR > PLAYER_MAP;
		typedef std::map<int, PLAYER_PTR >::iterator PLAYER_ITER;

		typedef std::list< std::shared_ptr<IUserStatusChangeEvent> > USER_STATUS_LISTENNER_LIST;
		typedef typename std::list< std::shared_ptr<IUserStatusChangeEvent> >::iterator USER_STATUS_LISTENNER_LIST_ITR;

    public:
		CUserMgr()
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


		virtual void HandleMsg(std::shared_ptr<SelfDescribingMessage> pMsg) override;

		virtual string GetModuleName() override
		{
			return "zhu.user";
		}

		virtual void ConnectionClosed(SocketLib::DataSocket sock) override;

		// ------------------------------------------------------------------------
		// Description: This add listener to m_listenners.
		// ------------------------------------------------------------------------
		void AddListener(std::shared_ptr<IUserStatusChangeEvent> pListenner)
		{
			m_listenerList.push_back(pListenner);
		}

		// ------------------------------------------------------------------------
		// Description: notify all listenners user status.
		// ------------------------------------------------------------------------
		void NotifyUserListenners(const string& strAccount, bool bOnline);

	private:
		void Login(int iSocket, LOGIN_PTR pLoginReq);

		bool ValidateLoginReq(LOGIN_PTR pLoginReq, zhu::user::LoginResp& errorMessage);
		
		void Register(int iSocket, PLAYER_PTR pRegisterReq);
		
		bool ValidateRegisterReq(PLAYER_PTR pRegisterReq, ErrorMessage& errorMessage);

		void Logout(int iSocket, LOGOUT_PTR pLogoutReq);

		PLAYER_MAP m_mapPlayers;//用户列表

		USER_STATUS_LISTENNER_LIST m_listenerList;
	};

} // end namespace BasicLib


#endif
