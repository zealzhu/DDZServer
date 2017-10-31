////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhou du
// Created： 2017/07/10
// Describe：模块接口
////////////////////////////////////////////////////////////////////////

#ifndef IMODULE_H
#define IMODULE_H

#include <google/protobuf/message.h>
#include "../message/BaseMsg.pb.h"

using namespace std;

namespace zhu
{
    class IModule
    {
    public:

		virtual bool Init() = 0;

		virtual bool Start() = 0;

		virtual bool Stop() = 0;

		virtual bool Reload() = 0;

		virtual void HandleMsg(std::shared_ptr<SelfDescribingMessage> pMsg) = 0;	//处理消息

		virtual string GetModuleName() = 0;												//获取模块名

	};

} // end namespace BasicLib


#endif
