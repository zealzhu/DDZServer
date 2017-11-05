#include "UserDao.h"
#include "../../basic/GameLog.h"
#include "../../basic/basic.h"
#include "../../db/ConnectionPool.h"
#include "msg/player.pb.h"

//mysql driver
#include <mysql_connection.h>
//mysql execute
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <exception>

bool zhu::CUserDao::IsExist(int iId)
{
	sql::SQLString strSql("select account from t_player where id=?");

	//获得一个连接
	std::shared_ptr<Connection> pCon = CConnectionPool::Instance().GetConnect();
	try
	{
		std::shared_ptr<PreparedStatement> pStmt(pCon->prepareStatement(strSql));
		pStmt->setInt(1, iId);
		std::shared_ptr<ResultSet> pResult(pStmt->executeQuery());
		CConnectionPool::Instance().ReturnConnect(pCon);
		if (pResult->next()) {
			logger_debug("query result: account {} id: {}", pResult->getString("account").asStdString(), iId);
			return true;
		}
	}
	catch (sql::SQLException& e)
	{
		CConnectionPool::Instance().HandleException(pCon, e);
	}
	
	return false;
}

bool zhu::CUserDao::IsExist(const char * strAccount)
{
	sql::SQLString strSql("select id from t_player where account=?");

	//获得一个连接
	std::shared_ptr<Connection> pCon = CConnectionPool::Instance().GetConnect();
	try
	{
		std::shared_ptr<PreparedStatement> pStmt(pCon->prepareStatement(strSql));
		pStmt->setString(1, strAccount);
		std::shared_ptr<ResultSet> pResult(pStmt->executeQuery());
		CConnectionPool::Instance().ReturnConnect(pCon);
		if (pResult->next()) {
			logger_debug("query result: account {} id {}", strAccount, pResult->getInt("id"));
			return true;
		}
	}
	catch (sql::SQLException& e)
	{
		CConnectionPool::Instance().HandleException(pCon, e);
	}
	
	return false;
}

zhu::PLAYER_PTR zhu::CUserDao::GetPlayer(const char * strAccount, const char * strPassword)
{
	sql::SQLString strSql("select * from t_player where account=? and password=?");

	PLAYER_PTR pPlayer(new zhu::user::Player());
	//获得一个连接
	std::shared_ptr<Connection> pCon = CConnectionPool::Instance().GetConnect();
	try
	{
		std::shared_ptr<PreparedStatement> pStmt(pCon->prepareStatement(strSql));
		int i = 1;
		pStmt->setString(i++, SQLString(strAccount));
		pStmt->setString(i++, SQLString(strPassword));
		std::shared_ptr<ResultSet> pResult(pStmt->executeQuery());
		CConnectionPool::Instance().ReturnConnect(pCon);
		if (pResult->next()) {
			pPlayer->set_id(pResult->getInt("id"));
			pPlayer->set_account(pResult->getString("account").c_str());
			pPlayer->set_password(pResult->getString("password").c_str());
			pPlayer->set_name(pResult->getString("name").c_str());
			zhu::user::Player_SexType sex =
				pResult->getString("sex").c_str() == "MALE" ? zhu::user::Player_SexType_MALE : zhu::user::Player_SexType_FEMALE;
			pPlayer->set_sex(sex);
			pPlayer->set_email(pResult->getString("email").c_str());
			sql::SQLString strMobile = pResult->getString("mobile").c_str();
			if (strMobile->length() > 0)
			{
				::zhu::user::Player_PhoneNumber* phones = pPlayer->add_phones();
				phones->set_type(::zhu::user::Player_PhoneType_MOBILE);
				phones->set_number(strMobile.c_str());
			}
			sql::SQLString strPhone(pResult->getString("phone").c_str());
			if (strPhone->length() > 0)
			{
				::zhu::user::Player_PhoneNumber* phones = pPlayer->add_phones();
				phones->set_type(::zhu::user::Player_PhoneType_HOME);
				phones->set_number(strPhone.c_str());
			}
			pPlayer->set_money(pResult->getInt("money"));
			zhu::user::UserStatus status = StatusFromString(pResult->getString("status").c_str());
			pPlayer->set_status(status);
			pPlayer->set_desc(pResult->getString("desc").c_str());
		}
	}
	catch (sql::SQLException& e)
	{
		CConnectionPool::Instance().HandleException(pCon, e);
	}
	
	return pPlayer;
}

