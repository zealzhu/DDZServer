/**
 * @file MessageManager.h
 * @brief 消息管理器
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-09
 */
#ifndef _MESSAGE_MANAGER_H
#define _MESSAGE_MANAGER_H

#include "Semaphore.h"
#include <google/protobuf/message.h>
#include <deque>

const unsigned int MAX_MSG_SIZE = 90000;

class MessageManager
{
    typedef std::shared_ptr<google::protobuf::Message> MESSAGE_PTR;
    typedef std::deque<MESSAGE_PTR> MESSAGE_DEQUE;
    typedef std::deque<MESSAGE_PTR>::iterator MESSAGE_ITER;

public:

    static MessageManager & Instance()
    {
        static MessageManager manager;
        return manager;
    }

    void Init();

    bool IsReceiveDequeEmpty();

    bool IsResponseDequeEmpty();

    void InsertResponseMessage(MESSAGE_PTR msg);

    MESSAGE_PTR GetResponseMessage();

    void InsertReceiveMessage(MESSAGE_PTR msg);

    MESSAGE_PTR GetReceiveMessage();

private:
    MessageManager() = default;
    ~MessageManager() = default;
    MessageManager(const MessageManager &) = delete;
    MessageManager & operator=(const MessageManager &) = default;

    MESSAGE_DEQUE recv_deque_;
    MESSAGE_DEQUE send_deque_;

    std::mutex recv_lock_;
    std::mutex send_lock_;

    ThreadLib::Semaphore recv_deque_semaphore_;
    ThreadLib::Semaphore send_deque_semaphore_;
};

#endif // _MESSAGE_MANAGER_H

