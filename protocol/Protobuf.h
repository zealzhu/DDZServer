////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhu
// Created： 2017/10/25
// Describe：Protobuf协议类
////////////////////////////////////////////////////////////////////////

#ifndef CPROTOBUF_H
#define CPROTOBUF_H

#include <string>
#include "../socket/SocketLib.h"
#include "../message/BaseMsg.pb.h"
#include <google/protobuf/message.h>


typedef std::shared_ptr<google::protobuf::Message> MessagePtr;

namespace zhu
{

class CProtobuf
{

public:
    typedef SocketLib::Connection<CProtobuf> connection;

	// This is the default header size of the protobuf package
	const static int HEADERRSIZE = 4;

	enum ErrorCode
	{
		SUCCESS = 0,
		INVALID_NAME_LEN,
		UNKNOWN_MESSAGE_TYPE,
		PARSE_ERROR,
	};

    // ------------------------------------------------------------------------
    //  解码并将数据包插入消息队列
    // ------------------------------------------------------------------------
    void Translate( connection& conn, char* p_buffer, int iSize = 0);

    // ------------------------------------------------------------------------
    //  将数据编码并存放到该连接的发送缓存中
    // ------------------------------------------------------------------------
    void SendString( connection& conn, void* pMsg );

    // ------------------------------------------------------------------------
    //  构造函数
    // ------------------------------------------------------------------------
    inline CProtobuf()
    {
        m_iBuffersize = 0;
    }

    inline int Buffered()   { return m_iBuffersize; }

	// 将Message编码成字节流，并设置大小
	static const char* Encode(const google::protobuf::Message& message, int& iBufSize);

	// 将错误码转换成对应的文本
	static const std::string& ErrorCodeToString(ErrorCode errorCode);

	// 创建Message
	static google::protobuf::Message* CreateMessage(const std::string& strTypeName);

	// 从char数组解码成Message并将设置sock值
	static MessagePtr Decode(int csock, google::protobuf::uint32 iSize);

	// 解析Message成真正有用的数据包
	static MessagePtr parseInnerMsg(std::shared_ptr<zhu::SelfDescribingMessage> pOutterMsg, ErrorCode& error);

	// 读取报头（报文大小字段）
	static unsigned int ReadHdr(char* pBuf);

protected:

    // This is the buffer that will contain all the processed data
    char m_buffer[SocketLib::BUFFERSIZE];

    int m_iBuffersize;
};


}   // end namespace BetterMUD

#endif
