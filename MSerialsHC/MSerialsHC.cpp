// MSerialsHC.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "MSerialsHC.h"

#ifdef _WIN64
#pragma comment(lib,"../third_party/lib/halcon/x64/halconcpp.lib")
#else
#pragma comment(lib,"../third_party/lib/halcon/x86/halconcpp.lib")
#endif


int hc_check_inv(const Halcon::Hobject & src, const Halcon::Hobject & ROI, Halcon::HTuple & disp_hand)
{
	return 0;
}
