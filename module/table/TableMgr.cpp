#include "TableMgr.h"
#include "../../message/MsgMgr.h"

void zhu::CTableMgr::HandleMsg(std::shared_ptr<zhu::SelfDescribingMessage> pMsg)
{
	// 解析消息正文
	CProtobuf::ErrorCode pErrorCode;
	MessagePtr pInnerMsg = CProtobuf::parseInnerMsg(pMsg, pErrorCode);

	// 解析出错
	if (zhu::CProtobuf::ErrorCode::UNKNOWN_MESSAGE_TYPE == pErrorCode || zhu::CProtobuf::ErrorCode::PARSE_ERROR == pErrorCode)
	{
		logger_error("CProtobuf parseInnerMsg failed");
		// 创建一条消息
		std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
		// 设置客户端地址
		pPayload->add_socket(pMsg->socket(0));
		// 创建错误信息
		std::shared_ptr<zhu::ErrorMessage> pErrorMessage(NEW_ND zhu::ErrorMessage());
		pErrorMessage->set_desc("erro from server parse");
		pPayload->set_message_data(pErrorMessage->SerializeAsString());
		pPayload->set_type_name(pErrorMessage->GetTypeName());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}

	// 出牌
	if ("zhu.table.PlayReq" == pMsg->type_name()) {
		logger_debug("begin handle play msg");
		if (NULL != pInnerMsg)
		{
			PLAY_REQ pPlayReq = dynamic_pointer_cast<zhu::table::PlayReq>(pInnerMsg);
			Play(pMsg->socket(0), pPlayReq);
		}
	}
	// 请求地主
	else if ("zhu.table.RequestLandlordReq" == pMsg->type_name()) {
		logger_debug("begin handle request landlord msg");
		if (NULL != pInnerMsg)
		{
			REQUEST_LANDLORD_REQ pRequestLandlordReq = dynamic_pointer_cast<zhu::table::RequestLandlordReq>(pInnerMsg);
			RequestLandlord(pMsg->socket(0), pRequestLandlordReq);
		}
	}
}

void zhu::CTableMgr::ConnectionClosed(SocketLib::DataSocket sock)
{

}

void zhu::CTableMgr::RoomNumberChange(const int iRoomNumber)
{
	// 房间数量改变
}

void zhu::CTableMgr::RoomDestory(ROOM_PTR pRoom)
{
	// 房间销毁移除对应桌子
	m_mapTables.erase(pRoom);
}

void zhu::CTableMgr::RoomUserNumberChange(ROOM_PTR pRoom, const int iUserNumber)
{
	// 房间人数改变
	if (iUserNumber == 3) {
		// 设置房间状态为人满
		pRoom->set_status(room::RoomStatus::FULL);
	}
	else {
		// 如果房间是开始状态则游戏结束并通知其他玩家
		if (pRoom->status() == room::RoomStatus::START) {
			// 创建响应消息
			std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
			std::shared_ptr<PlayerOut> pErrorMessage(NEW_ND PlayerOut());
			pPayload->set_type_name(pErrorMessage->GetTypeName());
			pErrorMessage->set_errorresult(ERROR_CODE::PLAYER_OUT_GAME);
			pErrorMessage->set_desc("player out game");

			// 获取座位玩家
			room::Seat* pSeat1 = pRoom->mutable_seat1();
			room::Seat* pSeat2 = pRoom->mutable_seat2();
			room::Seat* pSeat3 = pRoom->mutable_seat3();

			// 判断玩家1状态
			if (pSeat1->statu() != room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER) {
				pPayload->add_socket(pSeat1->socket());
				pSeat1->set_statu(room::Seat::SeatStatus::Seat_SeatStatus_UNREADY);
			}
			else {
				pErrorMessage->set_account(pSeat1->playeraccount());
			}

			// 判断玩家2状态
			if (pSeat2->statu() != room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER) {
				pPayload->add_socket(pSeat2->socket());
				pSeat2->set_statu(room::Seat::SeatStatus::Seat_SeatStatus_UNREADY);
			}
			else {
				pErrorMessage->set_account(pSeat2->playeraccount());
			}

			// 判断玩家3状态
			if (pSeat3->statu() != room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER) {
				pPayload->add_socket(pSeat3->socket());
				pSeat3->set_statu(room::Seat::SeatStatus::Seat_SeatStatus_UNREADY);
			}
			else {
				pErrorMessage->set_account(pSeat3->playeraccount());
			}

			logger_error("some body leave, clear table");
			pPayload->set_message_data(pErrorMessage->SerializeAsString());
			CMsgMgr::getInstance().insertResponseMsg(pPayload);

			// 清扫牌桌
			auto pTable = m_mapTables[pRoom];
			pTable->Clear();
			pTable->ClearSeat();	

			pRoom->set_status(zhu::room::RoomStatus::WAIT);

			// 通知离开牌桌
			NotifyListennersLeaveTable(pRoom->id(), pTable);
		}
		// 设置房间等待
		pRoom->set_status(room::RoomStatus::WAIT);
	}
}

