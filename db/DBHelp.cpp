/**
 * @file DBHelp.cpp
 * @brief 数据库帮助类
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-12
 */
#include "DBHelp.h"
#include <tools/GameLog.h>
#include "DBConnectionPool.h"
#include "Exception.h"

#include <sstream> // ostringstream
#include <stdio.h> // printf

namespace GameDB
{

DBHelp * DBHelp::instance_ = nullptr;

DBHelp::DBHelp()
{}

DBHelp & DBHelp::Instance()
{
    if(DBHelp::instance_ == nullptr)
        DBHelp::instance_ = new DBHelp;

    return *DBHelp::instance_;
}

void DBHelp::DestoryInstance()
{
    if(DBHelp::instance_ != nullptr)
        delete DBHelp::instance_;

    DBHelp::instance_ = nullptr;
}

DBHelp::~DBHelp()
{
    this->DestoryInstance();
}

void DBHelp::TestConnection()
{
    using namespace sql;
    mysql::MySQL_Driver * driver = mysql::get_mysql_driver_instance();
    Connection * conn = driver->connect("tcp://127.0.0.1:3306", "zpc", "no080740");
    conn->setSchema("mysql");

    Statement * state = conn->createStatement();
    ResultSet * result = state->executeQuery("select count(1) as userNum from user");
    int id = 0;
    while (result->next())
    {
        id = result->getInt("userNum");
        printf("db test: user account Num[%d]", id);
    }

    delete result;
    delete state;
    delete conn;
}

std::string DBHelp::BuildInsertSQL(const std::string & table_name, const RecordData & column_map)
{
    using namespace std;
    ostringstream columns_name; // 列名
    ostringstream columns_values; // 列名对应的数据
    auto it_end = column_map.end();

    for(auto it = column_map.begin(); it != it_end; ++it)
    {
        // 第一条数据前面不要逗号
        if(it == column_map.begin())
        {
            // 列名
            columns_name << it->first;
            // 列名对应值
            if(it->second.first == DBType::kDBStr)
            {
                columns_values << "'" << it->second.second << "'";
            }
            else
            {
                columns_values << it->second.second;
            }
        }
        // 后面的都要加个逗号
        else
        {
            // 列名
            columns_name << "," << it->first;
            // 列名对应值
            if(it->second.first == DBType::kDBStr)
            {
                columns_values << ",'" << it->second.second;
            }
            else
            {
                columns_values << "," << it->second.second;
            }
        }
    }

    ostringstream insert_sql_stream;
    insert_sql_stream << "insert into " << table_name << " (" << columns_name.str() << ") values(" << columns_values.str() << ")";

    return insert_sql_stream.str();
}

size_t DBHelp::InsertRecord(const std::string & table_name, const RecordData & column_map)
{
    std::string insert_sql = this->BuildInsertSQL(table_name, column_map);
    logger_debug("SQL 插入语句：{}", insert_sql);

    // 从连接池中获取一个连接
    ConnectionPtr conn = DBConnectionPool::Instance().GetConnection();
    // 创建一个statement
    StatementPtr state(conn->createStatement());

    int affected_rows = 0;
    try
    {
        affected_rows = state->executeUpdate(sql::SQLString(insert_sql));
    }
    catch(const sql::SQLException & e)
    {
        logger_error("插入失败：{}", e.what());
    }
    // 返回给数据库连接池
    DBConnectionPool::Instance().ReturnConnection(conn);

    return affected_rows;
}

std::string DBHelp::BuildUpdateSQL(const std::string & table_name, const RecordData & column_map, const RecordData & where_map)
{
    using namespace std;
    ostringstream set_values;

    // set
    for(auto it = column_map.begin(); it != column_map.end(); ++it)
    {
        // 第一条数据前面不要逗号
        if(it == column_map.begin())
        {
            // 列名
            set_values << it->first << "=";
            // 列名对应值
            if(it->second.first == DBType::kDBStr)
            {
                set_values << "'" << it->second.second << "'";
            }
            else
            {
                set_values << it->second.second;
            }
        }
        // 后面的都要加个逗号
        else
        {
            // 列名
            set_values << "," << it->first << "=";
            // 列名对应值
            if(it->second.first == DBType::kDBStr)
            {
                set_values << "'" << it->second.second;
            }
            else
            {
                set_values << it->second.second;
            }
        }
    }

    ostringstream update_sql_stream;
    update_sql_stream << "update " << table_name << " set " << set_values.str() << " where " << BuildWhere(where_map);

    return update_sql_stream.str();
}

size_t DBHelp::UpdateRecord(const std::string & table_name, const RecordData & column_map, const RecordData & where_map)
{
    std::string update_sql = this->BuildUpdateSQL(table_name, column_map, where_map);
    logger_debug("SQL 更新语句：{}", update_sql);

    // 从连接池中获取一个连接
    ConnectionPtr conn = DBConnectionPool::Instance().GetConnection();
    // 创建一个statement
    StatementPtr state(conn->createStatement());

    int affected_rows = 0;
    try
    {
        affected_rows = state->executeUpdate(sql::SQLString(update_sql));
    }
    catch(const sql::SQLException & e)
    {
        logger_error("更新失败：{}", e.what());
    }
    // 返回给数据库连接池
    DBConnectionPool::Instance().ReturnConnection(conn);

    //if(affected_rows != 0)
    //{
        //logger_error("更新数据错误，请检查数据的格式");
        //throw Exception(ErrorCode::kUpdateError);
    //}

    return affected_rows;
}

size_t DBHelp::DeleteRecord(const std::string & table_name, const std::string & where)
{
    std::ostringstream delete_sql_stream;
    delete_sql_stream << "delete from " << table_name << " where " << where;
    logger_debug("SQL 删除语句：{}", delete_sql_stream.str());

    // 从连接池中获取一个连接
    ConnectionPtr conn = DBConnectionPool::Instance().GetConnection();
    // 创建一个statement
    StatementPtr state(conn->createStatement());

    int affected_rows = 0;

    try
    {
        affected_rows = state->executeUpdate(sql::SQLString(delete_sql_stream.str()));
    }
    catch(const sql::SQLException & e)
    {
        logger_error("删除失败：{}", e.what());
    }
    // 返回给数据库连接池
    DBConnectionPool::Instance().ReturnConnection(conn);

    return affected_rows;
}

std::string DBHelp::BuildQuerySQL(const std::string & table, const std::string & field, const RecordData & where_map)
{
    std::ostringstream sql;
    sql << "select " << field << " from " << table << " where " << BuildWhere(where_map);

    return sql.str();
}

QueryData DBHelp::QueryRecord(const std::string & table, const std::string & fields, const RecordData & where_map)
{
    std::string query_sql = BuildQuerySQL(table, fields, where_map);
    return QueryRecord(query_sql);
}

QueryData DBHelp::QueryRecord(const std::string & query_sql)
{
    logger_debug("SQL 查询语句：{}", query_sql);
    QueryData query_data;

    // 从连接池中获取一个连接
    ConnectionPtr conn = DBConnectionPool::Instance().GetConnection();
    // 创建一个statement
    StatementPtr state(conn->createStatement());
    sql::ResultSet * result_set = nullptr;
    // 执行查询
    try
    {
        result_set = state->executeQuery(sql::SQLString(query_sql));
        sql::ResultSetMetaData * result_meta_data = result_set->getMetaData();

        // 获取字段名
        std::vector< std::string > field_name_vec;
        int column_count = result_meta_data->getColumnCount();
        for(int i = 0; i < column_count; i++)
        {
            sql::SQLString sql_str = result_meta_data->getColumnLabel(i + 1);
            field_name_vec.emplace_back(sql_str.asStdString());
        }

        std::map<std::string, std::string> rows_map;
        // 一行一行的读取
        while(result_set->next())
        {
            rows_map.clear();
            // 一个一个字段的读取
            for(auto & field_name : field_name_vec)
            {
                rows_map[field_name] = result_set->getString(field_name);
            }
            // 添加到查询数据中
            query_data.emplace_back(rows_map);
        }
        // 释放
        delete result_set;
    }
    catch(const sql::SQLException & e)
    {
        logger_error("查询失败：{}", e.what());
    }

    // 返回给数据库连接池
    DBConnectionPool::Instance().ReturnConnection(conn);

    return query_data;

}

std::string DBHelp::BuildWhere(const RecordData & where_map)
{
    std::ostringstream where;

    auto it_end = where_map.end();
    for(auto it = where_map.begin(); it != it_end; ++it)
    {
        // 第一条数据前面不要逗号
        if(it == where_map.begin())
        {
            // 列名
            where << it->first << "=";
            // 列名对应值
            if(it->second.first == DBType::kDBStr)
            {
                where << "'" << it->second.second << "'";
            }
            else
            {
                where << it->second.second;
            }
        }
        // 后面的都要加个逗号
        else
        {
            // 列名
            where << "," << it->first << "=";
            // 列名对应值
            if(it->second.first == DBType::kDBStr)
            {
                where << "'" << it->second.second;
            }
            else
            {
                where << it->second.second;
            }
        }
    }
    return where.str();
}

} // namespace GameDB
