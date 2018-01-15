/**
 * @file DBConnectionPool.h
 * @brief 数据库连接池
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-11
 */
#ifndef _DB_CONNECTION_POOL_H
#define _DB_CONNECTION_POOL_H

// mysql driver
#include <mysql_driver.h>
#include <mysql_connection.h>

#include <mutex>
#include <thread>
#include <list>

namespace GameDB
{
// 所有Connection都需要手动释放，因此使用智能指针进行管理
typedef std::shared_ptr<sql::Connection> ConnectionPtr;

class DBConnectionPool
{
    typedef std::list<ConnectionPtr> ConnectionList;
public:
    /**
     * @brief 获取实例
     *
     * @return
     */
    static DBConnectionPool & Instance();

    /**
     * @brief 获得一个连接
     *
     * @return
     */
    ConnectionPtr GetConnection();

    /**
     * @brief 将连接返回连接池中
     *
     * @param conn
     */
    void ReturnConnection(ConnectionPtr & conn);

    /**
     * @brief 获取池中可用连接数
     *
     * @return
     */
    int GetAvailableConnectionSize();

    ~DBConnectionPool();

private:

    DBConnectionPool(std::string user_name, std::string password, std::string url, int max_size);

    DBConnectionPool(const DBConnectionPool &);

    DBConnectionPool & operator=(const DBConnectionPool &) = default;

    void InitConnectionPool(int size);

    void DestoryConnectionPool();

    void ExpandPool(int size);

    void ReducePool(int size);

    void AddConnections(int size);

    void DestoryOneConnection();

    void ShrinkConnectNum();

    // 数据库连接驱动
    sql::mysql::MySQL_Driver * driver_;

    // 数据库连接列表
    ConnectionList connection_list_;

    // 连接池最大数量
    int max_pool_size_;
    int current_pool_size_;

    sql::SQLString user_name_;
    sql::SQLString password_;
    sql::SQLString url_;

    std::mutex lock_;
    bool thread_finish_;
    std::thread check_thread_;
};

} // namespace GameDB

#endif // _DB_CONNECTION_POOL_H
