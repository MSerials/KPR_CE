#pragma once

#include "stdafx.h"
#include "TensorFlow.h"
#include "MSerialsCV.h"
#include "../MSerialsCommon/SelDir.h"
#include "../MSerialsCommon/common.h"

//��ʾ��ʷ��¼��ָ��
#include "HistoryDlg.h"
//��ʾͳ��ͼ�Ի����ָ��
#include "StaticsShowDlg.h"

//csv�ļ��洢
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
	//��ʾ
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

	//����һ��ͼ�����ݣ��Ұ�Ҫ�����̫�ֲ�ͼ���ݻ�Ϊ512���ȷֵģ���ֵ��ΧΪ24-26
	int m_cesus_data[512];
	//��ȡ��ʷ��¼
	CHistoryDlg	*m_dlg_history;
	//��ʾ
	CStaticsShowDlg	*m_dlg_statistic;
	//��¼��ʷ��¼
	CTxt *txt;
};