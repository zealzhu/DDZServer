/**
 * @file Protocol.cpp
 * @brief
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-09
 */
#include "Protocol.h"
#include <tools/GameLog.h>
#include <msg/MessageManager.h>
#include <google/protobuf/descriptor.h>

namespace ProtocolLib
{

void Protocol::Translate(GameSocketLib::gsocket sock, const char * buffer, unsigned int size)
{
    // 解码
    MessagePtr msg = Decode(sock, buffer, size);

    // 插入消息队列中
    MessageManager::Instance().InsertReceiveMessage(msg);
}

MessagePtr Protocol::Decode(GameSocketLib::gsocket sock, const char * buffer, unsigned int size)
{
    // 反序列化
    std::shared_ptr<zhu::SelfDescribingMessage> message(new zhu::SelfDescribingMessage());
    std::string data(buffer, size);
    message->ParseFromString(data);
    message->add_socket(sock);

    return message;
}

void Protocol::Encode(const google::protobuf::Message & msg, std::string * out_buffer)
{
    static int size = msg.ByteSize() + RECEIVE_HEADER_SIZE;

    char * buffer = new char[size];
    memset(buffer, 0, size);

    logger_debug("发送大小：{}", msg.ByteSize());

    // 转换成网络字节
    unsigned int parse_size = htonl(size);
    // 将大小装进头部
    memcpy(buffer, &parse_size, RECEIVE_HEADER_SIZE);

    // 序列化
    msg.SerializeToString(out_buffer);
}

unsigned int Protocol::ReadHead(const char * buff, unsigned int size)
{
    unsigned int should_read_size = ntohl(size);
    logger_info("解析头部大小：{}", should_read_size);

    return should_read_size;
}

MessagePtr Protocol::ParseInnerMsg(OutterMessagePtr outter_msg, ErrorCode * error_code)
{
    // 创建一条对应消息
    MessagePtr msg(CreateMessage(outter_msg->type_name()));

    if(msg)
    {
        if(msg->ParseFromString(outter_msg->message_data()))
        {
            // 转换成功
            *error_code = ErrorCode::kSuccess;
            logger_debug("内部消息类型：{}", msg->GetTypeName());
            logger_debug("DebugString: {}", msg->DebugString());

        }
        else
        {
            *error_code = ErrorCode::kParseError;
        }
    }
    else
    {
        *error_code = ErrorCode::kUnknownMessageType;
    }
    return msg;
}

google::protobuf::Message * Protocol::CreateMessage(const std::string & type_name)
{
    // 利用反射机制通过类型名字创建对应消息
    google::protobuf::Message * msg = nullptr;
    auto descriptor =
        google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);

    if(descriptor)
    {
        auto prototype = google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
        if(prototype)
        {
            // 创建对应的消息
            msg = prototype->New();
        }
    }

    return msg;
}

std::string Protocol::ErrorToString(const ErrorCode & error_code)
{
    switch(error_code)
    {
    case ErrorCode::kParseError:
        return "解析失败";
    case ErrorCode::kUnknownMessageType:
        return "未知消息类型";
    default:
        return "未知错误";
    }
}
}

