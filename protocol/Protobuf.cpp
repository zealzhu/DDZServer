////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhu
// Created： 2017/10/26
// Describe：Protobuf协议类
////////////////////////////////////////////////////////////////////////

//#include "../utils/StringUtils.h"
#include "Protobuf.h"
#include "../message/MsgMgr.h"

#include "winsock2.h"
#include <iostream>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include "./socket/SocketLibErrors.h"
#include "./basic/basic.h"
#include "./basic/GameLog.h"

#pragma comment(lib, "ws2_32.lib")

using namespace google::protobuf;
using namespace google::protobuf::io;
using namespace SocketLib;

namespace zhu
{
	const std::string STR_SUCCESS = "Success";
	const std::string STR_INVALID_NAME_LEN = "InvalidNameLen";
	const std::string STR_UNKNOWN_MESSAGE_TYPE = "UnknownMessageType";
	const std::string STR_PARSE_ERROR = "ParseError";
	const std::string STR_UNKNOWN_ERROR = "UnknownError";

// ------------------------------------------------------------------------
//  Translates raw byte data into protobuf data, and insert into the
//  message queue
// ------------------------------------------------------------------------
void CProtobuf::Translate( connection& conn, char* pBuffer, int iSize )
{	
	// 解码
	MessagePtr messagePtr = CProtobuf::Decode(conn.GetSock(), CProtobuf::ReadHdr(pBuffer));

	// 插入消息队列
	zhu::CMsgMgr::getInstance().insertReceivedMsg(messagePtr);
}


// ------------------------------------------------------------------------
//  This sends a protobuf data to the connection
// ------------------------------------------------------------------------
void CProtobuf::SendString( connection& conn, void* pMsg )
{
    // just buffer the data on the connection
	int bufLen = 0;
	google::protobuf::Message* pProtobufMsg = (google::protobuf::Message*)pMsg;
	// 编码成字节流
	const char* encode = CProtobuf::Encode(*pProtobufMsg, bufLen);
	// 插入发送缓存中
    conn.BufferData(encode, bufLen);
	// 释放该缓存区
	SAFE_DELETE_ARRAY(encode);
}

const std::string& CProtobuf::ErrorCodeToString(ErrorCode errorCode)
{
	switch (errorCode)
	{
	case SUCCESS:
		return STR_SUCCESS;
	case INVALID_NAME_LEN:
		return STR_INVALID_NAME_LEN;
	case UNKNOWN_MESSAGE_TYPE:
		return STR_UNKNOWN_MESSAGE_TYPE;
	case PARSE_ERROR:
		return STR_PARSE_ERROR;
	default:
		return STR_UNKNOWN_ERROR;
	}
}

google::protobuf::Message* CProtobuf::CreateMessage(const std::string& strTypeName)
{
	google::protobuf::Message* pMessage = NULL;
	const google::protobuf::Descriptor* pDescriptor =
		google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(strTypeName);
	if (pDescriptor)
	{
		const google::protobuf::Message* pPrototype =
			google::protobuf::MessageFactory::generated_factory()->GetPrototype(pDescriptor);
		if (pPrototype)
		{
			pMessage = pPrototype->New();
		}
	}
	return pMessage;
}

const char* CProtobuf::Encode(const google::protobuf::Message& pMessage, int& iBufSize)
{
	// 4个字节的头部（长度）+数据包内容长度
	iBufSize = pMessage.ByteSize() + HEADERRSIZE;
	// 创建一个该大小的缓存区
	char *pBuf = NEW_ND char[iBufSize];
	// 清零
	ZeroMemory(pBuf, iBufSize);
	// 将本地字节流转换为网络字节流
	logger_debug("send byte size：{}", pMessage.ByteSize());
	unsigned int iNumber = htonl(pMessage.ByteSize());
	logger_debug("htonl size：{}", iNumber);
	// 头四个字节用来存储数据包大小
	memcpy(pBuf, &iNumber, HEADERRSIZE);
	// 以下就是将protobuf数据序列化到内存中
	ArrayOutputStream aos(pBuf + HEADERRSIZE, pMessage.ByteSize());
	CodedOutputStream pCodedOutput(&aos);
	pMessage.SerializeToCodedStream(&pCodedOutput);

	// 返回编码后的缓存区，需要手动自己释放
	return pBuf;
}

unsigned int CProtobuf::ReadHdr(char *pBuf)
{
	unsigned int iSize;
	//Decode the HDR and get the size
	//iSize = (pBuf[0] << 24) | (pBuf[1] << 16) | (pBuf[2] << 8) | pBuf[3];
	iSize = ntohl(*(unsigned int *)pBuf);
	logger_trace("size of payload is {}", iSize);
	return iSize;
}

MessagePtr CProtobuf::Decode(int csock, google::protobuf::uint32 iSize)
{
	unsigned uiByteCount = 0;
	std::shared_ptr<zhu::SelfDescribingMessage> pPayload(NEW_ND zhu::SelfDescribingMessage());
	char* pBuffer = NEW_ND char[iSize];//size of the payload and hdr
	ZeroMemory(pBuffer, iSize);
	//Read the entire buffer including the hdr
	unsigned uiTotal = 0;
	while (uiTotal < iSize && (uiByteCount = recv(csock, pBuffer + uiByteCount, iSize - uiByteCount, 0)) > 0)
	{
		if (uiByteCount == -1)
		{
			SAFE_DELETE_ARRAY(pBuffer);
			throw SocketLib::Exception(GetError());
		}

		uiTotal += uiByteCount;
	}
	logger_trace("Second read byte count is {}", uiByteCount);
	//Assign ArrayInputStream with enough memory 
	google::protobuf::io::ArrayInputStream ais(pBuffer, iSize);
	CodedInputStream codedInput(&ais);
	//De-Serialize
	pPayload->ParseFromCodedStream(&codedInput);
	pPayload->add_socket(csock);
	//logger_debug("inner TypeName is {}", payload->type_name());

	SAFE_DELETE_ARRAY(pBuffer);

	return pPayload;
}

MessagePtr CProtobuf::parseInnerMsg(std::shared_ptr<zhu::SelfDescribingMessage> pOutterMsg, ErrorCode& error)
{
	MessagePtr pMessage(CProtobuf::CreateMessage(pOutterMsg->type_name()));
	if (pMessage)
	{
		if (pMessage->ParseFromString(pOutterMsg->message_data()))
		{
			error = SUCCESS;
			logger_debug("inner TypeName is {}", pMessage->GetTypeName());
			logger_debug("payload is {}", pMessage->DebugString());
		}
		else
		{
			error = PARSE_ERROR;
		}
	}
	else
	{
		error = UNKNOWN_MESSAGE_TYPE;
	}

	return pMessage;
}

}   // end namespace nd
