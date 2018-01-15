/**
 * @file Protocol.cpp
 * @brief
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-09
 */
#include "Protocol.h"
#include <tools/GameLog.h>
#include <BaseMsg.pb.h>
#include <msg/MessageManager.h>

namespace ProtocolLib
{

void Protocol::Translate(GameSocketLib::gsocket sock, const char * buffer, unsigned int size)
{
    // 解码
    MessagePtr msg = Decode(sock, buffer, size);

    // 插入消息队列中
    MessageManager::Instance().InsertReceiveMessage(msg);
}

Protocol::MessagePtr Protocol::Decode(GameSocketLib::gsocket sock, const char * buffer, unsigned int size)
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


google::protobuf::Message * Protocol::CreateMessage(const std::string & type_name)
{

}
}

