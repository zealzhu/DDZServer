////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhou du
// Created： 2017/07/10
// Describe：模块管理类
////////////////////////////////////////////////////////////////////////

#ifndef CMODULEMGR_H
#define CMODULEMGR_H

// ========================================================================
//  Include Files
// ========================================================================
#include "IModule.h"
#include "user/UserMgr.h"
#include "room/RoomMgr.h"
#include "table/TableMgr.h"
//#include "score/ScoreMgr.h"
#include "../socket/ConnectionManager.h"
#include "../message/BaseMsg.pb.h"
#include <set>

using namespace std;
using namespace SocketLib;

namespace zhu
{
	typedef std::set<std::shared_ptr<IModule>> MODULE_SET;
	typedef std::set<std::shared_ptr<IModule>>::iterator MODULE_ITER;
	typedef std::shared_ptr<google::protobuf::Message> MSG_PTR;

	template<typename protocol>
    class CModuleMgr
    {
    public:
		static CModuleMgr& Instance()
		{
			static CModuleMgr instance;
			return instance;
		}
		
		void SetConnectionManager(ConnectionManager<protocol>* p_manager)
		{
			m_manager = p_manager;
		}

		bool AddMoudle(std::shared_ptr<IModule> pModule)
		{
			MODULE_SET::_Pairib pair = modules.insert(pModule);
			return pair.second;
		}

		void RemoveMoudle(MODULE_ITER it)
		{
			modules.erase(it);
		}

		void RemoveAllMoudle()
		{
			while (!modules.empty())
			{
				MODULE_ITER it = modules.begin();
				RemoveMoudle(it);
			}
		}

		MODULE_SET GetAllModules()
		{
			return modules;
		}

		void InitModules()
		{
			//创建用户模块管理器
			std::shared_ptr<CUserMgr> pUserMgr(NEW_ND CUserMgr());
			pUserMgr->Init();						//初始化
			m_manager->AddListener(pUserMgr);		//添加连接监听
			AddMoudle(pUserMgr);					//添加用户模块

			//创建房间模块管理器
			std::shared_ptr<CRoomMgr> pRoomMgr(NEW_ND CRoomMgr());
			pRoomMgr->Init();						//初始化
			m_manager->AddListener(pRoomMgr);		//添加连接监听
			AddMoudle(pRoomMgr);					//添加房间模块

			//创建牌桌模块管理器
			std::shared_ptr<CTableMgr> pTableMgr(NEW_ND CTableMgr());
			pTableMgr->Init();
			pRoomMgr->AddListener(pTableMgr);		//添加房间模块的监听
			m_manager->AddListener(pTableMgr);		//添加连接监听
			AddMoudle(pTableMgr);					//添加房间模块

			////创建分数模块
			//std::shared_ptr<CScoreMgr> pScoreMgr(NEW_ND CScoreMgr());
			//pScoreMgr->Init();						//初始化
			//pTableMgr->AddListener(pScoreMgr);		//添加桌子模块监听
			//m_manager->AddListener(pScoreMgr);		//添加连接监听
			//AddMoudle(pScoreMgr);					//添加房间模块
		}

		string GetModuleName(string strTypeName)
		{
			int index = strTypeName.find_last_of(".");
			string moduleName = strTypeName;
			if (index != std::string::npos)
			{
				moduleName = strTypeName.substr(0, index);
			}

			return moduleName;
		}


		void DispatchMsg(MSG_PTR pMsg)
		{
			std::shared_ptr<zhu::SelfDescribingMessage> pTmpMsg = dynamic_pointer_cast<zhu::SelfDescribingMessage>(pMsg);
			if (NULL == pTmpMsg.get())
			{
				logger_warn("dynamic_cast msg failed");
				return;
			}

			string strModuleType = GetModuleName(pTmpMsg->type_name());
			MODULE_ITER itModule = modules.begin();
			while (itModule != modules.end())
			{
				std::shared_ptr<IModule> pModule = *itModule;
				if (strModuleType == pModule->GetModuleName())
				{
					pModule->HandleMsg(pTmpMsg);
				}
				itModule++;
			}
		}


	private:
		CModuleMgr() {}

		~CModuleMgr() {}

		CModuleMgr(const CModuleMgr&) {}

		CModuleMgr& operator=(const CModuleMgr&) {}

		MODULE_SET modules;

		ConnectionManager<protocol>* m_manager;
	};

} // end namespace BasicLib


#endif
