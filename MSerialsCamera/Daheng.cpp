#include "stdafx.h"
#include "Daheng.h"
#include <iostream>
#include "../MSerialsCommon/common.h"




#define  GX_VERIFY(emStatus) \
	if(emStatus != GX_STATUS_SUCCESS) \
							{\
							ShowErrorString(emStatus); \
							return;\
							}  ///< ������ʾ�����궨��

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
	//���ؿ�
	//��ʼ���豸��
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
		AfxMessageBox(L"δ����ȷ�����");
		return -1;
	}

	OnStartCapture();
	if (m_bIsSnap != TRUE) {
		AfxMessageBox(L"δ����ȷ�ɼ�ͼƬ");
		return -1;
	}
#endif
}


Daheng::~Daheng()
{
	CloseCamera();
	// �豸������Ϊ0���ѻ�ȡ�����豸��Ϣ��ʾ��ö����Ϣ�б�
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
	//ÿһ�����һ��������֤���໥��ס��ֻ����ͬ�����
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
		//�öԲ�ɫ��������Ų�,RGBΪ��˳���ʽ,opencv ������bgr
		//��ɫ�����Ҫ����RGBת��
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

	// ��ȡ���ص��С
	emStatus = GXGetEnum(hDevice, GX_ENUM_PIXEL_SIZE, &nPixelSize);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		return emStatus;
	}

	// �ж�Ϊ8bitʱֱ�ӷ���,��������Ϊ8bit
	if (nPixelSize == GX_PIXEL_SIZE_BPP8)
	{
		return GX_STATUS_SUCCESS;
	}
	else
	{
		// ��ȡ�豸֧�ֵ����ظ�ʽö����
		emStatus = GXGetEnumEntryNums(hDevice, GX_ENUM_PIXEL_FORMAT, &nEnmuEntry);
		if (emStatus != GX_STATUS_SUCCESS)
		{
			return emStatus;
		}

		// Ϊ��ȡ�豸֧�ֵ����ظ�ʽö��ֵ׼����Դ
		nBufferSize = nEnmuEntry * sizeof(GX_ENUM_DESCRIPTION);
		pEnumDescription = new GX_ENUM_DESCRIPTION[nEnmuEntry];

		// ��ȡ֧�ֵ�ö��ֵ
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

		// �����豸֧�ֵ����ظ�ʽ,�������ظ�ʽΪ8bit,
		// ���豸֧�ֵ����ظ�ʽΪMono10��Mono8��������ΪMono8
		for (uint32_t i = 0; i < nEnmuEntry; i++)
		{
			if ((pEnumDescription[i].nValue & GX_PIXEL_8BIT) == GX_PIXEL_8BIT)
			{
				emStatus = GXSetEnum(hDevice, GX_ENUM_PIXEL_FORMAT, pEnumDescription[i].nValue);
				break;
			}
		}

		// �ͷ���Դ
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
	
	//���òɼ�ģʽ�����ɼ�
	GX_STATUS emStatus = GXSetEnum(m_hDevices[OperateID], GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);
	GX_VERIFY(emStatus);

	//���ô���ģʽΪ��
	emStatus = GXSetEnum(m_hDevices[OperateID], GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_ON);
	GX_VERIFY(emStatus);
	//����ʼ�ɼ�����
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
	// ���岢��ʼ���豸�򿪲���
	GX_OPEN_PARAM stOpenParam;
	stOpenParam.accessMode = GX_ACCESS_EXCLUSIVE;
	stOpenParam.openMode = GX_OPEN_USERID;
	stOpenParam.pszContent = "";
#pragma warning(disable:4996)
	//��һ�������Ŵ�1��ʼ 
	_itoa(OperateID + 1, szIndex, 10);

	//return;
	// ������ѱ������ȹر�
	if (m_hDevices[OperateID] != NULL)
	{

		emStatus = GXCloseDevice(m_hDevices[OperateID]);
		GX_VERIFY(emStatus);
		m_hDevices[OperateID] = NULL;
	}

	//�����
	stOpenParam.pszContent = szIndex;
	
	emStatus = GXOpenDevice(&stOpenParam, &m_hDevices[OperateID]);

	//����Ӧ�ó������Ϊ��ǰ�������豸��������
//	SetWindowText(m_pBaseinfo[m_nOperateID].szDisplayName);
	GX_VERIFY(emStatus);
	m_pstCam[OperateID].bIsOpen = TRUE;

	//��ȡ���Bayerת�����ͼ��Ƿ�֧�������ɫͼ��
	emStatus = GXIsImplemented(m_hDevices[OperateID], GX_ENUM_PIXEL_COLOR_FILTER, &bIsImplemented);
	GX_VERIFY(emStatus);
	m_pstCam[OperateID].bIsColorFilter = bIsImplemented;
	if (bIsImplemented)
	{
		emStatus = GXGetEnum(m_hDevices[OperateID], GX_ENUM_PIXEL_COLOR_FILTER, &nValue);
		GX_VERIFY(emStatus);
		m_pstCam[OperateID].nBayerLayout = nValue;
	}

	// ��ȡͼ���
	emStatus = GXGetInt(m_hDevices[OperateID], GX_INT_WIDTH, &nValue);
	GX_VERIFY(emStatus);
	m_pstCam[OperateID].nImageWidth = nValue;
	


	// ��ȡͼ���
	emStatus = GXGetInt(m_hDevices[OperateID], GX_INT_HEIGHT, &nValue);
	GX_VERIFY(emStatus);
	m_pstCam[OperateID].nImageHeight = nValue;

	// ��֪��ǰ���֧���ĸ�8λͼ�����ݸ�ʽ����ֱ������
	// �����豸֧�����ݸ�ʽGX_PIXEL_FORMAT_BAYER_GR8,��ɰ������´���ʵ��
	// emStatus = GXSetEnum(m_hDevice, GX_ENUM_PIXEL_FORMAT, GX_PIXEL_FORMAT_BAYER_GR8);
	// VERIFY_STATUS_RET(emStatus);

	// �������ǰ��������ݸ�ʽʱ�����Ե������º�����ͼ�����ݸ�ʽ����Ϊ8Bit
	emStatus = SetPixelFormat8bit(m_hDevices[OperateID]);
	GX_VERIFY(emStatus);

	// ��ȡԭʼͼ���С
	emStatus = GXGetInt(m_hDevices[OperateID], GX_INT_PAYLOAD_SIZE, &nValue);
	GX_VERIFY(emStatus);
	m_pstCam[OperateID].nPayLoadSise = nValue;


	//͵���ˣ�ֱ������ɲ�ɫ��
	pColorBuff[OperateID] = (unsigned char*)malloc(3 * (size_t)nValue);
	pFrameData[OperateID].pImgBuf = malloc(3*(size_t)nValue);
	GXSetBool(m_hDevices[OperateID], GX_BOOL_FRAMESTORE_COVER_ACTIVE, true);


	//���òɼ�ģʽ�����ɼ�
	emStatus = GXSetEnum(m_hDevices[OperateID], GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);
	GX_VERIFY(emStatus);

	//���ô���ģʽΪ��
	emStatus = GXSetEnum(m_hDevices[OperateID], GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_OFF);
	GX_VERIFY(emStatus);

}

