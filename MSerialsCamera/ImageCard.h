#pragma once
#include <atlstr.h>
#include <string>
#include <vector>
#include "Camera.h"

class CImageCard
{
private:
	CImageCard();
	virtual ~CImageCard();
	void FreeMemory();

public:

	//单例模式，仅让相机这个驱动驻留在特定内存中
	static CImageCard* GetIns() { static CImageCard l_ic; return &l_ic;}

	//根据厂商序列获得相机序列号
	const char * get_vendor(int manufactory);

	const char * get_sdk_ver_by_vendor(int _vendor);

	//关键，拍摄相机，获得图像宽，长，图像数据指针， 图像通道数量，输入参数为厂商，相机编号，拍摄延迟
	int Snap(int & width, int & height, unsigned char **data, int & ch, int manufactory, int camera_index, int delay);

	void Close();

	//刷新相机
	int RefreshList(int method = 0);

	int get_vendors_num();

	int get_cameras_num_by_vendor(int _vendor);

	//获取供应商名称
	const char * get_vendor_name(int manufactory);

	const char * get_camera_info_by_index(int manufactory, int camera_idx);

private:
	int error_code;
	int m_camera_index;
	//相机列表
	std::vector<Camera*> m_camera;
};

