/**
 * @file UserDao.h
 * @brief 用户数据帮助类
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-15
 */
#ifndef _USER_DAO_H
#define _USER_DAO_H

#include <Player.pb.h>

namespace GameModule
{
typedef std::shared_ptr<zhu::user::Player> PlayerPtr;
typedef std::shared_ptr<zhu::user::LoginReq> LoginRequestPtr;
typedef std::shared_ptr<zhu::user::LogoutReq> LogoutRequestPtr;

class UserDao
{
public:
    static UserDao& Instance()
    {
        static UserDao instance;
        return instance;
    }

    /**
     * @brief 判断用户是否存在
     *
     * @param id 用户id
     *
     * @return
     */
    bool IsExist(int id);

    /**
     * @brief 判断用户是否存在
     *
     * @param account 用户名
     *
     * @return
     */
    bool IsExist(const char * account);

    /**
     * @brief 根据用户名和用户密码获取用户信息
     *
     * @param account 用户名
     * @param password 用户密码
     *
     * @return
     */
    PlayerPtr GetPlayer(const char * account, const char * password);

    /**
     * @brief 用户状态转换成字符
     *
     * @param status
     *
     * @return
     */
    zhu::user::UserStatus StatusFromString(std::string status);

    /**
     * @brief 增加用户
     *
     * @param register_req 用户信息
     *
     * @return
     */
    bool AddNewAccount(PlayerPtr register_req);

    /**
     * @brief 改变用户状态
     *
     * @param account 用户名
     * @param status 用户状态枚举
     *
     * @return
     */
    bool ChangeAccountStatus(const char * account, zhu::user::UserStatus status);

private:
    UserDao() {};
    ~UserDao() {};
    UserDao(const UserDao&) {};
    UserDao& operator=(const UserDao&) {};
    std::string StatusToString(zhu::user::UserStatus status);
    std::string SexToString(zhu::user::Player_SexType sex);
public:
};

} // namespace GameModule

#endif // _USER_DAO_H
