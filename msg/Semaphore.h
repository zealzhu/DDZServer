/**
 * @file Semaphore.h
 * @brief 使用C++11新特性实现的信号量
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-09
 */
#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include <mutex>
#include <condition_variable>

namespace ThreadLib
{

class Semaphore
{
public:
    Semaphore(int count = 1);

    inline void SetCount(int count)
    {
        this->count_ = count;
    }

    /**
     * @brief 信号量--
     */
    void Wait();

    /**
     * @brief 信号量++
     */
    void Signal();
private:
    int count_;
    int wakeups_;
    std::mutex mutex_;                      // 锁，保证原子操作
    std::condition_variable condition_;
};

}

#endif // _SEMAPHORE_H

