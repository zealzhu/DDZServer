/**
 * @file DBConnectionPool.cpp
 * @brief 数据库连接池
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-11
 */
#include "DBConnectionPool.h"
#include <tools/GameLog.h>
#include <tools/ConfigManager.h>
#include "Exception.h"

namespace GameDB
{
DBConnectionPool::DBConnectionPool(std::string user_name, std::string password, std::string url, int max_size)
    : user_name_(sql::SQLString(user_name)), password_(sql::SQLString(password)), url_(sql::SQLString(url)),
    max_pool_size_(max_size), current_pool_size_(0), thread_finish_(false), driver_(nullptr)
{
    try
    {
        // 获取driver
        this->driver_ = sql::mysql::get_mysql_driver_instance();
        logger_info("url:{} 用户名;{} 密码:{}", url, user_name, password);
    }
    catch(sql::SQLException & e)
    {
        logger_error("驱动连接出错，{}", e.what());
    }
    catch(std::runtime_error & e)
    {
        logger_error("运行出错，{}", e.what());
    }
    // 初始化数据库连接池，只初始化一半的大小
    this->current_pool_size_ = max_size / 2;
    InitConnectionPool(max_size / 2);

    // 开启检查线程
    this->check_thread_ = std::thread(&DBConnectionPool::ShrinkConnectNum, this);
}

DBConnectionPool::DBConnectionPool(const DBConnectionPool&)
{

}

DBConnectionPool & DBConnectionPool::Instance()
{
    using ConfigMgr = GameTools::ConfigManager;
    static std::string user_name = ConfigMgr::GetConfigParam("db.uesr", "zhu");
    static std::string pwd = ConfigMgr::GetConfigParam("db.password", "no080740");
    static std::string max_size = GameTools::ConfigManager::GetConfigParam("db.maxpoolsize", "10");
    static std::string url = "tcp://" + ConfigMgr::GetConfigParam("db.ip", "120.78.219.164") + ":"
        + ConfigMgr::GetConfigParam("db.port", "3306") + "/" + ConfigMgr::GetConfigParam("db.scheme", "ddz");

    static DBConnectionPool instance = DBConnectionPool(user_name, pwd, url, atoi(max_size.c_str()));

    return instance;
}

DBConnectionPool::~DBConnectionPool()
{
    // 等待线程退出
    this->thread_finish_ = true;
    this->check_thread_.join();
    // 释放所有连接
    this->DestoryConnectionPool();
}

void DBConnectionPool::InitConnectionPool(int size)
{
    std::lock_guard<std::mutex> lck(lock_);
    this->AddConnections(size);
}

void DBConnectionPool::AddConnections(int size)
{
    logger_info("准备新增 {} 数据库连接", size);
    int success = 0;

    for(int i = 0; i < size; i++)
    {
        sql::Connection * conn;
        try
        {
            conn = this->driver_->connect(this->url_, this->user_name_, this->password_);
            ConnectionPtr connPtr(conn);
            this->connection_list_.emplace_back(std::move(connPtr));
            ++success;
        }
        catch(sql::SQLException & e)
        {
            logger_error("驱动连接出错，{}", e.what());
            return;
        }
        catch(std::runtime_error & e)
        {
            logger_error("运行出错, {}", e.what());
            return;
        }
    }

    logger_info("当前可用连接数：{}", this->current_pool_size_);
}

int DBConnectionPool::GetAvailableConnectionSize()
{
    return this->connection_list_.size();
}

void DBConnectionPool::ExpandPool(int size)
{
    std::lock_guard<std::mutex> lck(lock_);
    // 最大数量扩充
    this->max_pool_size_ += size;
    // 当前数量增加
    this->current_pool_size_ += size;
    // 添加连接
    this->AddConnections(size);
    logger_info("新增{}个数据库连接，可用连接数{}，当前连接池大小：{}", size, this->GetAvailableConnectionSize(), this->current_pool_size_);
}

void DBConnectionPool::ReducePool(int size)
{
    std::lock_guard<std::mutex> lck(lock_);

    // 要减少的数量不能大于最大数量
    if(size > this->max_pool_size_)
    {
        return;
    }

    for(int i = 0; i < size; i++)
    {
        this->DestoryOneConnection();
    }
    this->max_pool_size_ -= size;
    this->current_pool_size_ -= size;
    logger_info("减少{}个数据库连接，可用连接数{}，当前连接池大小：{}", size, this->GetAvailableConnectionSize(), this->current_pool_size_);
}

void DBConnectionPool::DestoryConnectionPool()
{
    logger_info("关闭所有连接");
    for(auto & conn : this->connection_list_)
    {
        // 依次转移所有权，智能指针t出作用域自动释放
        ConnectionPtr && t = std::move(this->connection_list_.front());
    }
}

void DBConnectionPool::DestoryOneConnection()
{
    if(this->connection_list_.size() > 0)
    {
        this->connection_list_.pop_front();
        logger_info("关闭一个数据库连接，可用连接数：{}，当前连接池大小：{}", this->GetAvailableConnectionSize(), this->current_pool_size_);
    }
    else
    {
        logger_warn("连接池无连接，无法关闭连接");
    }
}

ConnectionPtr DBConnectionPool::GetConnection()
{
    std::unique_lock<std::mutex> lck(lock_);

    // 如果没有可用连接就新增一个
    if(this->GetAvailableConnectionSize() == 0)
    {
        // 当前连接数不大于最大连接就添加一个连接
        if(this->current_pool_size_ < this->max_pool_size_)
        {
            AddConnections(1);
            ++this->current_pool_size_;
        }
        // 等待可用连接
        else
        {
            // 重试次数
            int retry_count = 0;

            // 重试次数小于10次且没有可用连接就一直重试
            while(this->GetAvailableConnectionSize() == 0 && retry_count < 10)
            {
                ++retry_count;
                logger_info("数据库连接达到最大数量，等待可用数据库连接");
                // 释放锁
                lck.unlock();
                // 等待
                std::this_thread::sleep_for(std::chrono::milliseconds(50 * retry_count));
                // 重新上锁
                lck.lock();
            }

            if(retry_count >= 10)
            {
                logger_warn("等待可用数据库连接10次，放弃等待");
                throw Exception(ErrorCode::kNoAvailiableConnection);
            }
        }
    }

    // 取第一个
    ConnectionPtr conn = this->connection_list_.front();
    this->connection_list_.pop_front();
    logger_info("获得一个数据库连接，可用连接数：{}，当前连接池大小：{}", this->GetAvailableConnectionSize(), this->current_pool_size_);

    return conn;
}

void DBConnectionPool::ReturnConnection(ConnectionPtr & conn)
{
    std::lock_guard<std::mutex> lck(this->lock_);

    this->connection_list_.emplace_back(conn);
    logger_info("归还一个数据库连接，可用连接数：{}，当前连接池大小：{}", this->GetAvailableConnectionSize(), this->current_pool_size_);
}

void DBConnectionPool::ShrinkConnectNum()
{
    using namespace std::chrono;
    logger_debug("启动数据库连接检查线程");

    // 最后一次可用连接数
    int last_available = this->GetAvailableConnectionSize();
    int cur_available = last_available;

    // 检查间隔
    int check_interval = atoi(GameTools::ConfigManager::GetConfigParam("db.checkinterval", "10").c_str());
    // 检查时间
    time_point<system_clock, seconds> pre_time = time_point_cast<seconds>(system_clock::now());
    // 当前时间
    time_point<system_clock, seconds> cur_time;
    // 时间差
    duration<int> time_span;

    while(!thread_finish_)
    {
        cur_time = time_point_cast<seconds>(system_clock::now());
        time_span = duration_cast< duration< int > >(cur_time - pre_time);
        // 计算时间差是否大于时间间隔
        if(time_span.count() >= check_interval)
        {
            std::lock_guard<std::mutex> lck(this->lock_);
            cur_available = this->GetAvailableConnectionSize();
            logger_trace("前一次可用连接：{} 当前可用连接：{}", last_available, cur_available);

            // 当可用连接大于连接池一半时且上一次与当前可用连接一样时减少一个连接
            if(cur_available > this->max_pool_size_ / 2 && last_available == cur_available)
            {
                this->DestoryOneConnection();
                --this->current_pool_size_;
            }
            last_available = this->GetAvailableConnectionSize();
            // 更新之前的时间
            pre_time = time_point_cast<seconds>(system_clock::now());
        }
        // 如果使用sleep等待的话主线程如果退出还要等到唤醒才结束
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000 * check_interval));
        std::this_thread::sleep_for(milliseconds(1000));
    }
    logger_debug("退出数据库连接检查线程");
}

}