void zhu::CTableMgr::RoomStart(ROOM_PTR pRoom)
{
	// 设置游戏开始状态
	pRoom->set_status(room::RoomStatus::START);

	// 获取座位玩家
	room::Seat* pSeat1 = pRoom->mutable_seat1();
	room::Seat* pSeat2 = pRoom->mutable_seat2();
	room::Seat* pSeat3 = pRoom->mutable_seat3();

	// 房间对应桌子
	TABLE_PTR pTable = m_mapTables[pRoom];

	// 添加玩家到桌子上
	pTable->AddPlayer(Player::PLAYER_1, pSeat1);
	pTable->AddPlayer(Player::PLAYER_2, pSeat2);
	pTable->AddPlayer(Player::PLAYER_3, pSeat3);

	InitTable(pTable);

	// 通知牌桌开始
	NotifyListennersPlayStart(pRoom->id(), pTable);
}

void zhu::CTableMgr::RoomOver(ROOM_PTR pRoom)
{
	// 游戏结束清扫桌子座位玩家
	auto pTable = m_mapTables[pRoom];
	pTable->ClearSeat();

	// 通知离开牌桌
	NotifyListennersLeaveTable(pRoom->id(), pTable);
}

void zhu::CTableMgr::RoomCreate(ROOM_PTR pRoom)
{
	// 创建一张桌子并初始化牌
	TABLE_PTR pTable(NEW_ND CTable);
	pTable->Init();
	m_mapTables[pRoom] = pTable;
}

void zhu::CTableMgr::NotifyListennersRequestLandlord(int iRoomId, TABLE_PTR pTable, bool bRequest)
{
	for each (auto listener in m_listenerList)
	{
		listener->RequestLandlord(iRoomId, pTable, bRequest);
	}
}

void zhu::CTableMgr::NotifyListennersPlayStart(int iRoomId, TABLE_PTR pTable)
{
	for each (auto listener in m_listenerList)
	{
		listener->PlayStart(iRoomId, pTable);
	}
}

void zhu::CTableMgr::NotifyListennersPlayOver(int iRoomId, TABLE_PTR pTable, bool bLandlordOver)
{
	for each (auto listener in m_listenerList)
	{
		listener->PlayOver(iRoomId, pTable, bLandlordOver);
	}
}

void zhu::CTableMgr::NotifyListennersPlaySuccess(int iRoomId, TABLE_PTR pTable, table::PLAY_TYPE type)
{
	for each (auto listener in m_listenerList)
	{
		listener->PlaySucces(iRoomId, pTable, type);
	}
}

void zhu::CTableMgr::NotifyListennersLeaveTable(int iRoomId, TABLE_PTR pTable)
{
	for each (auto listener in m_listenerList)
	{
		listener->LeaveTable(iRoomId, pTable);
	}
}

