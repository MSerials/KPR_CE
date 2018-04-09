#include "stdafx.h"
#include "Daheng.h"
#include <iostream>
#include "../MSerialsCommon/common.h"




#define  GX_VERIFY(emStatus) \
	if(emStatus != GX_STATUS_SUCCESS) \
							{\
							ShowErrorString(emStatus); \
							return;\
							}  ///< 错误提示函数宏定义

Daheng::Daheng()
	:m_nCamNum(0)
//	, m_nDeviceNum(0)
	, m_deviceNum(0)
	,error_code(0)
{
	m_hDevices = NULL;
	m_pBaseinfo = NULL;
	m_pstCam = NULL;
	m_npfpsCont = NULL;
	VendorName = L"DaHeng";
	//加载库
	//初始化设备库
	GX_STATUS emStatus = GX_STATUS_ERROR;
	emStatus = GXInitLib();
	if (emStatus != GX_STATUS_SUCCESS)
	{
		ShowErrorString(emStatus);
	}
#ifdef _TEST
	printf("going to init Daheng Lib\n");
#endif

#if 0
	OnOpenDevice();
	if (m_IsDevOpen != TRUE) {
		AfxMessageBox(L"未能正确打开相机");
		return -1;
	}

	OnStartCapture();
	if (m_bIsSnap != TRUE) {
		AfxMessageBox(L"未能正确采集图片");
		return -1;
	}
#endif
}


Daheng::~Daheng()
{
	CloseCamera();
	// 设备个数不为0将已获取到的设备信息显示到枚举信息列表
	//m_cmbcam.ResetContent();
	ClearBuffer();
	//BUG?
	GXCloseLib();
#ifdef _TEST
	printf("going to close Daheng Lib\n");
#endif
}



int Daheng::error_info()
{
	return 0;
}

const char * Daheng::vendor()
{
	return "DaHeng";
}

const char * Daheng::sdk_version()
{
	return "all mer sdk is ok\n";
}

const char * Daheng::CamerasInfo()
{
	return (char*)(LPCSTR)(CStringA)CameraName;
}

int Daheng::device_quantity()
{
	return static_cast<int>(m_deviceNum);
}

int Daheng::Snap(int & width, int & height, unsigned char **data, int &ch, int camera_index, int delay)
{
	if (camera_index >= device_quantity() || camera_index<0) return -1;
	static std::mutex m_mtx[MAX_DEVICE_MUN];
	//每一个相机一个锁，保证不相互锁住，只锁相同的相机
	std::lock_guard<std::mutex> lck(m_mtx[camera_index]);
	GX_STATUS emStatus = GXSendCommand(m_hDevices[camera_index], GX_COMMAND_TRIGGER_SOFTWARE);
	if (GX_STATUS_SUCCESS != emStatus) return emStatus;
	Sleep(delay);
	GXGetImage(m_hDevices[camera_index], &pFrameData[camera_index], 500);
	width = pFrameData[camera_index].nWidth;
	height = pFrameData[camera_index].nHeight;
	//printf("snap ");
	if (m_pstCam[camera_index].bIsColorFilter)
	{
		ch = 3;
		int nImgWidth = 3 * width;
		int nImgHeight = height;
		unsigned char *pImageBuffer = pColorBuff[camera_index];
		unsigned char *pRawBuf = (unsigned char*)pFrameData[camera_index].pImgBuf;
		//得对彩色相机进行排布,RGB为其顺序格式,opencv 可能是bgr
		//彩色相机需要经过RGB转换
		DxRaw8toRGB24(pFrameData[camera_index].pImgBuf, pColorBuff[camera_index], width, height, RAW2RGB_NEIGHBOUR, DX_PIXEL_COLOR_FILTER(m_pstCam[camera_index].nBayerLayout), TRUE);
		for (int i = 0; i < nImgHeight; i++)
		{
			memcpy((pRawBuf + (nImgHeight - i - 1) * nImgWidth), (pImageBuffer + i * nImgWidth), nImgWidth);
		}
		*data = pRawBuf;
	}
	else
	{
		ch = 1;
		*data = (unsigned char*)pFrameData[camera_index].pImgBuf;
	}
	//printf("snap over\n");
	return emStatus;
}

