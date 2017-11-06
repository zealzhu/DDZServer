#include "../../utils/StringUtils.h"
#include "../../basic/Basic.h"
#include "../../protocol/Protobuf.h"
#include "../../message/MsgMgr.h"
#include "../../socket/SocketLibSystem.h"
#include "UserMgr.h"
#include "../../protocol/Protobuf.h"

void zhu::CUserMgr::HandleMsg(std::shared_ptr<SelfDescribingMessage> pMsg)
{
	CProtobuf::ErrorCode pErrorCode;
	MessagePtr pInnerMsg = CProtobuf::parseInnerMsg(pMsg, pErrorCode);
	if (CProtobuf::ErrorCode::UNKNOWN_MESSAGE_TYPE == pErrorCode || CProtobuf::ErrorCode::PARSE_ERROR == pErrorCode)
	{
		logger_error("CProtobuf parseInnerMsg failed");
		std::shared_ptr<SelfDescribingMessage> pPayload(NEW_ND SelfDescribingMessage());
		pPayload->add_socket(pMsg->socket(0));
		std::shared_ptr<ErrorMessage> pErrorMessage(NEW_ND ErrorMessage());
		pErrorMessage->set_desc("account already exist");
		pPayload->set_message_data(pErrorMessage->SerializeAsString());
		pPayload->set_type_name(pErrorMessage->GetTypeName());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}
	// 注册
	if ("zhu.user.Player" == pMsg->type_name())
	{
		logger_debug("begin handle register msg");
		if (NULL != pInnerMsg)
		{
			PLAYER_PTR pRegisterReq = dynamic_pointer_cast<zhu::user::Player>(pInnerMsg);
			Register(pMsg->socket(0), pRegisterReq);
		}
	} 
	// 登录
	else if ("zhu.user.LoginReq" == pMsg->type_name())
	{
		logger_debug("begin handle login msg");
		if (NULL != pInnerMsg)
		{
			LOGIN_PTR pLoginReq = dynamic_pointer_cast<zhu::user::LoginReq>(pInnerMsg);
			Login(pMsg->socket(0), pLoginReq);
		}
	}
	// 注销
	else if ("zhu.user.LogoutReq" == pMsg->type_name())
	{
		logger_debug("begin handle logout msg");
		if (NULL != pInnerMsg)
		{
			LOGOUT_PTR pLogoutReq = dynamic_pointer_cast<zhu::user::LogoutReq>(pInnerMsg);
			Logout(pMsg->socket(0), pLogoutReq);
		}
	}
	else
	{
		logger_debug("discard unknown user module msg {}", pMsg->GetTypeName());
	}
}

