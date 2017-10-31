#include "ScoreDao.h"
#include "../../basic//GameLog.h"
#include "../../basic/basic.h"
#include "../../db/ConnectionPool.h"
#include "msg/score.pb.h"

//mysql driver
#include <mysql_connection.h>
//mysql execute
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <exception>

float zhu::CScoreDao::CalculationScore(int iMultiple)
{
	return iMultiple * 400.0f;
}

bool zhu::CScoreDao::IsExist(const::std::string & strAccount)
{
	sql::SQLString strSql("select id from t_player where account=?");

	//获得一个连接
	std::shared_ptr<Connection> pCon = CConnectionPool::Instance().GetConnect();
	try
	{
		std::shared_ptr<PreparedStatement> pStmt(pCon->prepareStatement(strSql));
		pStmt->setString(1, SQLString(strAccount));
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

bool zhu::CScoreDao::IsScoreExist(int iUid)
{
	sql::SQLString strSql("select id from t_score where uid=?");

	//获得一个连接
	std::shared_ptr<Connection> pCon = CConnectionPool::Instance().GetConnect();
	try
	{
		std::shared_ptr<PreparedStatement> pStmt(pCon->prepareStatement(strSql));
		pStmt->setInt(1, iUid);
		std::shared_ptr<ResultSet> pResult(pStmt->executeQuery());
		CConnectionPool::Instance().ReturnConnect(pCon);
		if (pResult->next()) {
			return true;
		}
	}
	catch (sql::SQLException& e)
	{
		CConnectionPool::Instance().HandleException(pCon, e);
	}

	return false;
}

int zhu::CScoreDao::GetPlayerId(std::string strAccount)
{
	sql::SQLString strSql("select id from t_player where account=?");
	int id = 0;
	//获得一个连接
	std::shared_ptr<Connection> pCon = CConnectionPool::Instance().GetConnect();
	try
	{
		std::shared_ptr<PreparedStatement> pStmt(pCon->prepareStatement(strSql));
		int i = 1;
		pStmt->setString(i++, SQLString(strAccount));

		std::shared_ptr<ResultSet> pResult(pStmt->executeQuery());
		CConnectionPool::Instance().ReturnConnect(pCon);
		if (pResult->next()) {
			id = pResult->getInt("id");
		}
	}
	catch (sql::SQLException& e)
	{
		CConnectionPool::Instance().HandleException(pCon, e);
	}

	return id;
}

bool zhu::CScoreDao::UpdateScore(SCORE_PTR pScore)
{
	sql::SQLString strSql("update t_score set score=score+? WHERE uid=?");

	//获得一个连接
	std::shared_ptr<Connection> pCon = CConnectionPool::Instance().GetConnect();
	try
	{
		std::shared_ptr<PreparedStatement> pStmt(pCon->prepareStatement(strSql));
		pStmt->setDouble(1, pScore->score());
		pStmt->setInt(2, pScore->uid());
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

bool zhu::CScoreDao::InsertScore(SCORE_PTR pScore)
{
	sql::SQLString strSql("insert into t_score(uid, score) values(?,?)");

	//获得一个连接
	std::shared_ptr<Connection> pCon = CConnectionPool::Instance().GetConnect();
	try
	{
		std::shared_ptr<PreparedStatement> pStmt(pCon->prepareStatement(strSql));
		int i = 1;
		pStmt->setInt(i++,pScore->uid());
		pStmt->setDouble(i++, pScore->score());

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

bool zhu::CScoreDao::SaveLog(SCORE_PTR pScore, int iMultiple)
{
	sql::SQLString strSql("insert into t_play_log(uid,score,multiple,time) values(?,?,ABS(?),CURRENT_TIME())");

	//获得一个连接
	std::shared_ptr<Connection> pCon = CConnectionPool::Instance().GetConnect();
	try
	{
		std::shared_ptr<PreparedStatement> pStmt(pCon->prepareStatement(strSql));
		int i = 1;
		pStmt->setInt(i++, pScore->uid());
		pStmt->setDouble(i++, pScore->score());
		pStmt->setInt(i++, iMultiple);

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

float zhu::CScoreDao::GetScore(int iUid)
{
	sql::SQLString strSql("select score from t_score where uid=?");
	int id = 0;
	//获得一个连接
	std::shared_ptr<Connection> pCon = CConnectionPool::Instance().GetConnect();
	try
	{
		std::shared_ptr<PreparedStatement> pStmt(pCon->prepareStatement(strSql));
		int i = 1;
		pStmt->setInt(i++, iUid);

		std::shared_ptr<ResultSet> pResult(pStmt->executeQuery());
		CConnectionPool::Instance().ReturnConnect(pCon);
		if (pResult->next()) {
			id = pResult->getInt("score");
		}
	}
	catch (sql::SQLException& e)
	{
		CConnectionPool::Instance().HandleException(pCon, e);
	}

	return id;
}