void Daheng::set_exposure(double exposure, int camera_index)
{
}

GX_STATUS Daheng::SetPixelFormat8bit(GX_DEV_HANDLE hDevice)
{
	GX_STATUS emStatus = GX_STATUS_SUCCESS;
	int64_t   nPixelSize = 0;
	uint32_t  nEnmuEntry = 0;
	size_t    nBufferSize = 0;
	BOOL      bIs8bit = TRUE;

	GX_ENUM_DESCRIPTION  *pEnumDescription = NULL;
	GX_ENUM_DESCRIPTION  *pEnumTemp = NULL;

	// 获取像素点大小
	emStatus = GXGetEnum(hDevice, GX_ENUM_PIXEL_SIZE, &nPixelSize);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		return emStatus;
	}

	// 判断为8bit时直接返回,否则设置为8bit
	if (nPixelSize == GX_PIXEL_SIZE_BPP8)
	{
		return GX_STATUS_SUCCESS;
	}
	else
	{
		// 获取设备支持的像素格式枚举数
		emStatus = GXGetEnumEntryNums(hDevice, GX_ENUM_PIXEL_FORMAT, &nEnmuEntry);
		if (emStatus != GX_STATUS_SUCCESS)
		{
			return emStatus;
		}

		// 为获取设备支持的像素格式枚举值准备资源
		nBufferSize = nEnmuEntry * sizeof(GX_ENUM_DESCRIPTION);
		pEnumDescription = new GX_ENUM_DESCRIPTION[nEnmuEntry];

		// 获取支持的枚举值
		emStatus = GXGetEnumDescription(hDevice, GX_ENUM_PIXEL_FORMAT, pEnumDescription, &nBufferSize);
		if (emStatus != GX_STATUS_SUCCESS)
		{
			if (pEnumDescription != NULL)
			{
				delete[] pEnumDescription;
				pEnumDescription = NULL;
			}
			return emStatus;
		}

		// 遍历设备支持的像素格式,设置像素格式为8bit,
		// 如设备支持的像素格式为Mono10和Mono8则设置其为Mono8
		for (uint32_t i = 0; i < nEnmuEntry; i++)
		{
			if ((pEnumDescription[i].nValue & GX_PIXEL_8BIT) == GX_PIXEL_8BIT)
			{
				emStatus = GXSetEnum(hDevice, GX_ENUM_PIXEL_FORMAT, pEnumDescription[i].nValue);
				break;
			}
		}

		// 释放资源
		if (pEnumDescription != NULL)
		{
			delete[] pEnumDescription;
			pEnumDescription = NULL;
		}
	} 

	return emStatus;
}

void Daheng::StartAcquisition(int OperateID)
{
	
	//设置采集模式连续采集
	GX_STATUS emStatus = GXSetEnum(m_hDevices[OperateID], GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);
	GX_VERIFY(emStatus);

	//设置触发模式为开
	emStatus = GXSetEnum(m_hDevices[OperateID], GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_ON);
	GX_VERIFY(emStatus);
	//发开始采集命令
	emStatus = GXSendCommand(m_hDevices[OperateID], GX_COMMAND_ACQUISITION_START);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		ShowErrorString(emStatus);
	}

}