void zhu::CUserMgr::Register(int iSocket, PLAYER_PTR pRegisterReq)
{
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
	std::shared_ptr<zhu::ErrorMessage> pErrorMessage(NEW_ND zhu::ErrorMessage());
	pPayload->set_type_name(pErrorMessage->GetTypeName());
	pPayload->add_socket(iSocket);
	if (!ValidateRegisterReq(pRegisterReq, *pErrorMessage))
	{
		logger_error("{}", pErrorMessage->desc());
		pPayload->set_message_data(pErrorMessage->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}

	if (CUserDao::Instance().IsExist(pRegisterReq->account().c_str()))
	{
		pErrorMessage->set_desc("account already exist");
		logger_error("account {} already exist", pRegisterReq->account());
		pPayload->set_message_data(pErrorMessage->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}
	// 添加新账号
	if (CUserDao::Instance().AddNewAccount(pRegisterReq))
	{
		pErrorMessage->set_desc("register account successful");
		logger_info("register account successful");
		pPayload->set_message_data(pErrorMessage->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
	}
	else
	{
		logger_error("register account failed");
		pErrorMessage->set_desc("register account failed");
		pPayload->set_message_data(pErrorMessage->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
	}
}

bool zhu::CUserMgr::ValidateRegisterReq(PLAYER_PTR pRegisterReq, zhu::ErrorMessage& errorMessage)
{
	if (!CStringUtils::IsValidAccount(pRegisterReq->account()))
	{
		errorMessage.set_desc("account invalid, must be 3-16 number of alpha or digit");
		return false;
	}
	if (!CStringUtils::IsValidPassword(pRegisterReq->password()))
	{
		errorMessage.set_desc("password invalid");
		return false;
	}
	if (pRegisterReq->name().length() < 1 || pRegisterReq->name().length() > 16)
	{
		errorMessage.set_desc("name invalid");
		return false;
	}
	if (pRegisterReq->email().length() > 0 && !CStringUtils::IsValidMail(pRegisterReq->email()))
	{
		errorMessage.set_desc("email invalid");
		return false;
	}
	for (int i = 0; i < pRegisterReq->phones_size(); i++)
	{
		if (zhu::user::Player_PhoneType_MOBILE == pRegisterReq->phones(i).type()
			&& !CStringUtils::IsValidMobile(pRegisterReq->phones(i).number()))
		{
			errorMessage.set_desc("mobile invalid");
			return false;
		}
		else if (zhu::user::Player_PhoneType_HOME == pRegisterReq->phones(i).type()
			&& !CStringUtils::IsValidHomeNumber(pRegisterReq->phones(i).number()))
		{
			errorMessage.set_desc("home phone invalid");
			return false;
		}
	}

	return true;
}

void zhu::CUserMgr::Login(int iSocket, LOGIN_PTR pLoginMsg)
{
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
	std::shared_ptr<zhu::user::LoginResp> pLoginResult(NEW_ND zhu::user::LoginResp());
	pPayload->set_type_name(pLoginResult->GetTypeName());
	pPayload->add_socket(iSocket);
	if (!ValidateLoginReq(pLoginMsg, *pLoginResult))
	{
		logger_error("{}", pLoginResult->desc());
		pPayload->set_message_data(pLoginResult->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}

	if (!CUserDao::Instance().IsExist(pLoginMsg->account().c_str()))
	{
		pLoginResult->set_loginresult(zhu::user::ERROR_CODE::ACCOUNT_NOT_EXIST);
		pLoginResult->set_desc("account not exist");
		logger_error("account not exist");
		pPayload->set_message_data(pLoginResult->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}

	// 检查账号密码
	PLAYER_PTR pPlayer = CUserDao::Instance().GetPlayer(pLoginMsg->account().c_str(), pLoginMsg->password().c_str());
	if (pPlayer->id() > 0)
	{
		//// 检查是否已经在线
		//if (pPlayer->status() == zhu::user::UserStatus::ONLINE) {
		//	pLoginResult->set_loginresult(zhu::user::ERROR_CODE::ALREADY_LOGON);
		//	pLoginResult->set_desc("the account has been logined");
		//	logger_error("uer account {} login failed", pLoginMsg->account());
		//	pPayload->set_message_data(pLoginResult->SerializeAsString());
		//	CMsgMgr::getInstance().insertResponseMsg(pPayload);
		//	return;
		//}

		pPlayer->set_socket(iSocket);
		m_mapPlayers[pPlayer->id()] = pPlayer;
		logger_debug("Player info: {}", pPlayer->DebugString());

		// 修改账号状态为登录
		CUserDao::Instance().ChangeAccountStatus(pPlayer->account().c_str(), zhu::user::UserStatus::ONLINE);
		pLoginResult->set_loginresult(zhu::user::ERROR_CODE::SUCCESS);
		pLoginResult->set_desc("login success");
		pLoginResult->set_account(pPlayer->account());
		pLoginResult->set_id(pPlayer->id());
		logger_info("login success");
		pPayload->set_message_data(pLoginResult->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);

		// 发送用户上线通知给其他用户
		if (m_mapPlayers.size() > 1)
		{
			std::shared_ptr<zhu::SelfDescribingMessage> pNotifyPayload(NEW_ND zhu::SelfDescribingMessage());
			std::shared_ptr<zhu::user::OnlineOfflineNotify> pOnlineNotify(NEW_ND zhu::user::OnlineOfflineNotify());
			pNotifyPayload->set_type_name(pOnlineNotify->GetTypeName());
			for each (auto player in m_mapPlayers)
			{
				if (player.first != pPlayer->id())
				{
					pNotifyPayload->add_socket((player.second)->socket());
				}
			}
			pOnlineNotify->set_account(pPlayer->account());
			pOnlineNotify->set_online(true);
			pNotifyPayload->set_message_data(pOnlineNotify->SerializeAsString());
			CMsgMgr::getInstance().insertResponseMsg(pNotifyPayload);
		}

		// 发送用户上线通知给其他模块
		NotifyUserListenners(pPlayer->account(), true);
	}
	else
	{
		pLoginResult->set_loginresult(zhu::user::ERROR_CODE::ACCOUNT_OR_PASSWD_ERROR);
		pLoginResult->set_desc("login failed, please check account or password");
		logger_error("uer account {} login failed", pLoginMsg->account());
		pPayload->set_message_data(pLoginResult->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}

	pPlayer->set_socket(iSocket);
	m_mapPlayers[pPlayer->id()] = pPlayer;
	logger_debug("Player info: {}", pPlayer->DebugString());

	// 修改账号状态为登录
	CUserDao::Instance().ChangeAccountStatus(pPlayer->account().c_str(), zhu::user::UserStatus::ONLINE);
	pLoginResult->set_loginresult(zhu::user::ERROR_CODE::SUCCESS);
	pLoginResult->set_desc("login success");
	pLoginResult->set_account(pPlayer->account());
	pLoginResult->set_id(pPlayer->id());
	logger_info("login success");
	pPayload->set_message_data(pLoginResult->SerializeAsString());
	CMsgMgr::getInstance().insertResponseMsg(pPayload);
}

bool zhu::CUserMgr::ValidateLoginReq(LOGIN_PTR pLoginMsg, zhu::user::LoginResp& errorMessage)
{
	if (!CStringUtils::IsValidAccount(pLoginMsg->account()))
	{
		errorMessage.set_desc("account invalid");
		errorMessage.set_loginresult(zhu::user::ERROR_CODE::ILLEGAL_PARAM);
		return false;
	}
	if (!CStringUtils::IsValidPassword(pLoginMsg->password()))
	{
		errorMessage.set_desc("password invalid");
		errorMessage.set_loginresult(zhu::user::ERROR_CODE::ILLEGAL_PARAM);
		return false;
	}
	return true;
}

void zhu::CUserMgr::Logout(int iSocket, LOGOUT_PTR logoutReq)
{
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
	std::shared_ptr<zhu::user::LogoutResp> pLogoutResp(NEW_ND zhu::user::LogoutResp());
	pPayload->set_type_name(pLogoutResp->GetTypeName());
	pPayload->add_socket(iSocket);

	if (!CUserDao::Instance().IsExist(logoutReq->account().c_str()))
	{
		pLogoutResp->set_logoutresult(zhu::user::ERROR_CODE::ACCOUNT_NOT_EXIST);
		logger_error("account {} not found", logoutReq->account());
		pPayload->set_message_data(pLogoutResp->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pPayload);
		return;
	}

	// 修改账号状态
	CUserDao::Instance().ChangeAccountStatus(logoutReq->account().c_str(), zhu::user::UserStatus::OFFLINE);
	// 移除在线用户
	m_mapPlayers.erase(logoutReq->id());
	pLogoutResp->set_logoutresult(zhu::user::ERROR_CODE::SUCCESS);
	logger_info("logout success");
	pPayload->set_message_data(pLogoutResp->SerializeAsString());
	CMsgMgr::getInstance().insertResponseMsg(pPayload);

	// 通知其他在线用户该用户下线
	if (m_mapPlayers.size() > 0)
	{
		std::shared_ptr<zhu::SelfDescribingMessage> pNotifyPayload(NEW_ND zhu::SelfDescribingMessage());
		std::shared_ptr<zhu::user::OnlineOfflineNotify> pOnlineNotify(NEW_ND zhu::user::OnlineOfflineNotify());
		pNotifyPayload->set_type_name(pOnlineNotify->GetTypeName());
		for each (auto player in m_mapPlayers)
		{
			if (player.second->socket() != iSocket)
			{
				pNotifyPayload->add_socket(player.second->socket());
			}
		}
		pOnlineNotify->set_account(logoutReq->account());
		pOnlineNotify->set_online(false);
		pNotifyPayload->set_message_data(pOnlineNotify->SerializeAsString());
		CMsgMgr::getInstance().insertResponseMsg(pNotifyPayload);
	}

	// 发送用户下线通知给其他模块
	NotifyUserListenners(logoutReq->account(), false);
}

void zhu::CUserMgr::ConnectionClosed(SocketLib::DataSocket sock)
{
	PLAYER_ITER it = m_mapPlayers.begin();
	// 离线用户
	PLAYER_PTR pOffLineUser;
	while (it != m_mapPlayers.end())
	{
		if (it->second->socket() == sock.GetSock())
		{
			pOffLineUser = it->second;
			//切换为离线状态
			//CUserDao::Instance().ChangeAccountStatus(pOffLineUser->account(), nd::user::UserStatus::OFFLINE);
			break;
		}
		it++;
	}
	

	if (pOffLineUser.get() != NULL)
	{
		// 通知其他在线用户该用户下线
		if (m_mapPlayers.size() > 0)
		{
			std::shared_ptr<zhu::SelfDescribingMessage> pNotifyPayload(NEW_ND zhu::SelfDescribingMessage());
			std::shared_ptr<zhu::user::OnlineOfflineNotify> pOfflineNotify(NEW_ND zhu::user::OnlineOfflineNotify());
			pNotifyPayload->set_type_name(pOfflineNotify->GetTypeName());
			for each (auto player in m_mapPlayers)
			{
				if (player.second->socket() != sock.GetSock())
				{
					pNotifyPayload->add_socket(player.second->socket());
				}
			}
			pOfflineNotify->set_account(pOffLineUser->account());
			pOfflineNotify->set_online(false);
			pNotifyPayload->set_message_data(pOfflineNotify->SerializeAsString());
			CMsgMgr::getInstance().insertResponseMsg(pNotifyPayload);
		}

		// 发送用户下线通知给其他模块
		NotifyUserListenners(pOffLineUser->account(), false);

		logger_info("user {} connection {}:{} closed, remove online player", pOffLineUser->account(),
			SocketLib::GetIPString(sock.GetLocalAddress()), sock.GetRemotePort());
		m_mapPlayers.erase(pOffLineUser->id());
	}
}

void zhu::CUserMgr::NotifyUserListenners(const string& strAccount, bool bOnline)
{
	if (bOnline)
	{
		for each (auto listener in m_listenerList)
		{
			listener->UserOnline(strAccount);
		}
	} 
	else
	{
		for each (auto listener in m_listenerList)
		{
			listener->UserOffline(strAccount);
		}
	}
}
