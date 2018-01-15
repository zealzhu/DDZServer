/**
 * @file StringUtils.cpp
 * @brief string帮助类
 * @author zhu peng cheng
 * @version 1.0
 * @date 2018-01-15
 */
#include "StringUtils.h"

using namespace std;

namespace StringUtils
{

string IntToString(int value)
{
    ostringstream os;
    os << value;
    return os.str();
}

}