void Daheng::OpenDevice(int OperateID)
{
	// TODO: Add your command handler code here
	GX_STATUS emStatus = GX_STATUS_SUCCESS;
	int64_t   nValue = 0;
	bool      bIsImplemented = false;
	char      szIndex[10] = { 0 };
	//m_nOperateID = 0;
	// 定义并初始化设备打开参数
	GX_OPEN_PARAM stOpenParam;
	stOpenParam.accessMode = GX_ACCESS_EXCLUSIVE;
	stOpenParam.openMode = GX_OPEN_USERID;
	stOpenParam.pszContent = "";
#pragma warning(disable:4996)
	//第一个相机序号从1开始 
	_itoa(OperateID + 1, szIndex, 10);

	//return;
	// 若相机已被打开则先关闭
	if (m_hDevices[OperateID] != NULL)
	{

		emStatus = GXCloseDevice(m_hDevices[OperateID]);
		GX_VERIFY(emStatus);
		m_hDevices[OperateID] = NULL;
	}

	//打开相机
	stOpenParam.pszContent = szIndex;
	
	emStatus = GXOpenDevice(&stOpenParam, &m_hDevices[OperateID]);

	//更新应用程序标题为当前操作的设备对象名称
//	SetWindowText(m_pBaseinfo[m_nOperateID].szDisplayName);
	GX_VERIFY(emStatus);
	m_pstCam[OperateID].bIsOpen = TRUE;

	//获取相机Bayer转换类型及是否支持输出彩色图像
	emStatus = GXIsImplemented(m_hDevices[OperateID], GX_ENUM_PIXEL_COLOR_FILTER, &bIsImplemented);
	GX_VERIFY(emStatus);
	m_pstCam[OperateID].bIsColorFilter = bIsImplemented;
	if (bIsImplemented)
	{
		emStatus = GXGetEnum(m_hDevices[OperateID], GX_ENUM_PIXEL_COLOR_FILTER, &nValue);
		GX_VERIFY(emStatus);
		m_pstCam[OperateID].nBayerLayout = nValue;
	}

	// 获取图像宽
	emStatus = GXGetInt(m_hDevices[OperateID], GX_INT_WIDTH, &nValue);
	GX_VERIFY(emStatus);
	m_pstCam[OperateID].nImageWidth = nValue;
	


	// 获取图像高
	emStatus = GXGetInt(m_hDevices[OperateID], GX_INT_HEIGHT, &nValue);
	GX_VERIFY(emStatus);
	m_pstCam[OperateID].nImageHeight = nValue;

	// 已知当前相机支持哪个8位图像数据格式可以直接设置
	// 例如设备支持数据格式GX_PIXEL_FORMAT_BAYER_GR8,则可按照以下代码实现
	// emStatus = GXSetEnum(m_hDevice, GX_ENUM_PIXEL_FORMAT, GX_PIXEL_FORMAT_BAYER_GR8);
	// VERIFY_STATUS_RET(emStatus);

	// 不清楚当前相机的数据格式时，可以调用以下函数将图像数据格式设置为8Bit
	emStatus = SetPixelFormat8bit(m_hDevices[OperateID]);
	GX_VERIFY(emStatus);

	// 获取原始图像大小
	emStatus = GXGetInt(m_hDevices[OperateID], GX_INT_PAYLOAD_SIZE, &nValue);
	GX_VERIFY(emStatus);
	m_pstCam[OperateID].nPayLoadSise = nValue;


	//偷懒了，直接申请成彩色的
	pColorBuff[OperateID] = (unsigned char*)malloc(3 * (size_t)nValue);
	pFrameData[OperateID].pImgBuf = malloc(3*(size_t)nValue);
	GXSetBool(m_hDevices[OperateID], GX_BOOL_FRAMESTORE_COVER_ACTIVE, true);


	//设置采集模式连续采集
	emStatus = GXSetEnum(m_hDevices[OperateID], GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);
	GX_VERIFY(emStatus);

	//设置触发模式为关
	emStatus = GXSetEnum(m_hDevices[OperateID], GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_OFF);
	GX_VERIFY(emStatus);

}

int Daheng::refresh_list()
{
	// TODO: Add your control notification handler code here
	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	// 停止显示设备信息
	//m_pMainFrame->m_nOperateID = -1;

	// 若设备被打开则先关闭
	emStatus = CloseCamera();
	if (emStatus != GX_STATUS_SUCCESS)
	{
		// 错误处理
		return (error_code = emStatus);
	}

//	return 0;
	// 设备个数不为0将已获取到的设备信息显示到枚举信息列表
	//m_cmbcam.ResetContent();
	ClearBuffer();

	// 枚举设备
	UpdateDeviceList();

	// 设备个数为0则直接返回
	
	//if (m_nDeviceNum <= 0)
	if (m_deviceNum <= 0)
	{
		m_nCamNum = m_deviceNum;
		return 0;
	}

	// 为主窗口分配资源
	if (AllocBufferForMainFrame())
	{
		CameraName.Empty();
		// 显示列表最多支持4只相机
		uint32_t i = 0;
		for (; i < m_deviceNum; i++)
		{
			//显示相机名称
			CString str = (CString)(CStringA)(LPCSTR)m_pBaseinfo[i].szDisplayName;
			str += L",";
			CameraName += str;
			OpenDevice(i);
			StartAcquisition(i);
			//m_cmbcam.InsertString(-1, m_pBaseinfo[i].szDisplayName);
		}

		Error_Info.Empty();

		
		if (GX_STATUS_SUCCESS != error_code)
		{
			return -2;
		}

		return (error_code = 0);
	}

	return -1;
}

