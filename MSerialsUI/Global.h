#pragma once

#include "stdafx.h"
#include "TensorFlow.h"
#include "MSerialsCV.h"
#include "../MSerialsCommon/SelDir.h"
#include "../MSerialsCommon/common.h"

//显示历史记录的指针
#include "HistoryDlg.h"
//显示统计图对话框的指针
#include "StaticsShowDlg.h"

//csv文件存储
#include "CTxt.h"

#include "opencv.hpp"
#ifdef _WIN64
#pragma comment(lib,"../third_party/opencv/x64/opencv_world320.lib")
#endif

#define CAMERA1 (1<<0)
#define CAMERA2 (1<<1)

class global
{
private:
	global() {
	camera_error = 0;
	camera_error |= CAMERA1;
	camera_error |= CAMERA2;

	system = 0;
	system |= NOHALCONPARA;
	system |= NOOPENCVPARA;
	//system |= NOCOM;
	SoftWareTick = 0;


	for(int i =0;i<512;i++) m_cesus_data[i] = 0;
	m_dlg_history = nullptr;;
	//显示
	m_dlg_statistic = nullptr;
	txt = nullptr;

	};

public:
	static global * GetIns()
	{
		static global l_g;
		return &l_g;
	}
	int system;
	int camera_error;
	DWORD SoftWareTick;

	//存了一个图表数据，我把要求的正太分布图数据化为512个等分的，数值范围为24-26
	int m_cesus_data[512];
	//获取历史记录
	CHistoryDlg	*m_dlg_history;
	//显示
	CStaticsShowDlg	*m_dlg_statistic;
	//记录历史记录
	CTxt *txt;
};