/**
 * @file UserDao.cpp
 * @brief 用户数据帮助类
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-15
 */
#include "UserDao.h"
#include <module/Exception.h>
#include <db/DBHelp.h>
#include <tools/StringUtils.h>
#include <tools/GameLog.h>

using namespace GameDB;
using namespace StringUtils;

namespace GameModule
{

bool UserDao::IsExist(int id)
{
    RecordData param;
    QueryData query_data;
    param["id"] = std::make_pair<DBType, std::string>(DBType::kDBInt, IntToString(id));
    query_data = DBHelp::Instance().QueryRecord("t_player", "account", param);
    if (query_data.size() > 0) {
        logger_debug("查询结果: account {} id: {}", query_data[0]["account"], id);
        return true;
    }

    return false;
}

bool UserDao::IsExist(const char * account)
{
    RecordData param;
    QueryData query_data;
    param["account"] = std::make_pair<DBType, std::string>(DBType::kDBStr, account);
    query_data = DBHelp::Instance().QueryRecord("t_player", "id", param);
    if (query_data.size() > 0) {
        logger_debug("查询结果: account {} id: {}", account, query_data[0]["id"]);
        return true;
    }

    return false;
}

PlayerPtr UserDao::GetPlayer(const char * account, const char * password)
{
    RecordData param;
    QueryData query_data;
    PlayerPtr player_ptr;

    param["account"] = std::make_pair<DBType, std::string>(DBType::kDBStr, account);
    param["password"] = std::make_pair<DBType, std::string>(DBType::kDBStr, password);
    query_data = DBHelp::Instance().QueryRecord("t_player", "*", param);
    if (query_data.size() > 0) {
        auto row = query_data[0];
        player_ptr->set_id(atoi(row["id"].c_str()));
        player_ptr->set_account(row["account"].c_str());
        player_ptr->set_password(row["password"].c_str());
        player_ptr->set_name(row["name"].c_str());
        zhu::user::Player_SexType sex =
            row["sex"] == "MALE" ? zhu::user::Player_SexType_MALE : zhu::user::Player_SexType_FEMALE;
        player_ptr->set_sex(sex);
        player_ptr->set_email(row["email"].c_str());
        std::string mobile = row["mobile"];
        if (mobile.size() > 0)
        {
            zhu::user::Player_PhoneNumber* phones = player_ptr->add_phones();
            phones->set_type(::zhu::user::Player_PhoneType_MOBILE);
            phones->set_number(mobile.c_str());
        }
        sql::SQLString strPhone(row["phone"]);
        if (strPhone->length() > 0)
        {
            zhu::user::Player_PhoneNumber* phones = player_ptr->add_phones();
            phones->set_type(::zhu::user::Player_PhoneType_HOME);
            phones->set_number(strPhone.c_str());
        }
        player_ptr->set_money(atoi(row["money"].c_str()));
        zhu::user::UserStatus status = StatusFromString(row["status"].c_str());
        player_ptr->set_status(status);
        player_ptr->set_desc(row["desc"].c_str());
    }

    return player_ptr;
}

bool UserDao::AddNewAccount(PlayerPtr register_req)
{
    RecordData param;
    param["account"] = std::make_pair<DBType, std::string>(DBType::kDBStr, register_req->account().c_str());
    param["password"] = std::make_pair<DBType, std::string>(DBType::kDBStr, register_req->password().c_str());
    param["name"] = std::make_pair<DBType, std::string>(DBType::kDBStr, register_req->name().c_str());
    param["email"] = std::make_pair<DBType, std::string>(DBType::kDBStr, register_req->email().c_str());
    int result = DBHelp::Instance().InsertRecord("t_player", param);

    return result > 0 ? true : false;
}

bool UserDao::ChangeAccountStatus(const char * account, zhu::user::UserStatus status)
{
   /* sql::SQLString strSql("update t_player set status=? where account=?");*/
    RecordData set_map;
    set_map["status"] = std::make_pair<DBType, std::string>(DBType::kDBStr, StatusToString(status));

    RecordData where_map;
    where_map["account"] = std::make_pair<DBType, std::string>(DBType::kDBStr, account);

    int result = DBHelp::Instance().UpdateRecord("t_player", set_map, where_map);
    return result > 0 ? true : false;
}

std::string UserDao::StatusToString(zhu::user::UserStatus status)
{
    const static std::map< zhu::user::UserStatus, std::string > status_map =
    { { zhu::user::UserStatus::OFFLINE, "OFFLINE"},
      { zhu::user::UserStatus::ONLINE, "ONLINE"},
      { zhu::user::UserStatus::FORBIDDEN, "FORBIDDEN"} };
    std::map< zhu::user::UserStatus, std::string >::const_iterator it = status_map.find(status);
    if (status_map.end() != it)
    {
        return it->second;
    }
    logger_error("用户状态 {} 转换为 string 失败", status);
    return "STATUS NOT FOUND";
}

zhu::user::UserStatus UserDao::StatusFromString(std::string status)
{
    if (status == "OFFLINE")
    {
        return zhu::user::UserStatus::OFFLINE;
    }
    else if (status == "ONLINE")
    {
        return zhu::user::UserStatus::ONLINE;
    }
    else if (status == "FORBIDDEN")
    {
        return zhu::user::UserStatus::FORBIDDEN;
    }
    else
    {
        logger_error("无法转换用户状态 {}", status);
        throw Exception("无法转换用户状态 status " + status);
    }
}

std::string UserDao::SexToString(zhu::user::Player_SexType sex)
{
    const static std::map< zhu::user::Player_SexType, std::string > sex_map =
    { { zhu::user::Player_SexType_MALE, "MALE" },
    { zhu::user::Player_SexType_FEMALE, "FEMALE" } };
    std::map< zhu::user::Player_SexType, std::string >::const_iterator it = sex_map.find(sex);
    if (sex_map.end() != it)
    {
        return it->second;
    }
    logger_error("性别 {} 转换为 string 失败", sex);
    return "SEX NOT FOUND";
}

}