CString Daheng::Get_Error_Info()
{
	return Error_Info;
}

CString Daheng::Get_Camera_Info()
{
	return CameraName;
}

int Daheng::CloseCamera()
{

	GX_STATUS emStatus = GX_STATUS_SUCCESS;
	uint32_t  i = 0;


	//如果相机没关闭，则关闭相机
	for (i = 0; i < (uint32_t)m_nCamNum; i++)
	{
		//if (m_pMainFrame->m_pstCam[i].bIsSnap)
		//{
			//停止采集
			emStatus = GXSendCommand(m_hDevices[i], GX_COMMAND_ACQUISITION_STOP);
			if (emStatus != GX_STATUS_SUCCESS)
			{
				// 错误处理
				ShowErrorString(emStatus);
			}

			//注销回调
			emStatus = GXUnregisterCaptureCallback(m_hDevices[i]);
			if (emStatus != GX_STATUS_SUCCESS)
			{
				// 错误处理
				ShowErrorString(emStatus);
			}

		//	m_pMainFrame->m_pstCam[i].bIsSnap = FALSE;
		//}
		//if (m_pMainFrame->m_pstCam[i].bIsOpen)
		//{
			//关闭相机
			emStatus = GXCloseDevice(m_hDevices[i]);
			if (emStatus != GX_STATUS_SUCCESS)
			{
				// 错误处理
				ShowErrorString(emStatus);
			}
			m_hDevices[i] = NULL;
		//	m_pMainFrame->m_hDevices[i] = NULL;
		//	m_pMainFrame->m_pstCam[i].bIsOpen = FALSE;
		//}
	}




	return emStatus;
}


void Daheng::ClearBuffer()
{
	
	CAMER_INFO *pTmpInfo = NULL;

	// 释放图像采集相关资源
	for (uint32_t i = 0; i < m_nCamNum; i++)
	{
		if (m_pstCam[i].pImageBuffer != NULL)
		{
			delete[]m_pstCam[i].pImageBuffer;
			m_pstCam[i].pImageBuffer = NULL;
		}

		if (m_pstCam[i].pRawBuffer != NULL)
		{
			delete[]m_pstCam[i].pRawBuffer;
			m_pstCam[i].pRawBuffer = NULL;
		}

		free(pFrameData[i].pImgBuf);
		free(pColorBuff[i]);
	//	::ReleaseDC(m_pCWnds[i]->m_hWnd, pFrame->m_hDCs[i]);
	//	pFrame->m_hDCs[i] = NULL;
	}

	// 清理设备句柄
	if (m_hDevices != NULL)
	{
		delete[]m_hDevices;
		m_hDevices = NULL;
	}

	// 清理设备信息
	if (m_pBaseinfo != NULL)
	{
		delete[]m_pBaseinfo;
		m_pBaseinfo = NULL;
	}

	// 释放自定义设备信息结构体指针
	if (m_pstCam != NULL)
	{
		delete[] m_pstCam;
		m_pstCam = NULL;
	}

	// 释放帧率信息的指针
	if (m_npfpsCont != NULL)
	{
		delete[] m_npfpsCont;
		m_npfpsCont = NULL;
	}

}

