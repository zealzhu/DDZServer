/**
 * @file ConfigManager.cpp
 * @brief
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-04
 */
#include "ConfigManager.h"

#include "FileUtils.h"

namespace GameTools
{

std::map<std::string, std::string> ConfigManager::config_map_ = FileUtils::GetConfigMap("config/config.ini");

std::string ConfigManager::GetConfigParam(const std::string & key, const std::string & default_value)
{
    auto it = ConfigManager::config_map_.find(key);

    if(it != ConfigManager::config_map_.end())
    {
        return it->second;
    }
    else
    {
        return default_value;
    }
}

}
