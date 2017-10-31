#pragma once

#include <string>
#include "msg/player.pb.h"

namespace zhu
{
	typedef std::shared_ptr<zhu::user::Player> PLAYER_PTR;
	typedef std::shared_ptr<zhu::user::LoginReq> LOGIN_PTR;
	typedef std::shared_ptr<zhu::user::LogoutReq> LOGOUT_PTR;
	class CUserDao
	{
	public:
		static CUserDao& Instance()
		{
			static CUserDao instance;
			return instance;
		}

		bool IsExist(int iId) { return true; }

		bool IsExist(const ::std::string& strAccount) { return true; }

		PLAYER_PTR GetPlayer(const ::std::string& strAccount, const ::std::string& strPassword);

		zhu::user::UserStatus StatusFromString(std::string strStatus);

		bool AddNewAccount(PLAYER_PTR pRegisterReq) { return true; }

		bool ChangeAccountStatus(const std::string& strAccount, zhu::user::UserStatus status);

	private:
		CUserDao() {};
		~CUserDao() {};
		CUserDao(const CUserDao&) {};
		CUserDao& operator=(const CUserDao&) {};
		std::string StatusToString(zhu::user::UserStatus status);
		std::string SexToString(zhu::user::Player_SexType sex);
	public:
	};
}