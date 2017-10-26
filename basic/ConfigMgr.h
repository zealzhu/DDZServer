////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhu
// Created： 2017/10/25
// Describe：配置管理器
////////////////////////////////////////////////////////////////////////
#ifndef _CONFIG_MGR_H
#define _CONFIG_MGR_H

#include "FileUtil.h"

namespace basic
{
	class CConfigMgr
	{
	public:
		// 获取配置参数
		static std::string getConfigParam(const std::string & strKey, const std::string & strDefaultValue) {
			map<std::string, std::string>::iterator itResult = m_mapConfig.find(strKey);

			if (m_mapConfig.end() != itResult)
				return itResult->second;
			else
				return strDefaultValue;
		}

	private:
		// 配置表
		static map<std::string, std::string> m_mapConfig;
	};
}


#endif