void zhu::CTableMgr::InitTable(TABLE_PTR pTable)
{
	// 清除操作
	pTable->Clear();

	// 洗牌
	pTable->Shuffle();

	// 分牌
	pTable->Deal();

	// 当前请求账号
	string strAccount = pTable->GetCurrentRequestPlayerAccount();

	// 发送牌给玩家1	
	Deal(pTable, strAccount, Player::PLAYER_1);

	// 发送牌给给玩家2	
	Deal(pTable, strAccount, Player::PLAYER_2);

	// 发送牌给给玩家3
	Deal(pTable, strAccount, Player::PLAYER_3);
}

void zhu::CTableMgr::Play(int iSocket, PLAY_REQ pPlayReq)
{
	// 创建响应消息
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
	std::shared_ptr<PlayResp> pPlayResp(NEW_ND PlayResp());
	pPayload->set_type_name(pPlayResp->GetTypeName());
	pPayload->add_socket(iSocket);

	TABLE_PTR pTable = GetTable(pPlayReq->roomid());
	int curPos = pTable->GetSeatByPlayer(pTable->GetPlayerByAccount(pPlayReq->account()))->position();
	Player player = pTable->GetPlayerByAccount(pPlayReq->account());
	pPlayResp->set_account(pPlayReq->account());
	pPlayResp->set_currentposition(curPos);

	// 判断是否不出
	if (pPlayReq->type() == table::PLAY_TYPE::NO_PLAYER) {
		// 与第一次请求不一样或者不能大过就能不出
		if (pTable->CanNoPlay()) {
			pTable->ChangeToNextPlayerRequest();

			pPlayResp->set_playresult(table::ERROR_CODE::NO_PLAY);
			pPlayResp->set_next(pTable->GetCurrentRequestPlayerAccount());
			pPlayResp->set_nextposition(pTable->GetSeatByPlayer(pTable->GetPlayerByAccount(pTable->GetCurrentRequestPlayerAccount()))->position());
			pPlayResp->set_desc("no play");
			logger_info("{} {}", pPlayReq->account(), pPlayResp->desc());
			pPayload->set_message_data(pPlayResp->SerializeAsString());
			// 发送消息给别人
			SendResponseToOtherPlayer(pPayload, player, pTable);

			// 发送下一位玩家的牌
			ShowCurrentPoker(pTable->GetCurrentRequestPlayer(), pTable);
			return;
		}
		// 轮到该玩家出牌不得不出
		else {
			pPlayResp->set_playresult(table::ERROR_CODE::CAN_NOT_NO_PLAY);
			pPlayResp->set_desc("must be play");
			logger_error("{} {}", pPlayReq->account(), pPlayResp->desc());
			pPayload->set_message_data(pPlayResp->SerializeAsString());
			CMsgMgr::getInstance().insertResponseMsg(pPayload);
			return;
		}
	}

	// 比较与最后一次出牌的牌型比较
	bool bCompareType = pTable->ComparePlayType(pPlayReq->type());
	if (!bCompareType) {
		pPlayResp->set_playresult(ERROR_CODE::PLAY_TYPE_ERROR);
		pPlayResp->set_desc("play type error");
		logger_error("{} {}", pPlayReq->account(), pPlayResp->desc());
		pPayload->set_message_data(pPlayResp->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}

	// 比较能否大过
	bool bCanBig = pTable->ComparePlayPokers(pPlayReq->type(), pPlayReq->pokers());
	if (!bCanBig) {
		pPlayResp->set_playresult(ERROR_CODE::COMPARE_LOSE);
		pPlayResp->set_desc("play not enough big");
		logger_error("{} {}", pPlayReq->account(), pPlayResp->desc());
		pPayload->set_message_data(pPlayResp->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}
	// 移除手牌
	else {
		pTable->RemovePokers(player, pPlayReq->pokers());
	}

	// 发送出的牌给所有玩家
	std::shared_ptr<zhu::SelfDescribingMessage> pDispatchPaload(NEW_ND zhu::SelfDescribingMessage());
	std::shared_ptr<DispatchPoker> pDispatchPokerMsg(NEW_ND DispatchPoker());
	pDispatchPaload->set_type_name(pDispatchPokerMsg->GetTypeName());
	pDispatchPokerMsg->set_landlordaccount(pPlayReq->account());// 设置出牌的账号
	pDispatchPokerMsg->set_type(DispatchPokerType::PLAYER_POKER);
	pDispatchPokerMsg->set_position(curPos);
	pDispatchPaload->add_socket(iSocket);
	// 赋值牌的信息
	for (auto it = pPlayReq->pokers().begin(); pPlayReq->pokers().end() != it; it++) {
		auto pPocker = pDispatchPokerMsg->add_pockers();
		pPocker->set_number(it->number());
		pPocker->set_suit(it->suit());
		pPocker->set_value(it->value());
	}
	pDispatchPaload->set_message_data(pDispatchPokerMsg->SerializeAsString());
	logger_info("send play pokers to other");
	SendResponseToOtherPlayer(pDispatchPaload, player, pTable);

	// 通知出牌成功
	NotifyListennersPlaySuccess(pPlayReq->roomid(), pTable, pPlayReq->type());

	// 发送成功消息
	pTable->ChangeToNextPlayerRequest();
	auto nextAccount = pTable->GetCurrentRequestPlayerAccount();
	pPlayResp->set_playresult(table::ERROR_CODE::SUCCESS);
	
	pPlayResp->set_next(nextAccount);
	pPlayResp->set_nextposition(pTable->GetSeatByPlayer(pTable->GetPlayerByAccount(nextAccount))->position());
	pPlayResp->set_desc("play success");
	pPlayResp->set_number(pTable->GetPlayerPockerNumber(pTable->GetPlayerByAccount(pPlayReq->account())));
	logger_info("{} {}", pPlayReq->account(), pPlayResp->desc());
	pPayload->set_message_data(pPlayResp->SerializeAsString());
	SendResponseToOtherPlayer(pPayload, player, pTable);

	// 更新当前玩家的手牌
	ShowCurrentPoker(pTable->GetPlayerByAccount(pPlayReq->account()), pTable);

	// 判断是否打完牌
	if (pTable->GetPlayerPockerNumber(player) == 0) {
		if (pTable->GetLandlordPlayerAccount() == pPlayReq->account()) {
			pPlayResp->set_playresult(table::ERROR_CODE::LANDLORD_WIN);
			logger_info("landlord win the game");
			NotifyListennersPlayOver(pPlayReq->roomid(), pTable, true);
		}
		else {
			pPlayResp->set_playresult(table::ERROR_CODE::PEASANT_WIN);
			logger_info("peasant win the game");
			NotifyListennersPlayOver(pPlayReq->roomid(), pTable, false);
		}
		
		// 更改房间状态
		auto pRoom = GetRoom(pPlayReq->roomid());
		if (pRoom != NULL)
			pRoom->set_status(room::RoomStatus::FULL);

		pPlayResp->set_account(pTable->GetLandlordPlayerAccount());	//设置地主账号
		pPlayResp->set_desc("game over");
		pPayload->set_message_data(pPlayResp->SerializeAsString());
		SendResponseToOtherPlayer(pPayload, player, pTable);
		return;
	}
}

void zhu::CTableMgr::Deal(TABLE_PTR pTable, string strAccount, Player player)
{
	auto pSeat = pTable->GetSeatByPlayer(player);
	auto pLandlordSeat = pTable->GetSeatByPlayer(pTable->GetPlayerByAccount(strAccount));

	// 创建响应消息
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
	std::shared_ptr<DispatchPoker> pDispatchPokerMsg(NEW_ND DispatchPoker());
	pPayload->set_type_name(pDispatchPokerMsg->GetTypeName());
	pPayload->add_socket(pSeat->socket());

	// 设置叫地主的账号
	pDispatchPokerMsg->set_landlordaccount(strAccount);
	pDispatchPokerMsg->set_type(DispatchPokerType::DEAL_POKER);
	pDispatchPokerMsg->set_position(pLandlordSeat->position());
	
	// 赋值牌的信息
	for (INT_VECTOR_ITER it = pTable->GetPlayerPockerId(player)->begin(); pTable->GetPlayerPockerId(player)->end() != it; it++) {
		auto pPocker = pDispatchPokerMsg->add_pockers();
		auto sendPocker = pTable->GetPockerById(*it).get();
		pPocker->CopyFrom(*sendPocker);
	}
	
	// 发送响应消息
	logger_info("send pocker to socket {}", pSeat->playeraccount());
	pPayload->set_message_data(pDispatchPokerMsg->SerializeAsString());
	CMsgMgr::getInstance().insertResponseMsg(pPayload);
}

void zhu::CTableMgr::ShowLandlordPoker(Player player, TABLE_PTR pTable)
{
	auto pSeat = pTable->GetSeatByPlayer(player);
	// 创建响应消息
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
	std::shared_ptr<DispatchPoker> pDispatchPokerMsg(NEW_ND DispatchPoker());
	pPayload->set_type_name(pDispatchPokerMsg->GetTypeName());
	pPayload->add_socket(pSeat->socket());

	pDispatchPokerMsg->set_type(DispatchPokerType::LANDLORD_POKER);
	
	// 获取地主牌id
	INT_VECTOR_PTR pVecLandlordPockers(NEW_ND vector<int>);
	pTable->GetLandlordPocker(pVecLandlordPockers);

	// 赋值牌的信息
	for (INT_VECTOR_ITER it = pVecLandlordPockers->begin(); pVecLandlordPockers->end() != it; it++) {
		auto pPocker = pDispatchPokerMsg->add_pockers();
		auto sendPocker = pTable->GetPockerById(*it).get();
		pPocker->CopyFrom(*sendPocker);
	}

	pPayload->set_message_data(pDispatchPokerMsg->SerializeAsString());
	logger_info("show landlord poker");
	SendResponseToOtherPlayer(pPayload, player, pTable);
}

void zhu::CTableMgr::ShowCurrentPoker(Player player, TABLE_PTR pTable)
{
	auto pSeat = pTable->GetSeatByPlayer(player);
	// 创建响应消息
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
	std::shared_ptr<DispatchPoker> pDispatchPokerMsg(NEW_ND DispatchPoker());
	pPayload->set_type_name(pDispatchPokerMsg->GetTypeName());
	pPayload->add_socket(pSeat->socket());

	pDispatchPokerMsg->set_type(DispatchPokerType::CURRENT_POKER);

	auto pPockers = pTable->GetPlayerPockerId(player);
	// 赋值牌的信息
	for (INT_VECTOR_ITER it = pPockers->begin(); pPockers->end() != it; it++) {
		auto pPocker = pDispatchPokerMsg->add_pockers();
		auto sendPocker = pTable->GetPockerById(*it).get();
		pPocker->CopyFrom(*sendPocker);
	}

	// 发送响应消息
	logger_info("show {} pokers", pSeat->playeraccount());
	pPayload->set_message_data(pDispatchPokerMsg->SerializeAsString());
	CMsgMgr::getInstance().insertResponseMsg(pPayload);
}

void zhu::CTableMgr::RequestLandlord(int iSocket, REQUEST_LANDLORD_REQ pMsg)
{
	// 创建响应消息
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
	std::shared_ptr<RequestLandlordResp> pRequestLandlordMsg(NEW_ND RequestLandlordResp());
	pPayload->set_type_name(pRequestLandlordMsg->GetTypeName());
	pPayload->add_socket(iSocket);

	// 对应桌子
	auto pTable = GetTable(pMsg->roomid());

	// 对应玩家
	Player player = pTable->GetPlayerByAccount(pMsg->account());
	auto seat = pTable->GetSeatByPlayer(player);

	ERROR_CODE code = (ERROR_CODE)pTable->RequestLandlord(player, pMsg->call());
	pRequestLandlordMsg->set_calllandlordresult(code);

	// 未轮到该玩家请求地主
	if (code == ERROR_CODE::NO_TURN_TO_REQUEST) {
		pRequestLandlordMsg->set_desc("no turn to you request");

		// 发送响应消息
		logger_info("no turn to {} request landlord", pMsg->account());
		pPayload->set_message_data(pRequestLandlordMsg->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
	}
	// 未选出地主则发送通知给所有用户继续选
	else if (code == ERROR_CODE::NOT_SELECTED_LANDLORD) {
		auto nextRequestInfo = pTable->GetCurrentRequestLandlordInfo();
		pRequestLandlordMsg->set_account(pMsg->account());
		pRequestLandlordMsg->set_currentposition(seat->position());
		pRequestLandlordMsg->set_call(pMsg->call());
		pRequestLandlordMsg->set_next(pTable->GetAccountByPlayer(nextRequestInfo.player));
		pRequestLandlordMsg->set_nextposition(pTable->GetSeatByPlayer(nextRequestInfo.player)->position());
		pRequestLandlordMsg->set_type(pMsg->type());
		pRequestLandlordMsg->set_nexttype(nextRequestInfo.type);

		pPayload->set_message_data(pRequestLandlordMsg->SerializeAsString());
		logger_info("send request response");
		SendResponseToOtherPlayer(pPayload, player, pTable);

		// 通知抢地主请求
		NotifyListennersRequestLandlord(pMsg->roomid(), pTable, pMsg->call());
	}
	// 无人叫地主
	else if (code == ERROR_CODE::NO_ONE_CALL_LANDLORD) {
		pRequestLandlordMsg->set_desc("no one call landlord");

		pPayload->set_message_data(pRequestLandlordMsg->SerializeAsString());
		logger_info("no one call landlord");
		SendResponseToOtherPlayer(pPayload, player, pTable);

		// 重新初始化牌桌
		InitTable(pTable);
	}
	// 选出地主，通知其他玩家以及通知地主出牌
	else if (code == ERROR_CODE::SELECTED_LANDLORD) {	
		auto nextRequestInfo = pTable->GetCurrentRequestLandlordInfo();
		pRequestLandlordMsg->set_desc("selected landlord");
		pRequestLandlordMsg->set_account(pMsg->account());
		pRequestLandlordMsg->set_currentposition(seat->position());
		pRequestLandlordMsg->set_next(pTable->GetAccountByPlayer(nextRequestInfo.player));
		pRequestLandlordMsg->set_nextposition(pTable->GetSeatByPlayer(nextRequestInfo.player)->position());
		pRequestLandlordMsg->set_type(nextRequestInfo.type);

		pPayload->set_message_data(pRequestLandlordMsg->SerializeAsString());
		logger_info("{} become the landlord", pTable->GetAccountByPlayer(nextRequestInfo.player));
		SendResponseToOtherPlayer(pPayload, player, pTable);

		// 显示地主牌
		ShowLandlordPoker(nextRequestInfo.player, pTable);

		// 分发地主牌
		pTable->DispatchLandlordPocker(nextRequestInfo.player);

		// 显示当前地主的牌
		pTable->Sort(nextRequestInfo.player, 0, 19);
		ShowCurrentPoker(nextRequestInfo.player, pTable);

		// 通知抢地主请求
		NotifyListennersRequestLandlord(pMsg->roomid(), pTable, pMsg->call());
	}
}

void zhu::CTableMgr::SendResponseToOtherPlayer(std::shared_ptr<zhu::SelfDescribingMessage> pMsg, 
	Player player, TABLE_PTR pTable)
{
	Player onePlayer = (Player)((player + 1) % 3);
	Player anotherOnePlayer = (Player)((player + 2) % 3);

	room::Seat* pSeat1 = pTable->GetSeatByPlayer(onePlayer);
	room::Seat* pSeat2 = pTable->GetSeatByPlayer(anotherOnePlayer);

	pMsg->add_socket(pSeat1->socket());
	pMsg->add_socket(pSeat2->socket());

	// 发送响应消息
	CMsgMgr::getInstance().insertResponseMsg(pMsg);
}

zhu::TABLE_PTR zhu::CTableMgr::GetTable(int iRoomId)
{
	for (auto room : m_mapTables) {
		if (room.first->id() == iRoomId)
			return room.second;
	}
	return NULL;
}

zhu::ROOM_PTR zhu::CTableMgr::GetRoom(int iRoomId)
{
	for (auto room : m_mapTables) {
		if (room.first->id() == iRoomId)
			return room.first;
	}
	return NULL;
}

