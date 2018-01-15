/**
 * @file UserMgr.cpp
 * @brief 用户模块
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-10
 */
#include "UserMgr.h"
#include <protocol/Protocol.h>
#include <msg/MessageManager.h>
#include <tools/GameLog.h>

using namespace ProtocolLib;

namespace GameModule
{
UserMgr::UserMgr()
{}

UserMgr::~UserMgr()
{}

void UserMgr::HandleMessage(OutterMessagePtr msg)
{
    ErrorCode error_code;
    Protocol::ParseInnerMsg(msg, &error_code);

    if(error_code == ErrorCode::kParseError || error_code == ErrorCode::kUnknownMessageType)
    {
        // 发送失败消息
        logger_error("转换消息失败", Protocol::ErrorToString(error_code));
        OutterMessagePtr outter_msg(new zhu::SelfDescribingMessage);
        outter_msg->add_socket(msg->socket(0));

        // 创建失败消息
        std::shared_ptr< zhu::ErrorMessage > error_msg(new zhu::ErrorMessage);
        error_msg->set_desc(Protocol::ErrorToString(error_code));

        outter_msg->set_message_data(error_msg->SerializeAsString());
        outter_msg->set_type_name(error_msg->GetTypeName());

        MessageManager::Instance().InsertResponseMessage(outter_msg);
        return;
    }
}

}