void Daheng::UpdateDeviceList()
{
	GX_STATUS  emStatus = GX_STATUS_SUCCESS;
	uint32_t   nDeviceNum = 0;
	bool       bRet = true;
	size_t     nSize = 0;

	//枚举设备
	emStatus = GXUpdateDeviceList(&nDeviceNum, 1000);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		ShowErrorString(emStatus);
		return;
	}

	//设备个数大于0,分配资源获取设备信息
	if (nDeviceNum > 0)
	{
		// 为获取设备信息准备资源
		m_pBaseinfo = new GX_DEVICE_BASE_INFO[nDeviceNum];
		if (m_pBaseinfo == NULL)
		{
			std::cout<<"为获取设备信息分配资源失败"<<std::endl;
			Error_Info += L"为获取设备信息分配资源失败";
			// 将主窗口的设备个数置为0
			nDeviceNum = 0;
			return;
		}
	
		//获取所有枚举相机的信息
		nSize = nDeviceNum * sizeof(GX_DEVICE_BASE_INFO);  //计算结构体大小
		emStatus = GXGetAllDeviceBaseInfo(m_pBaseinfo, &nSize);
		if (emStatus != GX_STATUS_SUCCESS)
		{
			printf("errorcode is %d\n", emStatus);
			ShowErrorString(emStatus);
			delete[] m_pBaseinfo;
			m_pBaseinfo = NULL;
			// 将设备个数置为0
			nDeviceNum = 0;
			return;
		}
	}
	m_deviceNum = min(nDeviceNum, MAX_DEVICE_MUN);
//	m_nDeviceNum = 4;// = nDeviceNum;
}


void Daheng::ShowErrorString(GX_STATUS emErrorStatus)
{
	char*     pchErrorInfo = NULL;
	size_t    nSize = 0;
	GX_STATUS emStatus = GX_STATUS_ERROR;
	error_code = emErrorStatus;
	// 获取错误信息长度，并申请内存空间
	emStatus = GXGetLastError(&emErrorStatus, NULL, &nSize);
	pchErrorInfo = new char[nSize];
	if (NULL == pchErrorInfo)
	{
		return;
	}

	// 获取错误信息，并显示
	emStatus = GXGetLastError(&emErrorStatus, pchErrorInfo, &nSize);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		std::cout << "接口调用失败！" << std::endl;
		Error_Info += L"接口调用失败！";
	}
	else
	{
		Error_Info += (CString)(LPCTSTR)(LPCSTR)pchErrorInfo;
		std::cout<<pchErrorInfo << std::endl;
	}

	// 释放申请的内存空间
	if (NULL != pchErrorInfo)
	{
		delete[] pchErrorInfo;
		pchErrorInfo = NULL;
	}
}


bool Daheng::AllocBufferForMainFrame()
{
	uint32_t i = 0;

	//---------------------申请空间-----------------------------
	//创建相机句柄
	m_hDevices = new GX_DEV_HANDLE[m_deviceNum];
	if (m_hDevices == NULL)
	{
		ClearBuffer();
		return false;
	}

	//创建相机数据结构体
	m_pstCam = new CAMER_INFO[m_deviceNum];
	if (m_pstCam == NULL)
	{
		ClearBuffer();
		return false;
	}

	m_npfpsCont = new int[m_deviceNum];
	if (m_npfpsCont == NULL)
	{
		ClearBuffer();
		return false;
	}

	//---------------------初始化资源信息-----------------------------
	for (i = 0; i<m_deviceNum; i++)
	{
		m_hDevices[i] = NULL;
		m_npfpsCont[i] = 0;

		m_pstCam[i].bIsColorFilter = false;
		m_pstCam[i].bIsOpen = FALSE;
		m_pstCam[i].bIsSnap = FALSE;
		m_pstCam[i].pBmpInfo = NULL;
		m_pstCam[i].pRawBuffer = NULL;
		m_pstCam[i].pImageBuffer = NULL;
		m_pstCam[i].fFps = 0.0;
		m_pstCam[i].nBayerLayout = 0;
		m_pstCam[i].nImageHeight = 0;
		m_pstCam[i].nImageWidth = 0;
		m_pstCam[i].nPayLoadSise = 0;
		memset(m_pstCam[i].chBmpBuf, 0, sizeof(m_pstCam[i].chBmpBuf));
	}

	return true;
}