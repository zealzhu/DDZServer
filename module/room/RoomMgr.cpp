#include "RoomMgr.h"
#include "../../message/MsgMgr.h"

void zhu::CRoomMgr::HandleMsg(std::shared_ptr<zhu::SelfDescribingMessage> pMsg)
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

	// 获取房间
	if ("zhu.room.GetRoomReq" == pMsg->type_name()) {
		logger_debug("begin handle get room msg");
		if (NULL != pInnerMsg)
		{
			GET_ROOM_PTR pGetRoomReq = dynamic_pointer_cast<zhu::room::GetRoomReq>(pInnerMsg);
			Get(pMsg->socket(0), pGetRoomReq);
		}
	}
	// 离开房间
	else if ("zhu.room.LeaveRoomReq" == pMsg->type_name()) {
		logger_debug("begin handle leave msg");
		if (NULL != pInnerMsg)
		{
			LEAVE_ROOM_PTR pLeaveRoomReq = dynamic_pointer_cast<zhu::room::LeaveRoomReq>(pInnerMsg);
			Leave(pMsg->socket(0), pLeaveRoomReq);
		}
	}
	// 进入房间
	else if ("zhu.room.EnterRoomReq" == pMsg->type_name()) {
		logger_debug("begin handle enter room msg");
		if (NULL != pInnerMsg)
		{
			ENTER_ROOM_PTR pEnterRoomReq = dynamic_pointer_cast<zhu::room::EnterRoomReq>(pInnerMsg);
			Enter(pMsg->socket(0), pEnterRoomReq);
		}
	}
	// 创建房间
	else if ("zhu.room.CreateRoomReq" == pMsg->type_name()) {
		logger_debug("begin handle create room msg");
		if (NULL != pInnerMsg)
		{
			CREATE_ROOM_PTR pCreateRoomReq = dynamic_pointer_cast<zhu::room::CreateRoomReq>(pInnerMsg);
			Create(pMsg->socket(0), pCreateRoomReq);
		}
	}
	// 准备
	else if ("zhu.room.ReadyReq" == pMsg->type_name()) {
		logger_debug("begin handle ready msg");
		if (NULL != pInnerMsg)
		{
			READY_PTR pReadyReq = dynamic_pointer_cast<zhu::room::ReadyReq>(pInnerMsg);
			Ready(pMsg->socket(0), pReadyReq);
		}
	}
	// 游戏结束
	else if ("zhu.room.GameOverMsg" == pMsg->type_name()) {
		logger_debug("begin handle game over msg");
		if (NULL != pInnerMsg)
		{
			GAME_OVER_PTR pGameOver = dynamic_pointer_cast<zhu::room::GameOverMsg>(pInnerMsg);
			GameOver(pMsg->socket(0), pGameOver);
		}
	}
}

void zhu::CRoomMgr::ConnectionClosed(SocketLib::DataSocket sock)
{
	// 遍历所有房间查看关闭的连接是否在房间中，如果在则退出房间
	for (ROOM_ITER it = m_mapRooms.begin(); m_mapRooms.end() != it; it++) {
		auto pRoom = it->second;
		zhu::room::Seat* seat1 = pRoom->mutable_seat1();
		zhu::room::Seat* seat2 = pRoom->mutable_seat2();
		zhu::room::Seat* seat3 = pRoom->mutable_seat3();

		if (seat1->socket() == sock.GetSock()) {
			logger_info("{} disconnect, leave room {}", seat1->playeraccount(), it->first);
			seat1->set_statu(zhu::room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER);
			pRoom->set_usercount(pRoom->usercount() - 1);
			NotifyRoomListennersUserCountChange(it->second, it->second->usercount());
			// 判断房间还有没有人,没人就关掉了，否则通知其他人玩家退出房间
			if (!CheckRoomShouldBeClose(pRoom)) {
				SendOtherPlayerStatuChange(pRoom, seat1->position());
			}
		}
		else if (seat2->socket() == sock.GetSock()) {
			logger_info("{} disconnect, leave room {}", seat2->playeraccount(), it->first);
			seat2->set_statu(zhu::room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER);
			pRoom->set_usercount(pRoom->usercount() - 1);
			NotifyRoomListennersUserCountChange(it->second, pRoom->usercount());
			// 判断房间还有没有人,没人就关掉了，否则通知其他人玩家退出房间
			if (!CheckRoomShouldBeClose(pRoom)) {
				SendOtherPlayerStatuChange(pRoom, seat2->position());
			}
		}
		else if (seat3->socket() == sock.GetSock()) {		
			logger_info("{} disconnect, leave room {}", seat2->playeraccount(), it->first);
			seat3->set_statu(zhu::room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER);
			pRoom->set_usercount(pRoom->usercount() - 1);
			NotifyRoomListennersUserCountChange(it->second, pRoom->usercount());
			// 判断房间还有没有人,没人就关掉了，否则通知其他人玩家退出房间
			if (!CheckRoomShouldBeClose(pRoom)) {
				SendOtherPlayerStatuChange(pRoom, seat3->position());
			}
		}

		// 检查是否关闭房间
		if (CheckRoomShouldBeClose(pRoom))
			break;
	}
}

