#pragma once
#include "Camera.h"
#include "atlstr.h"
#include <iostream>
#include <mutex>

/**
#include "../third_party/daheng/inc/DxImageProc.h"
#include "../third_party/daheng/inc/GxIAPI.h"

#ifdef _WIN64
#pragma comment(lib,"../third_party/daheng/lib/x64/DxImageProc.lib")
#pragma comment(lib,"../third_party/daheng/lib/x64/GxIAPI.lib")
#else
#pragma comment(lib,"../third_party/daheng/lib/x86/DxImageProc.lib")
#pragma comment(lib,"../third_party/daheng/lib/x86/GxIAPI.lib")
#endif
*/

class Baumer :public Camera
{
private:
	int Method;
	int error = 0;
public:
	Baumer(int method = 0) { Method = method; }
	virtual ~Baumer() { std::cout << "free baumer camera" << std::endl; }//To CLose camera};

	int error_info();

	const char * vendor();

	const char * CamerasInfo();

	int device_quantity();
	//
	int Snap(int &width, int &height, unsigned char **data, int &ch, int camera_index = 0, int delay = 35);

	void set_exposure(double exposure = 2000.0, int camera_index = 0);

	//以下函数非所有相机公有
#if 0
	GX_STATUS SetPixelFormat8bit(GX_DEV_HANDLE hDevice);

	void StartAcquisition(int OperateID);

	void OpenDevice(int OperateID = 0);

	//virtual void Grap
	int refresh_list();

	CString Get_Error_Info();

	CString Get_Camera_Info();
#endif
};
