#include "stdafx.h"
#include "ImageCard.h"
#include "Daheng.h"
#include "basler.h"
#include "../MSerialsCommon/Tools.h"

//为了使跨编译器能使用，采用了C风格方式写出接口

#pragma warning(disable:4996)

CImageCard::CImageCard()
	:m_camera_index(-1)
	,error_code(0)
{

}


CImageCard::~CImageCard()
{
	FreeMemory();
}

void CImageCard::FreeMemory()
{
	for (std::vector<Camera*>::iterator iter = m_camera.begin(); iter != m_camera.end(); iter++)
		delete *iter;
	m_camera.clear();
}


const char * CImageCard::get_vendor(int manufactory)
{
	static char charbuf[128] = { 0 };
	if (m_camera.size() <= manufactory)
	{
		sprintf(charbuf, "only %d devices \r\n\0", static_cast<int>(m_camera.size()));
		return charbuf;
	}
	return m_camera[manufactory]->vendor();
}

const char * CImageCard::get_sdk_ver_by_vendor(int _vendor)
{
	try
	{
		return  m_camera.at(_vendor)->sdk_version();
		char ver[_MAX_PATH] = { 0 };
#ifdef _MSC_VER
		sprintf_s(ver, "%s", m_camera.at(_vendor)->sdk_version());
#else
		sprintf(ver, "%s", m_camera.at(_vendor)->sdk_version());
#endif
		return ver;
	}
	catch (std::out_of_range e)
	{
		const char *res = "error vendor index in get sdk ver function\n";
		printf(res);
		return res;
	}
}




int CImageCard::Snap(int & width, int & height, unsigned char **data, int &ch, int manufactory, int camera_index, int delay)
{
	try {
	return m_camera.at(manufactory)->Snap(width, height, data, ch, camera_index, delay);
	}
	catch (std::out_of_range e)
	{
		printf("manufactory(vendor) index out of range\n");
	}
	return 1;
}

void CImageCard::Close()
{
	FreeMemory();
}

int CImageCard::RefreshList(int method)
{
	FreeMemory();
	int res = 0;
	//先加入大恒相机
	Daheng *daheng = new Daheng();
	daheng->refresh_list();
	if (0 != daheng->device_quantity())
	{
		Camera *a = daheng;
		m_camera.push_back(a);
	}
	else
	{
		std::cout << "no daheng camera found" << std::endl;
		delete daheng;
	}

	//加入巴斯勒相机
	Basler *basler = new Basler(method);
	res = basler->refresh_list();
	if (0 == res) printf("basler camera init success!\n");
	if (0 != basler->device_quantity())
	{
		Camera *a = basler;
		m_camera.push_back(a);
	}
	else
	{
		printf("basler init error code is %d\n", res);
		delete basler;
	}

	int number = 0;
	for (size_t i = 0; i < m_camera.size(); i++)
	{
		number += m_camera[i]->device_quantity();
	}
	return number;
}

int CImageCard::get_vendors_num()
{
	return static_cast<int>(m_camera.size());
}

int CImageCard::get_cameras_num_by_vendor(int _vendor)
{
	try
	{
		return m_camera.at(_vendor)->device_quantity();
	}catch(std::out_of_range e)
	{
		printf("error in _vendor param %s\n", e.what());
		return 0;
	}
	if(_vendor < 0 || static_cast<int>(m_camera.size() < 1 || _vendor > static_cast<int>(m_camera.size())))
		return -1;
	return m_camera[_vendor]->device_quantity();
}

const char * CImageCard::get_vendor_name(int manufactory)
{
	return m_camera[manufactory]->vendor();
}

const char * CImageCard::get_camera_info_by_index(int manufactory, int camera_idx)
{
	win_tool wt;
	try {
		std::vector<CString> v = wt.SplitCString(m_camera.at(manufactory)->CamerasInfo(), L",");
		return (char*)(LPCSTR)(CStringA)v.at(camera_idx);
	}
	catch (std::out_of_range e)
	{
		return "querey camera information error, error in param index\n\0";
	}
}