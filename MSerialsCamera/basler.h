#pragma once
#include "Camera.h"
#include "atlstr.h"
#include <iostream>
#include <mutex>


/**
#include "PylonAviCompressionOptions.h"
#include "PylonC.h"
#include "PylonCDefines.h"
#include "PylonCEnums.h"
#include "PylonCError.h"
#include "PylonCModule.h"

*/

#include <PylonC.h>
#include <GenApiCError.h>

#ifdef _WIN64
#pragma comment(lib,"../third_party/basler/lib/x64/PylonC_MD_VC120.lib")
#else
#error not support win32 in basler camera
#pragma comment(lib,"../third_party/daheng/lib/x86/DxImageProc.lib")
#pragma comment(lib,"../third_party/daheng/lib/x86/GxIAPI.lib")
#endif


#ifndef NUM_DEVICES
#define NUM_DEVICES MAX_DEVICE_MUN
#endif

#ifndef NUM_BUFFERS
#define NUM_BUFFERS 5
#endif 

class Basler:public Camera
{
private:
	int Method;
	int error = 0;
	int camera_num = 0;
	//数据
	GENAPIC_RESULT              res;                      /* Return value of pylon methods. */
	size_t                      numDevicesAvail;          /* Number of available devices. */
	_Bool                        isAvail;                  /* Used for checking feature availability. */
	int                         deviceIndex;              /* Index of device used in following variables. */
	PYLON_WAITOBJECTS_HANDLE    wos;                      /* Wait objects. */
#ifdef GENAPIC_WIN_BUILD
	HANDLE                      hTimer;                   /* Grab timer. */
#else
	int                         fdTimer;                  /* Grab timer. */
#endif
	PYLON_WAITOBJECT_HANDLE     woTimer;                  /* Timer wait object. */

														  /* These are camera specific variables */
	PYLON_DEVICE_HANDLE         hDev[NUM_DEVICES];        /* Handle for the pylon device. */
	PYLON_STREAMGRABBER_HANDLE  hGrabber[NUM_DEVICES];    /* Handle for the pylon stream grabber. */
	unsigned char              *buffers[NUM_DEVICES][NUM_BUFFERS];    /* Buffers used for grabbing. */
	PYLON_STREAMBUFFER_HANDLE   bufHandles[NUM_DEVICES][NUM_BUFFERS]; /* Handles for the buffers. */
	int32_t						payloadSize[NUM_DEVICES];
	unsigned char              *imgBuff[NUM_DEVICES];
public:
	Basler(int method = 0) { Method = method; }
	virtual ~Basler();
	int Init();
	int InitCamera(PYLON_DEVICE_HANDLE &hDev, char * DevName, unsigned char** imgBuf, int index);
	char id[256];
	// { std::cout << "freeing basler camera" << std::endl;  close();  }//To CLose camera};
	int refresh_list();
	void _close();

	int error_info();
	
	const char * vendor();

	const char * sdk_version();

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