bool zhu::CUserDao::AddNewAccount(PLAYER_PTR pRegisterReq)
{
	sql::SQLString strSql("insert into t_player(account, password, name, email) values(?,?,?,?)");

	//获得一个连接
	std::shared_ptr<Connection> pCon = CConnectionPool::Instance().GetConnect();
	try
	{
		std::shared_ptr<PreparedStatement> pStmt(pCon->prepareStatement(strSql));
		int i = 1;
		pStmt->setString(i++, SQLString(pRegisterReq->account().c_str()));
		pStmt->setString(i++, SQLString(pRegisterReq->password().c_str()));
		pStmt->setString(i++, SQLString(pRegisterReq->name().c_str()));
		pStmt->setString(i++, SQLString(pRegisterReq->email().c_str()));

		int iResult = pStmt->executeUpdate();
		CConnectionPool::Instance().ReturnConnect(pCon);
		if (iResult > 0) {
			return true;
		}
	}
	catch (sql::SQLException& e)
	{
		CConnectionPool::Instance().HandleException(pCon, e);
	}
	
	return false;
}

bool zhu::CUserDao::ChangeAccountStatus(const char * strAccount, zhu::user::UserStatus status)
{
	sql::SQLString strSql("update t_player set status=? where account=?");

	//获得一个连接
	std::shared_ptr<Connection> pCon = CConnectionPool::Instance().GetConnect();
	try
	{
		std::shared_ptr<PreparedStatement> pStmt(pCon->prepareStatement(strSql));
		int i = 1;
		std::string strStatus = StatusToString(status);
		pStmt->setString(i++, SQLString(strStatus.c_str()));
		pStmt->setString(i++, strAccount);

		int iResult = pStmt->executeUpdate();
		CConnectionPool::Instance().ReturnConnect(pCon);
		if (iResult > 0) {
			return true;
		}
	}
	catch (sql::SQLException& e)
	{
		CConnectionPool::Instance().HandleException(pCon, e);
	}
	
	return false;
}

std::string zhu::CUserDao::StatusToString(zhu::user::UserStatus status)
{
	const static std::map< zhu::user::UserStatus, std::string > mapStatus =
	{ { zhu::user::UserStatus::OFFLINE, "OFFLINE"},
	  { zhu::user::UserStatus::ONLINE, "ONLINE"},
	  { zhu::user::UserStatus::FORBIDDEN, "FORBIDDEN"} };
	std::map< zhu::user::UserStatus, std::string >::const_iterator it = mapStatus.find(status);
	if (mapStatus.end() != it)
	{
		return it->second;
	}
	logger_error("player status {} to string failed", status);
	return "STATUS NOT FOUND";
}

zhu::user::UserStatus zhu::CUserDao::StatusFromString(std::string strStatus)
{
	if (strStatus == "OFFLINE")
	{
		return zhu::user::UserStatus::OFFLINE;
	} 
	else if (strStatus == "ONLINE")
	{
		return zhu::user::UserStatus::ONLINE;
	}
	else if (strStatus == "FORBIDDEN")
	{
		return zhu::user::UserStatus::FORBIDDEN;
	}
	else
	{
		logger_error("unsupport user status {}", strStatus);
		throw zhu::Exception("unsupport user status " + strStatus);
	}
}

std::string zhu::CUserDao::SexToString(zhu::user::Player_SexType sex)
{
	const static std::map< zhu::user::Player_SexType, std::string > mapSex =
	{ { zhu::user::Player_SexType_MALE, "MALE" },
	{ zhu::user::Player_SexType_FEMALE, "FEMALE" } };
	std::map< zhu::user::Player_SexType, std::string >::const_iterator it = mapSex.find(sex);
	if (mapSex.end() != it)
	{
		return it->second;
	}
	logger_error("sex {} to string failed", sex);
	return "SEX NOT FOUND";
}

