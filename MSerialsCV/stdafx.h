// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>


#ifndef __OPENCV__INCLUDE__
#define __OPENCV__INCLUDE__
#ifdef _WIN64
#pragma comment(lib,"../third_party/opencv/x64/opencv_world320.lib")
#else
#error can not support x86 system
#endif
#endif
// TODO: 在此处引用程序需要的其他头文件
