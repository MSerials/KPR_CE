#pragma once


#ifndef __HALCON__
#define __HALCON__

#define DLL_EXPORT

#ifdef DLL_EXPORT
#define HCDLL __declspec(dllexport)
#else
#define HCDLL __declspec(dllimport)
#endif
#include <atlstr.h>
#include "halconcpp.h"





//ö�����,��������������൱�ڳ�ʼ��
HCDLL int			 hc_check_inv(const Halcon::Hobject&src, const Halcon::Hobject& ROI, Halcon::HTuple& disp_hand);





#endif
