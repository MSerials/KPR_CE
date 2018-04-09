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
		BITMAPINFO					*pBmpInfo;		  ///< BITMAPINFO �ṹָ�룬��ʾͼ��ʱʹ��
		BYTE						*pImageBuffer;	  ///< ָ�򾭹�������ͼ�����ݻ�����
		BYTE                        *pRawBuffer;      ///< ָ��ԭʼRAWͼ������
		char						chBmpBuf[2048];	  ///< BIMTAPINFO �洢��������m_pBmpInfo��ָ��˻�����	
		int64_t                     nPayLoadSise;     ///< ͼ����С
		int64_t					    nImageWidth;	  ///< ͼ����
		int64_t					    nImageHeight;	  ///< ͼ��߶�	
		int64_t					    nBayerLayout;	  ///< Bayer�Ų���ʽ
		bool						bIsColorFilter;	  ///< �ж��Ƿ�Ϊ��ɫ���
		BOOL						bIsOpen;		  ///< ����Ѵ򿪱�־
		BOOL						bIsSnap;		  ///< ������ڲɼ���־
		float						fFps;			  ///< ֡��
	}CAMER_INFO;

	int error_code;
	CString Error_Info;
	CString CameraName;
	CString VendorName;
	unsigned		int	m_deviceNum;
//	uint32_t            m_nDeviceNum;		///< ��¼�������
	GX_DEV_HANDLE*	    m_hDevices;			///< �豸���ָ��
	CAMER_INFO*	  	    m_pstCam;			///< ������ݽṹ��		
	int				    m_nOperateID;		///< �����豸ID
	uint32_t		    m_nCamNum;			///< �����Ŀ
	BOOL			    m_bViewID;			///< �Ƿ���ʾ������
	int*		 	    m_npfpsCont;		///< ֡�ʼ���
	GX_DEVICE_BASE_INFO *m_pBaseinfo; 	    ///< �豸��Ϣ�ṹ��
	GX_FRAME_DATA		pFrameData[MAX_DEVICE_MUN];			///<�ĸ����������֡
	unsigned char		*pColorBuff[MAX_DEVICE_MUN];

	//std::vector<std::mutex> m_mtxs;
	
};

