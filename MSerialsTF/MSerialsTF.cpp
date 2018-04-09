// MSerialsTF.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <string>
#include <vector>
#include "MSerialsTF.h"
#include "tensorflow.h"
#include "Global.h"
#include "../MSerialsCommon/common.h"
//#include "..\MSerialsUI\TensorFlow.h"	//坑爹的vs给我加了这个造成编译不过去

#define	tf					MSerial::TensorFlow::GetIns()

const char * Get_ErrorInfo()
{
	return tf->ErrorInfo();
}

const char* Get_Version()
{
	return "exTensorFlow0.9.0";
}

const char * Get_Vendor()
{
	return "AET_LIMITED_CO";
}

const char * Get_AuthorList()
{
	return tf->Get_AuthroList();
}

int load_image(const int rows, const int cols, const int ch, const unsigned char * data)
{
	//为了图像数据通用，不能采用第三方库
	if (!data)	return -1;
	if (1 == ch)
	{
		cv::Mat img(rows, cols, CV_8UC1);
		memcpy(img.data, data, rows*cols);
		tf->load_image(img);
	}
	else if(3 == ch)
	{
		cv::Mat img(rows, cols, CV_8UC3);
		memcpy(img.data, data, rows*cols*3);
		tf->load_image(img);
	}
	else
	{
		return -2;
	}
	return 0;
}

int load_image_by_path(const char * path)
{
#if 0
	return tf->load_image_by_path(path);
#else
#ifndef OVER_LAP
#define OVER_LAP - 3
#endif
	if (!strlen(path) || strlen(path) > 2047) return PARA_ERROR;
	char cmds[2048] = { 0 };
	memcpy(cmds, path, strlen(path));
	//win_tool wt;
	//const char * image_path = wt.UnicodeToUtf8((CString)(CStringA)(LPCSTR)cmds);
	return tf->load_image_by_path(cmds);
#endif
}

int ReLoadScript()
{
	return tf->ReLoadScript();
}

int VarsFunction(char * command)
{
#if 0
	return tf->VarsFunction(command);
#else
#ifndef OVER_LAP
#define OVER_LAP - 3
#endif
	if (strlen(command) > 2047) return OVER_LAP;
	char cmds[2048] = {0};
	memcpy(cmds, command, strlen(command));
	return tf->VarsFunction(cmds);
#endif
}