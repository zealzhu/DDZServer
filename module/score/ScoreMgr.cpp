#include "ScoreMgr.h"
#include "../../message/MsgMgr.h"

void zhu::CScoreMgr::ConnectionClosed(SocketLib::DataSocket socket)
{
}

void zhu::CScoreMgr::HandleMsg(std::shared_ptr<nd::SelfDescribingMessage> pMsg)
{
	// 解析消息正文
	CProtobuf::ErrorCode pErrorCode;
	MessagePtr pInnerMsg = CProtobuf::parseInnerMsg(pMsg, pErrorCode);

	// 解析出错
	if (nd::CProtobuf::ErrorCode::UNKNOWN_MESSAGE_TYPE == pErrorCode || nd::CProtobuf::ErrorCode::PARSE_ERROR == pErrorCode)
	{
		logger_error("CProtobuf parseInnerMsg failed");
		// 创建一条消息
		std::shared_ptr<nd::SelfDescribingMessage> pPayload(NEW_ND nd::SelfDescribingMessage());
		// 设置客户端地址
		pPayload->add_socket(pMsg->socket(0));
		// 创建错误信息
		std::shared_ptr<nd::ErrorMessage> pErrorMessage(NEW_ND nd::ErrorMessage());
		pErrorMessage->set_desc("erro from server parse");
		pPayload->set_message_data(pErrorMessage->SerializeAsString());
		pPayload->set_type_name(pErrorMessage->GetTypeName());
		CMsgMgr::Instance().InsertResponseMsg(pPayload);
		return;
	}
}

void zhu::CScoreMgr::PlaySucces(int iRoomId, TABLE_PTR pTable, table::PLAY_TYPE type)
{
	bool bIsAddMultiple = false;
	// 炸弹两倍
	if (type == table::PLAY_TYPE::BOMB) {
		m_mapMultiple[iRoomId] *= 2;
		bIsAddMultiple = true;
	}	
	// 天王炸四倍
	else if (type == table::PLAY_TYPE::DOUBLE_JOCKER) {
		m_mapMultiple[iRoomId] *= 4;
		bIsAddMultiple = true;
	}
		
	if (bIsAddMultiple) {
		// 发送当前倍数
		GetCurrentMultiple(pTable->GetSeatByPlayer(Player::PLAYER_1)->socket(), iRoomId);
		GetCurrentMultiple(pTable->GetSeatByPlayer(Player::PLAYER_2)->socket(), iRoomId);
		GetCurrentMultiple(pTable->GetSeatByPlayer(Player::PLAYER_3)->socket(), iRoomId);
	}
}

void zhu::CScoreMgr::PlayStart(int iRoomId, TABLE_PTR pTable)
{
	// 初始化当前倍数为1
	m_mapMultiple[iRoomId] = 1;
}

void zhu::CScoreMgr::PlayOver(int iRoomId, TABLE_PTR pTable, bool bLandlordOver)
{
	// 结算得分
	auto pSeat1 = pTable->GetSeatByPlayer(Player::PLAYER_1);
	auto pSeat2 = pTable->GetSeatByPlayer(Player::PLAYER_2);
	auto pSeat3 = pTable->GetSeatByPlayer(Player::PLAYER_3);

	auto strLandlordAccount = pTable->GetLandlordPlayerAccount();

	JudgeIsLandlord(iRoomId, strLandlordAccount, pSeat1, bLandlordOver);
	JudgeIsLandlord(iRoomId, strLandlordAccount, pSeat2, bLandlordOver);
	JudgeIsLandlord(iRoomId, strLandlordAccount, pSeat3, bLandlordOver);
}

void zhu::CScoreMgr::LeaveTable(int iRoomId, TABLE_PTR pTable)
{
	// 清空该房间倍数
	m_mapMultiple.erase(iRoomId);
}

