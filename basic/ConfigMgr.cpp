#include "ConfigMgr.h"

// ªÒ»°≈‰÷√±Ì
std::map<std::string, std::string> basic::CConfigMgr::m_mapConfig = basic::CFileUtil::getConfigMap("./config/config.ini");