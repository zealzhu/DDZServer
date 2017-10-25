////////////////////////////////////////////////////////////////////////
// Copyright(c) 1999-2099, TQ Digital Entertainment, All Rights Reserved
// Author：  zhu
// Created： 2017/07/10
// Describe：
////////////////////////////////////////////////////////////////////////

#ifndef BASIC_H
#define BASIC_H

//可以定位到发生内存泄露 所在的文件和具体那一行，用于检测 malloc 分配的内存  
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG //重载new运算符，把分配内存的信息保存下来，可以定位到那一行发生了内存泄露。用于检测 new 分配的内存
#define NEW  new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define NEW  new
#endif

#include "Exception.h"
#include "GameLog.h"

#define SAFE_DELETE(p)  if (p) { delete p; p = NULL;}
#define SAFE_DELETE_ARRAY(p)  if (p) { delete [] p; p = NULL;}

#endif