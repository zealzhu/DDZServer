////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhou du
// Created： 2017/07/10
// Describe：字符串操作工具类
////////////////////////////////////////////////////////////////////////

#include "StringUtils.h"
#include <regex>
#include <afx.h>

//// 名字正则表达式
//static regex NAME_PATTERN("^[a-z0-9_-]{6,18}$");
//
//// ip地址正则表达式
//static regex IP_PATTERN("^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?).){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
//
//// 中文字符正则表达式
//static regex CH_PATTERN("[\u4e00-\u9fa5]");

CStringUtils::CStringUtils(){

}

CStringUtils::~CStringUtils(){

}

vector<string>& CStringUtils::Split(const string &str, const string &strDelimiters, vector<string> &vecElems, bool bSkipEmpty){

	string::size_type iPos, iPrev = 0;
	while ((iPos = str.find_first_of(strDelimiters, iPrev)) != string::npos) {
		if (iPos > iPrev) {
			vecElems.emplace_back(str, iPrev, iPos - iPrev);
		}
		iPrev = iPos + 1;
	}
	if (iPrev < str.size()) vecElems.emplace_back(str, iPrev, str.size() - iPrev);
	return vecElems;
}

// 字符串转UTF8
std::string CStringUtils::string_To_UTF8(const std::string & str) 
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴  
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

// UTF8转字符串
std::string CStringUtils::UTF8_To_string(const std::string & str)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴  
	memset(pwBuf, 0, nwLen * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr = pBuf;

	delete[]pBuf;
	delete[]pwBuf;

	pBuf = NULL;
	pwBuf = NULL;

	return retStr;
}

bool CStringUtils::IsValidAccount(const string& strAccount)
{
	static regex ACCOUNT_PATTERN("^[a-z0-9_-]{3,16}$");
	return regex_match(strAccount, ACCOUNT_PATTERN);
}

bool CStringUtils::IsValidPassword(const string& strPassword)
{
	static regex PASSWORD_PATTERN("^[a-z0-9_-]{6,18}$");
	return regex_match(strPassword, PASSWORD_PATTERN);
}

bool CStringUtils::IsValidMail(const ::std::string& strMail)
{
	static regex MAIL_PATTERN("([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,3}(\\.[a-z]{2})?)");
	return regex_match(strMail, MAIL_PATTERN);
}

bool CStringUtils::IsValidMobile(const string& strMobile)
{
	static regex MOBILE_PATTERN("^1[34578]\\d{9}$");
	return regex_match(strMobile, MOBILE_PATTERN);
}

bool CStringUtils::IsValidHomeNumber(const string& strHomeNum)
{
	// 座机号正则表达式 "^(\(\d{3,4}\)|\d{3,4}-|\s)?\d{7,14}$"
	static regex PHONE_PATTERN("^(\\(\\d{3,4}\\)|\\d{3,4}-|\\s)?\\d{7,14}$");
	return regex_match(strHomeNum, PHONE_PATTERN);
}
