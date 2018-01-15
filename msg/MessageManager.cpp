/**
 * @file MessageManager.cpp
 * @brief
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-09
 */
#include "MessageManager.h"
#include <tools/ConfigManager.h>

void MessageManager::Init()
{
    std::string msg_queue_size = GameTools::ConfigManager::GetConfigParam("msg.maxsize", "9999");

    int max_size = atoi(msg_queue_size.c_str());

    if(max_size == 0)
    {
        max_size = MAX_MSG_SIZE;
    }
    this->recv_deque_semaphore_.SetCount(max_size);
    this->send_deque_semaphore_.SetCount(max_size);
}

bool MessageManager::IsReceiveDequeEmpty()
{
    bool flag = false;
    this->recv_lock_.lock();

    flag = this->recv_deque_.empty();

    this->recv_lock_.unlock();

    return flag;
}

bool MessageManager::IsResponseDequeEmpty()
{
    bool flag = false;
    this->recv_lock_.lock();

    flag = this->send_deque_.empty();

    this->recv_lock_.unlock();

    return flag;
}

void MessageManager::InsertResponseMessage(MESSAGE_PTR msg)
{
    this->send_deque_semaphore_.Wait();

    {
        std::lock_guard<std::mutex> lock(this->send_lock_);
        this->send_deque_.emplace_back(msg);
    }

    this->send_deque_semaphore_.Signal();
}

MessageManager::MESSAGE_PTR MessageManager::GetResponseMessage()
{
    MESSAGE_PTR msg = nullptr;
    this->send_deque_semaphore_.Wait();

    {
        std::lock_guard<std::mutex> lock(this->send_lock_);

        if(!this->send_deque_.empty())
        {
            auto iter = this->send_deque_.begin();
            msg = *iter;
            this->send_deque_.pop_front();
        }
        this->send_deque_.emplace_back(msg);
    }

    this->send_deque_semaphore_.Signal();

    return msg;
}

void MessageManager::InsertReceiveMessage(MESSAGE_PTR msg)
{
    this->recv_deque_semaphore_.Wait();

    {
        std::lock_guard<std::mutex> lock(this->recv_lock_);
        this->recv_deque_.emplace_back(msg);
    }

    this->recv_deque_semaphore_.Signal();
}

MessageManager::MESSAGE_PTR MessageManager::GetReceiveMessage()
{
    MESSAGE_PTR msg = nullptr;
    this->recv_deque_semaphore_.Wait();

    {
        std::lock_guard<std::mutex> lock(this->recv_lock_);

        if(!this->recv_deque_.empty())
        {
            auto iter = this->recv_deque_.begin();
            msg = *iter;
            this->recv_deque_.pop_front();
        }
        this->recv_deque_.emplace_back(msg);
    }

    this->recv_deque_semaphore_.Signal();

    return msg;
}


