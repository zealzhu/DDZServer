////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhou du
// Created： 2017/07/10
// Describe：字符串操作工具类
////////////////////////////////////////////////////////////////////////

#if !defined(EA_22D89707_011F_411e_9790_648969014B4A__INCLUDED_)
#define EA_22D89707_011F_411e_9790_648969014B4A__INCLUDED_

#include <string>
#include <vector>

using namespace std;

// 字符串操作工具类
class CStringUtils
{

public:
	CStringUtils();
	virtual ~CStringUtils();

	// 分割字符串
	static vector<string>& Split(const string &str, const string &strDelimiters, vector<string> &vecElems, bool bSkipEmpty = true);

	static   std::string string_To_UTF8(const std::string & str);
	
	static  std::string UTF8_To_string(const std::string & str);
	
	static bool IsValidAccount(const string& strAccount);

	static bool IsValidPassword(const string& strPassword);

	static bool IsValidMail(const ::std::string& strMail);

	static bool IsValidMobile(const string& strMobile);

	static bool IsValidHomeNumber(const string& strHomeNum);
};
#endif //
