/**
 * @file Protocol.h
 * @brief 协议类
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-05
 */
#ifndef _GAME_PROTOCOL_H
#define _GAME_PROTOCOL_H

#include <socket/GameSocketType.h>
#include <google/protobuf/message.h>
#include <BaseMsg.pb.h>

const static int RECEIVE_HEADER_SIZE = sizeof(int);

namespace ProtocolLib
{
typedef std::shared_ptr<google::protobuf::Message> MessagePtr;
typedef std::shared_ptr< zhu::SelfDescribingMessage > OutterMessagePtr;

enum ErrorCode
{
    kSuccess = 0,
    kInvalidNameLen,
    kUnknownMessageType,
    kParseError,
};

class Protocol
{
public:
    // 序列化数据成消息并插入消息队列
    static void Translate(GameSocketLib::gsocket sock, const char * buffer, unsigned int size);

    // 解码
    static MessagePtr Decode(GameSocketLib::gsocket sock, const char * buffer, unsigned int size);

    // 编码
    static void Encode(const google::protobuf::Message & msg, std::string * out_buffer);

    // 读取头部大小
    static unsigned int ReadHead(const char * buff, unsigned int size);

    /**
     * @brief 将SelfDescribingMessage包装的消息转换为对应typename消息
     *
     * @param outter_msg 最外部的消息
     * @param error_code 错误代码
     *
     * @return
     */
    static MessagePtr ParseInnerMsg(std::shared_ptr<zhu::SelfDescribingMessage> outter_msg, ErrorCode * error_code);

    static std::string ErrorToString(const ErrorCode & error_code);

protected:
    // 通过类型创建一条对应消息
    static google::protobuf::Message * CreateMessage(const std::string & type_name);
};

}

#endif //_GAME_PROTOCOL_H

