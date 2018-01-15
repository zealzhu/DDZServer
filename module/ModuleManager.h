/**
 * @file MoudleManager.h
 * @brief 模块管理器
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-10
 */

#ifndef _MODULE_MANAGER_H
#define _MODULE_MANAGER_H

#include "ModuleInterface.h"
#include "user/UserMgr.h"
//#include "room/RoomMgr.h"
//#include "table/Table.h"
//#include "score/ScoreMgr.h"

#include <socket/ConnectionManager.h>
#include <tools/GameLog.h>

namespace GameModule
{

class ModuleManager
{
    typedef std::shared_ptr<ModuleInterface> ModulePtr;
    typedef std::set<ModulePtr> ModuleSet;
    typedef ModuleSet::iterator ModuleIter;
public:
    /**
     * @brief 获取实例
     *
     * @return
     */
    static ModuleManager & Instance()
    {
        static ModuleManager instance;
        return instance;
    }

    bool AddModule(ModulePtr module)
    {
        std::pair<ModuleIter, bool> pair = this->module_set_.insert(module);

        return pair.second;
    }

    void RemoveModule(ModuleIter iter)
    {
        this->module_set_.erase(iter);
    }

    void RemoveAllModule()
    {
        ModuleIter it;
        while(!this->module_set_.empty())
        {
            it = this->module_set_.begin();
            RemoveModule(it);
        }
    }

    ModuleSet GetAllModule()
    {
        return this->module_set_;
    }

    void InitModules()
    {
        // 这里面创建模块
        std::shared_ptr< UserMgr > user_mgr(new UserMgr());
        user_mgr->Init();
        this->AddModule(user_mgr);
    }

    /**
     * @brief
     *
     * @param type_name
     *
     * @return
     */
    std::string GetModuleName(std::string type_name)
    {
        int index = type_name.find_last_of(".");
        std::string module_name = type_name;

        if(index != std::string::npos)
        {
            module_name = type_name.substr(0, index);
        }
        return module_name;
    }

    void DispatchMessage(std::shared_ptr<google::protobuf::Message> message)
    {
        std::shared_ptr<zhu::SelfDescribingMessage> msg = std::dynamic_pointer_cast<zhu::SelfDescribingMessage>(message);
        if(nullptr == msg.get())
        {
            logger_warn("消息转换失败");
            return;
        }

        std::string module_type = this->GetModuleName(msg->type_name());
        auto find = std::find_if(this->module_set_.begin(), this->module_set_.end(), [module_type](const ModulePtr & module){
            return module->GetModuleName() == module_type;
        });
        if(find != this->module_set_.end())
        {
            // find 是迭代器, *find才表示智能指针对象
            (*find)->HandleMessage(msg);
        }
        else
        {
            logger_warn("找不到消息模块：{}", module_type);
        }
    }

    inline void SetConnectionManager(GameSocketLib::ConnectionManager & conn)
    {
        this->connection_manager_ = &conn;
    }

private:
    ModuleManager()
        : connection_manager_(nullptr)
    {}
    ~ModuleManager() = default;
    ModuleManager(const ModuleManager &) = delete;
    ModuleManager & operator=(const ModuleManager &) = delete;

    ModuleSet module_set_;
    GameSocketLib::ConnectionManager * connection_manager_;

}; // end ModuleManager

} // end GameModule
#endif // _MODULE_MANAGER_H

