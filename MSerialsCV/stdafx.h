// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>


#ifndef __OPENCV__INCLUDE__
#define __OPENCV__INCLUDE__
#ifdef _WIN64
#pragma comment(lib,"../third_party/opencv/x64/opencv_world320.lib")
#else
#error can not support x86 system
#endif
#endif
// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