void zhu::CRoomMgr::NotifyRoomListennersUserCountChange(ROOM_PTR pRoom, const int iUserCount)
{
	if (iUserCount == 0) {
		// 房间人数为0的时候发送房间销毁
		for each (auto listener in m_listenerList)
		{
			listener->RoomDestory(pRoom);
		}
	}
	else {
		// 房间人数改变
		for each (auto listener in m_listenerList)
		{
			listener->RoomUserNumberChange(pRoom, iUserCount);
		}
	}
}

void zhu::CRoomMgr::NotifyRoomListennersGameStatuChange(ROOM_PTR pRoom, bool bStart)
{
	if (bStart) {
		for each (auto listener in m_listenerList)
		{
			listener->RoomStart(pRoom);
		}
	}
	else {
		for each (auto listener in m_listenerList)
		{
			listener->RoomOver(pRoom);
		}
	}
}

void zhu::CRoomMgr::NotifyRoomListennersRoomNumberChange(const int iRoomNumber)
{
	for each (auto listener in m_listenerList)
	{
		listener->RoomNumberChange(iRoomNumber);
	}
}

void zhu::CRoomMgr::NotifyRoomListennersRoomCreate(ROOM_PTR pRoom)
{
	for each (auto listener in m_listenerList)
	{
		listener->RoomCreate(pRoom);
	}
}

