//#include "UserDao.h"
//#include "../../basic//GameLog.h"
//#include "../../basic/basic.h"
//#include "../../db/ConnectionPool.h"
//#include "msg/player.pb.h"
//
////mysql driver
//#include <mysql_connection.h>
////mysql execute
//#include <cppconn/statement.h>
//#include <cppconn/prepared_statement.h>
//#include <cppconn/resultset.h>
//#include <exception>
//
//bool zhu::CUserDao::IsExist(int iId)
//{
//	sql::SQLString strSql("select account from t_player where id=?");
//
//	//获得一个连接
//	std::shared_ptr<Connection> pCon = CConnectionPool::Instance().GetConnect();
//	try
//	{
//		std::shared_ptr<PreparedStatement> pStmt(pCon->prepareStatement(strSql));
//		pStmt->setInt(1, iId);
//		std::shared_ptr<ResultSet> pResult(pStmt->executeQuery());
//		CConnectionPool::Instance().ReturnConnect(pCon);
//		if (pResult->next()) {
//			logger_debug("query result: account {} id: {}", pResult->getString("account").asStdString(), iId);
//			return true;
//		}
//	}
//	catch (sql::SQLException& e)
//	{
//		CConnectionPool::Instance().HandleException(pCon, e);
//	}
//	
//	return false;
//}
//
//bool nd::CUserDao::IsExist(const ::std::string& strAccount)
//{
//	sql::SQLString strSql("select id from t_player where account=?");
//
//	//获得一个连接
//	std::shared_ptr<Connection> pCon = CConnectionPool::Instance().GetConnect();
//	try
//	{
//		std::shared_ptr<PreparedStatement> pStmt(pCon->prepareStatement(strSql));
//		pStmt->setString(1, SQLString(strAccount));
//		std::shared_ptr<ResultSet> pResult(pStmt->executeQuery());
//		CConnectionPool::Instance().ReturnConnect(pCon);
//		if (pResult->next()) {
//			logger_debug("query result: account {} id {}", strAccount, pResult->getInt("id"));
//			return true;
//		}
//	}
//	catch (sql::SQLException& e)
//	{
//		CConnectionPool::Instance().HandleException(pCon, e);
//	}
//	
//	return false;
//}
//
//nd::PLAYER_PTR nd::CUserDao::GetPlayer(const ::std::string& strAccount, const ::std::string& strPassword)
//{
//	sql::SQLString strSql("select * from t_player where account=? and password=?");
//
//	PLAYER_PTR pPlayer(NEW_ND nd::user::Player());
//	//获得一个连接
//	std::shared_ptr<Connection> pCon = CConnectionPool::Instance().GetConnect();
//	try
//	{
//		std::shared_ptr<PreparedStatement> pStmt(pCon->prepareStatement(strSql));
//		int i = 1;
//		pStmt->setString(i++, SQLString(strAccount));
//		pStmt->setString(i++, SQLString(strPassword));
//		std::shared_ptr<ResultSet> pResult(pStmt->executeQuery());
//		CConnectionPool::Instance().ReturnConnect(pCon);
//		if (pResult->next()) {
//			pPlayer->set_id(pResult->getInt("id"));
//			pPlayer->set_account(pResult->getString("account"));
//			pPlayer->set_password(pResult->getString("password"));
//			pPlayer->set_name(pResult->getString("name"));
//			nd::user::Player_SexType sex =
//				pResult->getString("sex") == "MALE" ? nd::user::Player_SexType_MALE : nd::user::Player_SexType_FEMALE;
//			pPlayer->set_sex(sex);
//			pPlayer->set_email(pResult->getString("email"));
//			sql::SQLString strMobile = pResult->getString("mobile");
//			if (strMobile->length() > 0)
//			{
//				::nd::user::Player_PhoneNumber* phones = pPlayer->add_phones();
//				phones->set_type(::nd::user::Player_PhoneType_MOBILE);
//				phones->set_number(strMobile);
//			}
//			sql::SQLString strPhone = pResult->getString("phone");
//			if (strPhone->length() > 0)
//			{
//				::nd::user::Player_PhoneNumber* phones = pPlayer->add_phones();
//				phones->set_type(::nd::user::Player_PhoneType_HOME);
//				phones->set_number(strPhone);
//			}
//			pPlayer->set_money(pResult->getInt("money"));
//			nd::user::UserStatus status = StatusFromString(pResult->getString("status"));
//			pPlayer->set_status(status);
//			pPlayer->set_desc(pResult->getString("desc"));
//		}
//	}
//	catch (sql::SQLException& e)
//	{
//		CConnectionPool::Instance().HandleException(pCon, e);
//	}
//	
//	return pPlayer;
//}
//
//bool nd::CUserDao::AddNewAccount(PLAYER_PTR pRegisterReq)
//{
//	sql::SQLString strSql("insert into t_player(account, password, name, email) values(?,?,?,?)");
//
//	//获得一个连接
//	std::shared_ptr<Connection> pCon = CConnectionPool::Instance().GetConnect();
//	try
//	{
//		std::shared_ptr<PreparedStatement> pStmt(pCon->prepareStatement(strSql));
//		int i = 1;
//		pStmt->setString(i++, SQLString(pRegisterReq->account()));
//		pStmt->setString(i++, SQLString(pRegisterReq->password()));
//		pStmt->setString(i++, SQLString(pRegisterReq->name()));
//		pStmt->setString(i++, SQLString(pRegisterReq->email()));
//
//		int iResult = pStmt->executeUpdate();
//		CConnectionPool::Instance().ReturnConnect(pCon);
//		if (iResult > 0) {
//			return true;
//		}
//	}
//	catch (sql::SQLException& e)
//	{
//		CConnectionPool::Instance().HandleException(pCon, e);
//	}
//	
//	return false;
//}
//
//bool nd::CUserDao::ChangeAccountStatus(const std::string& strAccount, nd::user::UserStatus status)
//{
//	sql::SQLString strSql("update t_player set status=? where account=?");
//
//	//获得一个连接
//	std::shared_ptr<Connection> pCon = CConnectionPool::Instance().GetConnect();
//	try
//	{
//		std::shared_ptr<PreparedStatement> pStmt(pCon->prepareStatement(strSql));
//		int i = 1;
//		std::string strStatus = StatusToString(status);
//		pStmt->setString(i++, SQLString(strStatus));
//		pStmt->setString(i++, strAccount);
//
//		int iResult = pStmt->executeUpdate();
//		CConnectionPool::Instance().ReturnConnect(pCon);
//		if (iResult > 0) {
//			return true;
//		}
//	}
//	catch (sql::SQLException& e)
//	{
//		CConnectionPool::Instance().HandleException(pCon, e);
//	}
//	
//	return false;
//}
//
//std::string nd::CUserDao::StatusToString(nd::user::UserStatus status)
//{
//	const static std::map< nd::user::UserStatus, std::string > mapStatus =
//	{ {nd::user::UserStatus::OFFLINE, "OFFLINE"},
//	  {nd::user::UserStatus::ONLINE, "ONLINE"},
//	  {nd::user::UserStatus::FORBIDDEN, "FORBIDDEN"} };
//	std::map< nd::user::UserStatus, std::string >::const_iterator it = mapStatus.find(status);
//	if (mapStatus.end() != it)
//	{
//		return it->second;
//	}
//	logger_error("player status {} to string failed", status);
//	return "STATUS NOT FOUND";
//}
//
//nd::user::UserStatus nd::CUserDao::StatusFromString(std::string strStatus)
//{
//	if (strStatus == "OFFLINE")
//	{
//		return nd::user::UserStatus::OFFLINE;
//	} 
//	else if (strStatus == "ONLINE")
//	{
//		return nd::user::UserStatus::ONLINE;
//	}
//	else if (strStatus == "FORBIDDEN")
//	{
//		return nd::user::UserStatus::FORBIDDEN;
//	}
//	else
//	{
//		logger_error("unsupport user status {}", strStatus);
//		throw nd::Exception("unsupport user status " + strStatus);
//	}
//}
//
//std::string nd::CUserDao::SexToString(nd::user::Player_SexType sex)
//{
//	const static std::map< nd::user::Player_SexType, std::string > mapSex =
//	{ { nd::user::Player_SexType_MALE, "MALE" },
//	{ nd::user::Player_SexType_FEMALE, "FEMALE" } };
//	std::map< nd::user::Player_SexType, std::string >::const_iterator it = mapSex.find(sex);
//	if (mapSex.end() != it)
//	{
//		return it->second;
//	}
//	logger_error("sex {} to string failed", sex);
//	return "SEX NOT FOUND";
//}
//
