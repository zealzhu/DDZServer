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

		bool IsExist(int iId);

		bool IsExist(const char * strAccount);

		PLAYER_PTR GetPlayer(const char * strAccount, const char * strPassword);

		zhu::user::UserStatus StatusFromString(std::string strStatus);

		bool AddNewAccount(PLAYER_PTR pRegisterReq);

		bool ChangeAccountStatus(const char * strAccount, zhu::user::UserStatus status);

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