void zhu::CScoreMgr::RequestLandlord(int iRoomId, TABLE_PTR pTable, bool bRequest)
{
	bool bIsAddMultiple = false;
	// 有请求地主翻倍
	if (bRequest) {
		m_mapMultiple[iRoomId] *= 2;
		bIsAddMultiple = true;
	}

	if (bIsAddMultiple) {
		// 发送当前倍数
		GetCurrentMultiple(pTable->GetSeatByPlayer(Player::PLAYER_1)->socket(), iRoomId);
		GetCurrentMultiple(pTable->GetSeatByPlayer(Player::PLAYER_2)->socket(), iRoomId);
		GetCurrentMultiple(pTable->GetSeatByPlayer(Player::PLAYER_3)->socket(), iRoomId);
	}
}

void zhu::CScoreMgr::SettleScores(int iSocket, string strAccount, int iMultiple)
{
	// 创建响应消息
	std::shared_ptr<nd::SelfDescribingMessage> pPayload(NEW_ND nd::SelfDescribingMessage());
	std::shared_ptr<SettleScoreResp> pResp(NEW_ND SettleScoreResp());
	pPayload->set_type_name(pResp->GetTypeName());
	pPayload->add_socket(iSocket);

	// 用户不存在
	if (!CScoreDao::Instance().IsExist(strAccount)) {
		pResp->set_settleresult(score::ERROR_CODE::USER_NOT_EXIT);
		pResp->set_desc("user not exit");
		logger_info("{} user not exit", strAccount);	
		return;
	}

	SCORE_PTR pScore(NEW_ND Score);
	pScore->set_uid(CScoreDao::Instance().GetPlayerId(strAccount));
	pScore->set_score(CScoreDao::Instance().CalculationScore(iMultiple));

	bool bSuccess = true;

	if (CScoreDao::Instance().IsScoreExist(pScore->uid()))
		bSuccess = CScoreDao::Instance().UpdateScore(pScore);
	else
		bSuccess = CScoreDao::Instance().InsertScore(pScore);

	//插入得分
	if (bSuccess) {
		if(CScoreDao::Instance().SaveLog(pScore, iMultiple))
			logger_info("save {} log success", strAccount);
		else
			logger_error("save {} log failed", strAccount);

		pResp->set_settleresult(score::ERROR_CODE::SUCCESS);
		pResp->set_totalscore(CScoreDao::Instance().GetScore(pScore->uid()));
		pResp->set_currentscore(pScore->score());
		pResp->set_multiple(abs(iMultiple));
		logger_info("{}'s score: {}", strAccount, pScore->score());
		pPayload->set_message_data(pResp->SerializeAsString());
		CMsgMgr::Instance().InsertResponseMsg(pPayload);
	}
	else {
		pResp->set_settleresult(score::ERROR_CODE::SAVE_ERROR);
		pResp->set_desc("save score error");
		logger_info("{} save error", strAccount);
		pPayload->set_message_data(pResp->SerializeAsString());
		CMsgMgr::Instance().InsertResponseMsg(pPayload);
	}
}

void zhu::CScoreMgr::GetCurrentMultiple(int iSocket, int iRoomId)
{
	// 创建响应消息
	std::shared_ptr<nd::SelfDescribingMessage> pPayload(NEW_ND nd::SelfDescribingMessage());
	std::shared_ptr<CurrentMultiple> pResp(NEW_ND CurrentMultiple());
	pPayload->set_type_name(pResp->GetTypeName());
	pPayload->add_socket(iSocket);

	pResp->set_multiple(m_mapMultiple[iRoomId]);
	pPayload->set_message_data(pResp->SerializeAsString());
	CMsgMgr::Instance().InsertResponseMsg(pPayload);
}

void zhu::CScoreMgr::JudgeIsLandlord(int iRoomId, string strLandlordAccount, zhu::room::Seat * pSeat, bool bLandlordOver)
{
	if (strLandlordAccount == pSeat->playeraccount()) {
		// 地主是倍数是两倍
		SettleScores(pSeat->socket(), pSeat->playeraccount(),
			bLandlordOver ? (m_mapMultiple[iRoomId] * 2) : (m_mapMultiple[iRoomId] * (-2)));
	}
	else {
		// 农名
		SettleScores(pSeat->socket(), pSeat->playeraccount(),
			bLandlordOver ? (m_mapMultiple[iRoomId] * -1) : m_mapMultiple[iRoomId]);
	}
}
