/**
 * @file DBHelp.h
 * @brief 数据库帮助类
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-12
 */
#ifndef _DB_HELP_H
#define _DB_HELP_H

// Statement
#include <cppconn/statement.h>

#include <memory>
#include <map>
#include <vector>
#include <string>

namespace GameDB
{
    // 数据类型
enum DBType
{
    kDBInt,
    kDBStr,
    kDBFloat,
};

// 记录数据 列名 <-> { 数据类型, 数据 }
typedef std::map< std::string, std::pair< DBType, std::string > > RecordData;
// 查询返回的数据 可能不止一条数据因此用vector 每条记录都是一个map集合 列名 <-> 数据
typedef std::vector< std::map< std::string, std::string > > QueryData;

class DBHelp
{
    typedef std::shared_ptr< sql::Statement > StatementPtr;
public:
    /**
     * @brief 测试数据库连接
     */
    void TestConnection();

    static DBHelp & Instance();

    static void DestoryInstance();

    /**
     * @brief 构建sql插入语句
     *
     * @param table_name 插入表名
     * @param column_map 数据map
     *
     * @return
     */
    std::string BuildInsertSQL(const std::string & table_name, const RecordData & column_map);

    /**
     * @brief 插入记录
     *
     * @param table_name 表名
     * @param column_map 数据map
     *
     * @return
     */
    size_t InsertRecord(const std::string & table_name, const RecordData & column_map);

    /**
     * @brief 构建sql更新语句
     *
     * @param table_name 表名
     * @param column_map 数据map
     * @param where 条件
     *
     * @return 返回sql语句
     */
    std::string BuildUpdateSQL(const std::string & table_name, const RecordData & column_map, const RecordData & where_map);

    /**
     * @brief 更新记录
     *
     * @param table_name 表名
     * @param column_map 数据map
     * @param where 条件
     *
     * @return 受影响行数
     */
    size_t UpdateRecord(const std::string & table_name, const RecordData & column_map, const RecordData & where_map);

    /**
     * @brief 删除记录
     *
     * @param table_name 表名
     * @param where 条件
     *
     * @return 受影响函数
     */
    size_t DeleteRecord(const std::string & table_name, const std::string & where);

    std::string BuildQuerySQL(const std::string & table, const std::string & field, const RecordData & where_map);

    /**
     * @brief 查询数据
     *
     * @param query_sql 查询sql语句
     *
     * @return 返回查询数据
     */
    QueryData QueryRecord(const std::string & table, const std::string & field, const RecordData & where_map);

    QueryData QueryRecord(const std::string & query_sql);
private:
    DBHelp();
    ~DBHelp();

    std::string BuildWhere(const RecordData & where_map);

    static DBHelp * instance_;
}; // DBHelp

} // namespace GameDB

#endif // _DB_HELP_H
