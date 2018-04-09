#pragma once
#include "Camera.h"
#include "atlstr.h"
#include <mutex>

#include "../third_party/daheng/inc/DxImageProc.h"
#include "../third_party/daheng/inc/GxIAPI.h"

#ifdef _WIN64
#pragma comment(lib,"../third_party/daheng/lib/x64/DxImageProc.lib")
#pragma comment(lib,"../third_party/daheng/lib/x64/GxIAPI.lib")
#else
#pragma comment(lib,"../third_party/daheng/lib/x86/DxImageProc.lib")
#pragma comment(lib,"../third_party/daheng/lib/x86/GxIAPI.lib")
#endif

class Daheng:public Camera
{
//#define MAX_DEVICE_MUN 6

private:
	int Method;
public:
	Daheng(int method) { Method = method; };
	Daheng();
	virtual ~Daheng();

	int error_info();

	const char * vendor();

	const char * sdk_version(); 

	const char * CamerasInfo();

	int device_quantity();
	//
	int Snap(int &width, int &height, unsigned char **data, int &ch, int camera_index = 0, int delay = 35); 

	void set_exposure(double exposure = 2000.0, int camera_index = 0);

	GX_STATUS SetPixelFormat8bit(GX_DEV_HANDLE hDevice);

	void StartAcquisition(int OperateID);

	void OpenDevice(int OperateID = 0);

	//virtual void Grap
	int refresh_list();

	CString Get_Error_Info();

	CString Get_Camera_Info();
	
private:
	int CloseCamera();

	void ClearBuffer();

	void UpdateDeviceList();

	void ShowErrorString(GX_STATUS emErrorStatus);

	bool AllocBufferForMainFrame();

	typedef struct CAMER_INFO
	{
		BITMAPINFO					*pBmpInfo;		  ///< BITMAPINFO 结构指针，显示图像时使用
		BYTE						*pImageBuffer;	  ///< 指向经过处理后的图像数据缓冲区
		BYTE                        *pRawBuffer;      ///< 指向原始RAW图缓冲区
		char						chBmpBuf[2048];	  ///< BIMTAPINFO 存储缓冲区，m_pBmpInfo即指向此缓冲区	
		int64_t                     nPayLoadSise;     ///< 图像块大小
		int64_t					    nImageWidth;	  ///< 图像宽度
		int64_t					    nImageHeight;	  ///< 图像高度	
		int64_t					    nBayerLayout;	  ///< Bayer排布格式
		bool						bIsColorFilter;	  ///< 判断是否为彩色相机
		BOOL						bIsOpen;		  ///< 相机已打开标志
		BOOL						bIsSnap;		  ///< 相机正在采集标志
		float						fFps;			  ///< 帧率
	}CAMER_INFO;

	int error_code;
	CString Error_Info;
	CString CameraName;
	CString VendorName;
	unsigned		int	m_deviceNum;
//	uint32_t            m_nDeviceNum;		///< 记录相机个数
	GX_DEV_HANDLE*	    m_hDevices;			///< 设备句柄指针
	CAMER_INFO*	  	    m_pstCam;			///< 相机数据结构体		
	int				    m_nOperateID;		///< 操作设备ID
	uint32_t		    m_nCamNum;			///< 相机数目
	BOOL			    m_bViewID;			///< 是否显示相机编号
	int*		 	    m_npfpsCont;		///< 帧率计数
	GX_DEVICE_BASE_INFO *m_pBaseinfo; 	    ///< 设备信息结构体
	GX_FRAME_DATA		pFrameData[MAX_DEVICE_MUN];			///<四个相机的拍照帧
	unsigned char		*pColorBuff[MAX_DEVICE_MUN];

	//std::vector<std::mutex> m_mtxs;
	
};

