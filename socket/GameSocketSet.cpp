/**
 * @file GameSocketSet.cpp
 * @brief
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-02
 */
#include "GameSocketSet.h"

namespace GameSocketLib
{

GameSocketSet::GameSocketSet()
{
    FD_ZERO(&this->set_);
    FD_ZERO(&this->activity_set_);
}

void GameSocketSet::AddSocket(gsocket sock)
{
    FD_SET(sock, &this->set_);

    this->sock_desc_.insert(sock);
}

void GameSocketSet::RemoveSocket(gsocket sock)
{
    FD_CLR(sock, &this->set_);

    this->sock_desc_.erase(sock);
}

}
