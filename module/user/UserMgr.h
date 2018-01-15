/**
 * @file UserMgr.h
 * @brief 用户模块
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-10
 */
#ifndef _USER_MGR_H
#define _USER_MGR_H

// ========================================================================
//  Include Files
// ========================================================================
#include <module/ModuleInterface.h>
#include "UserDao.h"

#include <list>

using namespace std;

namespace GameModule
{
class UserStatusChangeEvent
{
public:

    virtual void UserOnline(const string& account) = 0;

    virtual void UserOffline(const string& account) = 0;

};

class UserMgr : public ModuleInterface
{
    // UID <-> PLAYER
    typedef std::map< int, PlayerPtr > PlayerMap;
    typedef PlayerMap::iterator PlayerIter;

    typedef std::shared_ptr< UserStatusChangeEvent > UserStatusListenner;
    typedef std::list< UserStatusListenner > UserStatusListennerList;
    typedef typename UserStatusListennerList::iterator UserStatusListennerIter;

public:
    UserMgr();

    ~UserMgr();

    virtual bool Init() override
    {
        return true;
    }


    virtual bool Start() override
    {
        return true;
    }


    virtual bool Stop() override
    {
        return true;
    }


    virtual bool Reload() override
    {
        return true;
    }


    virtual void HandleMessage(MessagePtr msg) override;

    virtual string GetModuleName() override
    {
        return "zhu.user";
    }

    //virtual void ConnectionClosed(SocketLib::DataSocket sock) override;

    // ------------------------------------------------------------------------
    // Description: This add listener to m_listenners.
    // ------------------------------------------------------------------------
    void AddListener(UserStatusListenner listenner)
    {
        this->listener_list_.push_back(listenner);
    }

    // ------------------------------------------------------------------------
    // Description: notify all listenners user status.
    // ------------------------------------------------------------------------
    void NotifyUserListenners(const string& account, bool online);

private:
    //void Login(int socket, LOGIN_PTR pLoginReq);

    //bool ValidateLoginReq(LOGIN_PTR pLoginReq, zhu::user::LoginResp& errorMessage);

    //void Register(int iSocket, PLAYER_PTR pRegisterReq);

    //bool ValidateRegisterReq(PLAYER_PTR pRegisterReq, ErrorMessage& errorMessage);

    //void Logout(int iSocket, LOGOUT_PTR pLogoutReq);

    PlayerMap player_map_;//用户列表

    UserStatusListennerList listener_list_;
};

} // end namespace BasicLib


#endif
