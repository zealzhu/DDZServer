/**
 * @file GameSocketSet.h
 * @brief 用于tcp非阻塞模式轮询用的集合
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-02
 */
#ifndef _GAME_SOCKET_SET_H
#define _GAME_SOCKET_SET_H

#include "GameSocketType.h"
#include <set>

namespace GameSocketLib
{

const int MAX = FD_SETSIZE;

class GameSocketSet
{
public:
    GameSocketSet();

    /**
     * @brief 添加socket
     *
     * @param sock
     */
    void AddSocket(const gsocket sock);

    /**
     * @brief 移除socket
     *
     * @param sock
     */
    void RemoveSocket(const gsocket sock);

    /**
     * @brief 轮询
     *
     * @param time 轮询间隔
     */
    inline int Poll(long time = 0)
    {
        struct timeval t = {0, time * 1000};    // {秒， 毫秒}

        activity_set_ = set_;

        if(sock_desc_.size() == 0)
            return 0;
        return select(*(sock_desc_.rbegin()) + 1, &activity_set_, 0, 0, &t);
    }

    /**
     * @brief 检测socket是否有活动
     *
     * @param sock 检测的socket
     *
     * @return 返回是否有活动
     */
    inline bool HasActivity(const gsocket sock)
    {
        return FD_ISSET(sock, &activity_set_) != 0;
    }

protected:
    fd_set set_;            // 所有socket的文件描述符set
    fd_set activity_set_;

    std::set<gsocket> sock_desc_;// 用于select时的第一个参数，要求传入最大值+1
};

}

#endif // _GAME_SOCKET_SET_H