void zhu::CRoomMgr::Create(int iSocket, CREATE_ROOM_PTR pCreateRoomMsg)
{
	// 创建响应消息
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
	std::shared_ptr<zhu::room::CreateRoomResp> pCreateRoomResult(NEW_ND zhu::room::CreateRoomResp());
	pPayload->set_type_name(pCreateRoomResult->GetTypeName());
	pPayload->add_socket(iSocket);

	// 房间数满
	if (m_mapRooms.size() >= MAX_ROOM_NUMBER) {
		pCreateRoomResult->set_createresult(zhu::room::ERROR_CODE::CREATE_ROOM_ERROR);
		pCreateRoomResult->set_desc("the room is full!");
		logger_error("{}", pCreateRoomResult->desc());
		pPayload->set_message_data(pCreateRoomResult->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}

	// 是否已经在房间中
	if (CheckPlayerInRoom(pCreateRoomMsg->account())) {
		pCreateRoomResult->set_createresult(zhu::room::ERROR_CODE::PLAYER_HAS_IN_ROOM);
		pCreateRoomResult->set_desc("player has in a room, please exit first!");
		logger_error("{}", pCreateRoomResult->desc());
		pPayload->set_message_data(pCreateRoomResult->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}

	// 创建一个房间
	ROOM_PTR pRoom(NEW_ND zhu::room::Room());
	pRoom->set_id(GenerateRoomId());							// 设置房间id
	pRoom->set_roomname(pCreateRoomMsg->account() + "'s room");	// 设置房间名
	pRoom->set_status(zhu::room::RoomStatus::WAIT);			    // 设置房间为等待状态
	pRoom->set_usercount(0);									// 设置玩家数
	CreateSeat(pRoom);											// 创建三个座位
	m_mapRooms[pRoom->id()] = pRoom;

	// 当前玩家进入座位
	Seating(pRoom, pCreateRoomMsg->account(), iSocket);		

	// 发送创建成功响应消息
	logger_info("create room success");
	pCreateRoomResult->set_createresult(zhu::room::ERROR_CODE::SUCCESS);
	// 使用 pCreateRoomResult->set_allocated_createroom(pRoom.get()) 后会把内存释放掉
	pCreateRoomResult->mutable_createroom()->CopyFrom(*pRoom.get());
	pPayload->set_message_data(pCreateRoomResult->SerializeAsString());
	CMsgMgr::getInstance().insertResponseMsg(pPayload);

	// 通知其他模块房间数量改变
	NotifyRoomListennersRoomNumberChange(m_mapRooms.size());
	// 通知创建了一个房间
	NotifyRoomListennersRoomCreate(pRoom);
}

void zhu::CRoomMgr::Enter(int iSocket, ENTER_ROOM_PTR pEnterRoomMsg)
{
	// 创建响应消息
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
	std::shared_ptr<zhu::room::EnterRoomResp> pEnterRoomResult(NEW_ND zhu::room::EnterRoomResp());
	pPayload->set_type_name(pEnterRoomResult->GetTypeName());
	pPayload->add_socket(iSocket);

	// 房间不存在
	if (!m_mapRooms.count(pEnterRoomMsg->roomid())) {
		pEnterRoomResult->set_enterresult(zhu::room::ERROR_CODE::ROOM_NOT_EXIST);
		pEnterRoomResult->set_desc("the room is not exist");
		logger_error("{}", pEnterRoomResult->desc());
		pPayload->set_message_data(pEnterRoomResult->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}

	ROOM_PTR pRoom = m_mapRooms[pEnterRoomMsg->roomid()];

	// 房间人数已满
	if (pRoom->usercount() >= 3) {
		pEnterRoomResult->set_enterresult(zhu::room::ERROR_CODE::ROOM_USER_FULL_ERROR);
		pEnterRoomResult->set_desc("the room's user is full");
		logger_error("{}", pEnterRoomResult->desc());
		pPayload->set_message_data(pEnterRoomResult->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}

	// 用户是否在房间中
	zhu::room::Seat* pSeat = NULL;
	if ((pSeat = CheckPlayerInRoom(pRoom, pEnterRoomMsg->account())) != NULL) {
		pEnterRoomResult->set_enterresult(zhu::room::ERROR_CODE::PLAYER_HAS_IN_ROOM);
		pEnterRoomResult->set_desc("player has in room");
		logger_error("{}", pEnterRoomResult->desc());
		pPayload->set_message_data(pEnterRoomResult->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}
	pEnterRoomResult->roominfo().id();
	// 进入房间入座
	int iPosition;
	if ((iPosition = Seating(pRoom, pEnterRoomMsg->account(), iSocket)) != 0) {
		pEnterRoomResult->set_enterresult(zhu::room::ERROR_CODE::SUCCESS);
		pEnterRoomResult->set_position(iPosition);
		auto roomInfo = pEnterRoomResult->mutable_roominfo();
		roomInfo->CopyFrom(*pRoom.get());
		logger_info("{} enter room {} success and seat in position {}", 
			pEnterRoomMsg->account(), pEnterRoomMsg->roomid(), iPosition);
		pPayload->set_message_data(pEnterRoomResult->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);

		// 通知房间人数改变
		NotifyRoomListennersUserCountChange(pRoom, pRoom->usercount());
		SendOtherPlayerStatuChange(pRoom, iPosition);

		return;
	}
	// 入座失败
	else {
		pEnterRoomResult->set_enterresult(zhu::room::ERROR_CODE::ROOM_USER_FULL_ERROR);
		pEnterRoomResult->set_desc("the room's user is full");
		logger_error("{}", pEnterRoomResult->desc());
		pPayload->set_message_data(pEnterRoomResult->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}
}

void zhu::CRoomMgr::Leave(int iSocket, LEAVE_ROOM_PTR pLeaveRoomMsg)
{
	// 创建响应消息
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
	std::shared_ptr<zhu::room::LeaveRoomResp> pLeaveRoomResult(NEW_ND zhu::room::LeaveRoomResp());
	pPayload->set_type_name(pLeaveRoomResult->GetTypeName());
	pPayload->add_socket(iSocket);

	// 房间不存在
	if (!m_mapRooms.count(pLeaveRoomMsg->roomid())) {
		pLeaveRoomResult->set_leaveroomresult(zhu::room::ERROR_CODE::ROOM_NOT_EXIST);
		logger_error("{}", "the room is not exist");
		pPayload->set_message_data(pLeaveRoomResult->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}

	ROOM_PTR pRoom = m_mapRooms[pLeaveRoomMsg->roomid()];
	zhu::room::Seat* pSeat = NULL;
	int iPosition;
	// 用户不在房间中
	if ((pSeat = CheckPlayerInRoom(pRoom, pLeaveRoomMsg->account())) == NULL) {
		pLeaveRoomResult->set_leaveroomresult(zhu::room::ERROR_CODE::PLAYER_NOT_IN_ROOM);
		logger_error("{}", "player not in room");
		pPayload->set_message_data(pLeaveRoomResult->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}
	iPosition = pSeat->position();
	
	// 离开房间
	pSeat->set_statu(zhu::room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER);
	pSeat->clear_playeraccount();
	pSeat->set_socket(-1);
	pRoom->set_usercount(pRoom->usercount() - 1);
	
	// 判断房间还有没有人,没人就关掉了，否则通知其他人玩家退出房间
	if (!CheckRoomShouldBeClose(pRoom)) {
		SendOtherPlayerStatuChange(pRoom, iPosition);
	}
	
	// 成功消息
	pLeaveRoomResult->set_leaveroomresult(zhu::room::ERROR_CODE::SUCCESS);
	logger_info("{} leave room {} success", pLeaveRoomMsg->account(), pLeaveRoomMsg->roomid());
	pPayload->set_message_data(pLeaveRoomResult->SerializeAsString());
	CMsgMgr::getInstance().insertResponseMsg(pPayload);

	// 通知房间人数改变
	NotifyRoomListennersUserCountChange(pRoom, pRoom->usercount());
}

void zhu::CRoomMgr::Get(int iSocket, GET_ROOM_PTR pGetRoomMsg)
{
	// 创建响应消息
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
	std::shared_ptr<zhu::room::GetRoomResp> pGetRoomResp(NEW_ND zhu::room::GetRoomResp());
	pPayload->set_type_name(pGetRoomResp->GetTypeName());
	pPayload->add_socket(iSocket);

	// 检测用户是否存在

	// 添加房间数
	pGetRoomResp->set_count(m_mapRooms.size());

	// 添加房间
	for (ROOM_ITER it = m_mapRooms.begin(); m_mapRooms.end() != it; it++) {
		auto room = pGetRoomResp->add_room();
		auto sendRoom = it->second.get();
		room->CopyFrom(*sendRoom);
	}

	// 发送响应消息
	logger_info("get {} room", m_mapRooms.size());
	pPayload->set_message_data(pGetRoomResp->SerializeAsString());
	CMsgMgr::getInstance().insertResponseMsg(pPayload);
}

void zhu::CRoomMgr::Ready(int iSocket, READY_PTR pReadyReq)
{
	// 创建响应消息
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
	std::shared_ptr<zhu::room::ReadyResp> pReadyResp(NEW_ND zhu::room::ReadyResp());
	pPayload->set_type_name(pReadyResp->GetTypeName());
	pPayload->add_socket(iSocket);

	// 判断用户是否在房间中
	ROOM_PTR pRoom = m_mapRooms[pReadyReq->roomid()];
	zhu::room::Seat* pSeat = NULL;

	// 检查是否在房间
	if ((pSeat = CheckPlayerInRoom(pRoom, pReadyReq->account())) == NULL) {
		pReadyResp->set_readyresult(zhu::room::ERROR_CODE::PLAYER_NOT_IN_ROOM);
		pReadyResp->set_desc("player not in room");
		logger_error("{}", pReadyResp->desc());
		pPayload->set_message_data(pReadyResp->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}

	// 判断是否已经准备
	if (pSeat->statu() == zhu::room::Seat::SeatStatus::Seat_SeatStatus_READY) {
		pReadyResp->set_readyresult(zhu::room::ERROR_CODE::PLAYER_HAS_READY);
		pReadyResp->set_desc("player has ready");
		logger_error("{}", "has ready");
		pPayload->set_message_data(pReadyResp->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}
	
	// 准备
	pSeat->set_statu(zhu::room::Seat::SeatStatus::Seat_SeatStatus_READY);
	pReadyResp->set_readyresult(zhu::room::ERROR_CODE::SUCCESS);
	pReadyResp->set_account(pReadyReq->account());
	pReadyResp->set_ready(true);
	pReadyResp->set_position(pSeat->position());
	pReadyResp->set_desc("player ready");
	logger_info("{} {}", pReadyReq->account(), " ready");
	pPayload->set_message_data(pReadyResp->SerializeAsString());
	CMsgMgr::getInstance().insertResponseMsg(pPayload);

	// 全部准备则通知游戏开始
	if (CheckAllPlayerIsReady(pRoom)) {
		SendStartToAllPlayer(pRoom);
		// 通知其他模块
		NotifyRoomListennersGameStatuChange(pRoom, true);
	}
	// 通知其他用户已准备
	else {
		SendReadyToOtherPlayer(pRoom, pSeat, pReadyReq->account(), true);
	}
}

void checkSend(const zhu::room::Seat & pSeat, int iSock, zhu::SelfDescribingMessage * msg) {
	if (pSeat.statu() != zhu::room::Seat::NO_PLAYER)
		msg->add_socket(iSock);
}

void zhu::CRoomMgr::SendOtherPlayerStatuChange(ROOM_PTR pRoom, int iSeat)
{
	// 创建响应消息
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
	std::shared_ptr<zhu::room::Seat> pSeatMsg(NEW_ND zhu::room::Seat());
	pPayload->set_type_name(pSeatMsg->GetTypeName());

	if (iSeat == 1) 
	{
		pSeatMsg->CopyFrom(pRoom->seat1());
		checkSend(pRoom->seat2(), pRoom->seat2().socket(), pPayload.get());
		checkSend(pRoom->seat3(), pRoom->seat3().socket(), pPayload.get());
	}
	else if (iSeat == 2)
	{
		pSeatMsg->CopyFrom(pRoom->seat2());
		checkSend(pRoom->seat1(), pRoom->seat1().socket(), pPayload.get());
		checkSend(pRoom->seat3(), pRoom->seat3().socket(), pPayload.get());
	}
	else if (iSeat == 3)
	{
		pSeatMsg->CopyFrom(pRoom->seat3());
		checkSend(pRoom->seat2(), pRoom->seat2().socket(), pPayload.get());
		checkSend(pRoom->seat1(), pRoom->seat1().socket(), pPayload.get());
	}
	logger_info("send playerenter room {} to other", pRoom->roomname());
	pPayload->set_message_data(pSeatMsg->SerializeAsString());
	CMsgMgr::getInstance().insertResponseMsg(pPayload);
}

void zhu::CRoomMgr::SendReadyToOtherPlayer(int iSocket, int iSeat, string strAccount, bool bReady)
{
	// 创建响应消息
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
	std::shared_ptr<zhu::room::ReadyResp> pReadyResp(NEW_ND zhu::room::ReadyResp());
	pPayload->set_type_name(pReadyResp->GetTypeName());
	pPayload->add_socket(iSocket);

	pReadyResp->set_readyresult(zhu::room::ERROR_CODE::SUCCESS);
	pReadyResp->set_position(iSeat);
	pReadyResp->set_account(strAccount);
	pReadyResp->set_ready(bReady);
	pReadyResp->set_desc("player ready");
	logger_info("send {} ready to other player", strAccount);
	pPayload->set_message_data(pReadyResp->SerializeAsString());
	CMsgMgr::getInstance().insertResponseMsg(pPayload);
}

void zhu::CRoomMgr::Start(int iSocket, int iRoomId)
{
	// 创建响应消息
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
	std::shared_ptr<room::RoomGameStatuChangeNotify> pReadyResp(NEW_ND room::RoomGameStatuChangeNotify());
	pPayload->set_type_name(pReadyResp->GetTypeName());
	pPayload->add_socket(iSocket);

	pReadyResp->set_id(iRoomId);
	pReadyResp->set_start(true);
	logger_info("room {} game start", iRoomId);
	pPayload->set_message_data(pReadyResp->SerializeAsString());
	CMsgMgr::getInstance().insertResponseMsg(pPayload);
}

void zhu::CRoomMgr::GameOver(int iSocket, GAME_OVER_PTR pGameOverMsg)
{
	auto pRoom = m_mapRooms[pGameOverMsg->roomid()];
	zhu::room::Seat* seat1 = pRoom->mutable_seat1();
	zhu::room::Seat* seat2 = pRoom->mutable_seat2();
	zhu::room::Seat* seat3 = pRoom->mutable_seat3();

	if (seat1->socket() == iSocket) {
		seat1->set_statu(room::Seat::SeatStatus::Seat_SeatStatus_UNREADY);
	}
	else if (seat2->socket() == iSocket) {
		seat2->set_statu(room::Seat::SeatStatus::Seat_SeatStatus_UNREADY);
	}
	else if (seat3->socket() == iSocket) {
		seat3->set_statu(room::Seat::SeatStatus::Seat_SeatStatus_UNREADY);
	}

	if (pRoom->status() == room::RoomStatus::START) {
		pRoom->set_status(room::RoomStatus::FULL);
		NotifyRoomListennersGameStatuChange(pRoom, false);
	}
}

int zhu::CRoomMgr::GenerateRoomId()
{
	int id = 1;
	while (m_mapRooms.count(id)) {
		if (id > MAX_ROOM_NUMBER)
			break;
		id++;
	}
	
	return id;
}

void zhu::CRoomMgr::CreateSeat(ROOM_PTR pRoom)
{
	auto seat1 = pRoom->mutable_seat1();
	seat1->set_statu(zhu::room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER);
	seat1->set_position(1);

	auto seat2 = pRoom->mutable_seat2();
	seat2->set_statu(zhu::room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER);
	seat2->set_position(2);

	auto seat3 = pRoom->mutable_seat3();
	seat3->set_statu(zhu::room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER);
	seat3->set_position(3);
}

int zhu::CRoomMgr::Seating(ROOM_PTR pRoom, string strAccount, int iSocket)
{
	int position = 0;
	zhu::room::Seat* seat1 = pRoom->mutable_seat1();
	zhu::room::Seat* seat2 = pRoom->mutable_seat2();
	zhu::room::Seat* seat3 = pRoom->mutable_seat3();
	

	if (seat1->statu() == zhu::room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER) {
		seat1->set_statu(zhu::room::Seat::SeatStatus::Seat_SeatStatus_UNREADY);
		seat1->set_playeraccount(strAccount);
		seat1->set_socket(iSocket);
		position = 1;
	}
	else if (seat2->statu() == zhu::room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER) {
		seat2->set_statu(zhu::room::Seat::SeatStatus::Seat_SeatStatus_UNREADY);
		seat2->set_playeraccount(strAccount);
		seat2->set_socket(iSocket);
		position = 2;
	}
	else if (seat3->statu() == zhu::room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER) {
		seat3->set_statu(zhu::room::Seat::SeatStatus::Seat_SeatStatus_UNREADY);
		seat3->set_playeraccount(strAccount);
		seat3->set_socket(iSocket);
		position = 3;
	}

	// 房间人数+1
	if (position != 0) {
		pRoom->set_usercount(pRoom->usercount() + 1);
	}

	return position;
}

zhu::room::Seat* zhu::CRoomMgr::CheckPlayerInRoom(ROOM_PTR pRoom, string strAccount)
{
	zhu::room::Seat* pSeat = NULL;

	zhu::room::Seat* seat1 = pRoom->mutable_seat1();
	zhu::room::Seat* seat2 = pRoom->mutable_seat2();
	zhu::room::Seat* seat3 = pRoom->mutable_seat3();

	if (seat1->statu() != zhu::room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER &&
		seat1->playeraccount() == strAccount) {
		pSeat = seat1;
	}
	else if (seat2->statu() != zhu::room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER &&
			seat2->playeraccount() == strAccount) {
		pSeat = seat2;
	}
	else if (seat3->statu() != zhu::room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER &&
			seat3->playeraccount() == strAccount) {
		pSeat = seat3;
	}

	return pSeat;
}

bool zhu::CRoomMgr::CheckPlayerInRoom(string strAccount)
{
	bool bInRoom = false;
	for (ROOM_ITER it = m_mapRooms.begin(); m_mapRooms.end() != it; it++) {
		if (CheckPlayerInRoom(it->second, strAccount) != NULL) {
			bInRoom = true;
			break;
		}
	}
	return bInRoom;
}

bool zhu::CRoomMgr::CheckRoomShouldBeClose(ROOM_PTR pRoom)
{
	// 获取房间人数
	int iRoomPlayerNumber = GetRoomPlayerNumber(pRoom);

	if (iRoomPlayerNumber == 0) {
		m_mapRooms.erase(pRoom->id());
		NotifyRoomListennersRoomNumberChange(m_mapRooms.size());
		return true;
	}

	return false;
}

int zhu::CRoomMgr::GetRoomPlayerNumber(ROOM_PTR pRoom)
{
	int iNumber = 0;

	zhu::room::Seat seat1 = pRoom->seat1();
	zhu::room::Seat seat2 = pRoom->seat2();
	zhu::room::Seat seat3 = pRoom->seat3();

	if (seat1.statu() != zhu::room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER) {
		iNumber++;
	}

	if (seat2.statu() != zhu::room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER) {
		iNumber++;
	}

	if (seat3.statu() != zhu::room::Seat::SeatStatus::Seat_SeatStatus_NO_PLAYER) {
		iNumber++;
	}
	return iNumber;
}

bool zhu::CRoomMgr::CheckAllPlayerIsReady(ROOM_PTR pRoom)
{
	bool bAllReady = false;
	zhu::room::Seat* seat1 = pRoom->mutable_seat1();
	zhu::room::Seat* seat2 = pRoom->mutable_seat2();
	zhu::room::Seat* seat3 = pRoom->mutable_seat3();

	if (seat1->statu() == zhu::room::Seat::SeatStatus::Seat_SeatStatus_READY &&
		seat2->statu() == zhu::room::Seat::SeatStatus::Seat_SeatStatus_READY &&
		seat3->statu() == zhu::room::Seat::SeatStatus::Seat_SeatStatus_READY) {
		bAllReady = true;
	}

	return bAllReady;
}

void zhu::CRoomMgr::SendReadyToOtherPlayer(ROOM_PTR pRoom, room::Seat* pSeat, string strAccount, bool bReady)
{
	zhu::room::Seat* seat1 = pRoom->mutable_seat1();
	zhu::room::Seat* seat2 = pRoom->mutable_seat2();
	zhu::room::Seat* seat3 = pRoom->mutable_seat3();

	if (seat1 != pSeat && seat1->statu() != room::Seat::NO_PLAYER)
		SendReadyToOtherPlayer(seat1->socket(), pSeat->position(), strAccount, bReady);

	if (seat2 != pSeat && seat2->statu() != room::Seat::NO_PLAYER)
		SendReadyToOtherPlayer(seat2->socket(), pSeat->position(), strAccount, bReady);

	if (seat3 != pSeat && seat3->statu() != room::Seat::NO_PLAYER)
		SendReadyToOtherPlayer(seat3->socket(), pSeat->position(), strAccount, bReady);
}

void zhu::CRoomMgr::SendStartToAllPlayer(ROOM_PTR pRoom)
{
	zhu::room::Seat* seat1 = pRoom->mutable_seat1();
	zhu::room::Seat* seat2 = pRoom->mutable_seat2();
	zhu::room::Seat* seat3 = pRoom->mutable_seat3();

	Start(seat1->socket(), pRoom->id());

	Start(seat2->socket(), pRoom->id());

	Start(seat3->socket(), pRoom->id());
}