int Daheng::refresh_list()
{
	// TODO: Add your control notification handler code here
	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	// ֹͣ��ʾ�豸��Ϣ
	//m_pMainFrame->m_nOperateID = -1;

	// ���豸�������ȹر�
	emStatus = CloseCamera();
	if (emStatus != GX_STATUS_SUCCESS)
	{
		// ������
		return (error_code = emStatus);
	}

//	return 0;
	// �豸������Ϊ0���ѻ�ȡ�����豸��Ϣ��ʾ��ö����Ϣ�б�
	//m_cmbcam.ResetContent();
	ClearBuffer();

	// ö���豸
	UpdateDeviceList();

	// �豸����Ϊ0��ֱ�ӷ���
	
	//if (m_nDeviceNum <= 0)
	if (m_deviceNum <= 0)
	{
		m_nCamNum = m_deviceNum;
		return 0;
	}

	// Ϊ�����ڷ�����Դ
	if (AllocBufferForMainFrame())
	{
		CameraName.Empty();
		// ��ʾ�б����֧��4ֻ���
		uint32_t i = 0;
		for (; i < m_deviceNum; i++)
		{
			//��ʾ�������
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


	//������û�رգ���ر����
	for (i = 0; i < (uint32_t)m_nCamNum; i++)
	{
		//if (m_pMainFrame->m_pstCam[i].bIsSnap)
		//{
			//ֹͣ�ɼ�
			emStatus = GXSendCommand(m_hDevices[i], GX_COMMAND_ACQUISITION_STOP);
			if (emStatus != GX_STATUS_SUCCESS)
			{
				// ������
				ShowErrorString(emStatus);
			}

			//ע���ص�
			emStatus = GXUnregisterCaptureCallback(m_hDevices[i]);
			if (emStatus != GX_STATUS_SUCCESS)
			{
				// ������
				ShowErrorString(emStatus);
			}

		//	m_pMainFrame->m_pstCam[i].bIsSnap = FALSE;
		//}
		//if (m_pMainFrame->m_pstCam[i].bIsOpen)
		//{
			//�ر����
			emStatus = GXCloseDevice(m_hDevices[i]);
			if (emStatus != GX_STATUS_SUCCESS)
			{
				// ������
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

	// �ͷ�ͼ��ɼ������Դ
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

	// �����豸���
	if (m_hDevices != NULL)
	{
		delete[]m_hDevices;
		m_hDevices = NULL;
	}

	// �����豸��Ϣ
	if (m_pBaseinfo != NULL)
	{
		delete[]m_pBaseinfo;
		m_pBaseinfo = NULL;
	}

	// �ͷ��Զ����豸��Ϣ�ṹ��ָ��
	if (m_pstCam != NULL)
	{
		delete[] m_pstCam;
		m_pstCam = NULL;
	}

	// �ͷ�֡����Ϣ��ָ��
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

	//ö���豸
	emStatus = GXUpdateDeviceList(&nDeviceNum, 1000);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		ShowErrorString(emStatus);
		return;
	}

	//�豸��������0,������Դ��ȡ�豸��Ϣ
	if (nDeviceNum > 0)
	{
		// Ϊ��ȡ�豸��Ϣ׼����Դ
		m_pBaseinfo = new GX_DEVICE_BASE_INFO[nDeviceNum];
		if (m_pBaseinfo == NULL)
		{
			std::cout<<"Ϊ��ȡ�豸��Ϣ������Դʧ��"<<std::endl;
			Error_Info += L"Ϊ��ȡ�豸��Ϣ������Դʧ��";
			// �������ڵ��豸������Ϊ0
			nDeviceNum = 0;
			return;
		}
	
		//��ȡ����ö���������Ϣ
		nSize = nDeviceNum * sizeof(GX_DEVICE_BASE_INFO);  //����ṹ���С
		emStatus = GXGetAllDeviceBaseInfo(m_pBaseinfo, &nSize);
		if (emStatus != GX_STATUS_SUCCESS)
		{
			printf("errorcode is %d\n", emStatus);
			ShowErrorString(emStatus);
			delete[] m_pBaseinfo;
			m_pBaseinfo = NULL;
			// ���豸������Ϊ0
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
	// ��ȡ������Ϣ���ȣ��������ڴ�ռ�
	emStatus = GXGetLastError(&emErrorStatus, NULL, &nSize);
	pchErrorInfo = new char[nSize];
	if (NULL == pchErrorInfo)
	{
		return;
	}

	// ��ȡ������Ϣ������ʾ
	emStatus = GXGetLastError(&emErrorStatus, pchErrorInfo, &nSize);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		std::cout << "�ӿڵ���ʧ�ܣ�" << std::endl;
		Error_Info += L"�ӿڵ���ʧ�ܣ�";
	}
	else
	{
		Error_Info += (CString)(LPCTSTR)(LPCSTR)pchErrorInfo;
		std::cout<<pchErrorInfo << std::endl;
	}

	// �ͷ�������ڴ�ռ�
	if (NULL != pchErrorInfo)
	{
		delete[] pchErrorInfo;
		pchErrorInfo = NULL;
	}
}


bool Daheng::AllocBufferForMainFrame()
{
	uint32_t i = 0;

	//---------------------����ռ�-----------------------------
	//����������
	m_hDevices = new GX_DEV_HANDLE[m_deviceNum];
	if (m_hDevices == NULL)
	{
		ClearBuffer();
		return false;
	}

	//����������ݽṹ��
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

	//---------------------��ʼ����Դ��Ϣ-----------------------------
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