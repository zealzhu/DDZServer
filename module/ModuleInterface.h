/**
 * @file ModuleInterface.h
 * @brief 模块接口
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-10
 */
#ifndef _MODULE_INTERFACE_H
#define _MODULE_INTERFACE_H

#include <BaseMsg.pb.h>

namespace GameModule
{

class ModuleInterface
{
public:
    virtual bool Init() = 0;
    virtual bool Start() = 0;
    virtual bool Stop() = 0;
    virtual bool Reload() = 0;

    virtual void HandleMessage(std::shared_ptr<zhu::SelfDescribingMessage> message) = 0;

    virtual std::string GetModuleName() = 0;

    virtual ~ModuleInterface() {}
}; // end ModuleInterface

}

#endif // _MODULE_INTERFACE_H
