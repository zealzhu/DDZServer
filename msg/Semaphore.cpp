/**
 * @file Semaphore.cpp
 * @brief
 * @author zhu peng cheng
 * @version 1.9
 * @date 2018-01-09
 */
#include "Semaphore.h"

namespace ThreadLib
{

Semaphore::Semaphore(int count)
    : count_(count), wakeups_(0)
{}

void Semaphore::Wait()
{
    std::unique_lock<std::mutex> lck(this->mutex_);
    if(--this->count_ < 0)
    {
        condition_.wait(lck, [&](){ return this->wakeups_ > 0; });
        --this->wakeups_;
    }
}

void Semaphore::Signal()
{
    std::lock_guard<std::mutex> lck(this->mutex_);

    if(++this->count_ <= 0)
    {
        ++this->wakeups_;
        this->condition_.notify_one();
    }
}

}
