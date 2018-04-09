// RS232Dialg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MSerialsUI.h"
#include "RS232Dialg.h"
#include "afxdialogex.h"
#include "Winspool.h"
#include "MainFrm.h"
#include <iostream>
#include <vector>
#include "MSerialsCam.h"
#include "../MSerialsCommon/Tools.h"
#include "../MSerialsCommon/common.h"
#include "third_party\Preferences\Preferences.h"
#include "halcon.h"
#include "Verity.h"
// RS232Dialg �Ի���

IMPLEMENT_DYNAMIC(RS232Dialg, CDialogEx)

RS232Dialg::RS232Dialg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RS232, pParent)
{
	is_serial_open = false;
	isParse = false;
}

RS232Dialg::~RS232Dialg()
{
}

void RS232Dialg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(RS232Dialg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &RS232Dialg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_COMFIRM, &RS232Dialg::OnBnClickedButtonComfirm)
	ON_MESSAGE(WM_COMM_RXCHAR, &RS232Dialg::OnCommunication)
	ON_BN_CLICKED(IDC_BUTTON_CLOSEPORT, &RS232Dialg::OnBnClickedButtonCloseport)
	ON_BN_CLICKED(IDC_BUTTON_REFRESHCAM, &RS232Dialg::OnBnClickedButtonRefreshcam)
	ON_BN_CLICKED(IDC_BUTTON_SAVEPARAPRJ, &RS232Dialg::OnBnClickedButtonSaveparaprj)
	ON_CBN_SELCHANGE(IDC_COMBO_PRJ_SEL, &RS232Dialg::OnSelchangeComboPrjSel)
	ON_BN_CLICKED(IDC_BUTTON_DEL_PRJ, &RS232Dialg::OnBnClickedButtonDelPrj)
	ON_BN_CLICKED(IDC_BUTTON_SAVEACC, &RS232Dialg::OnBnClickedButtonSaveacc)
END_MESSAGE_MAP()


// RS232Dialg ��Ϣ�������


void RS232Dialg::OnBnClickedButtonDelPrj()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	Verity v;
	v.DoModal();
	if ((GetTickCount() - global::GetIns()->SoftWareTick) > 1000)
	{
		return;
	}
	win_tool wt;
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();

	vector<CString> prj = wt.SplitCString(pMainFrm->sys.m_Prj_Vector, L",");
	if (prj.size() < 2) return;


	CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBO_PRJ_SEL);
	int nSel = pBox->GetCurSel();
	// ����ѡ����������ȡ�����ַ���
	CString str;
	pBox->GetLBText(nSel, str);

	bool found = false;
	for (std::vector<CString>::iterator it = prj.begin();it!=prj.end();)
	{
		//��bug����ΪCString�洢����ʽ��һ����unicode���ᵼ�¸��ֱ���任�������ַ���������һ��������ֵ��һ��
		if (0 == str.Compare(*it))
		{
			found = true;
			it = prj.erase(it);
		}
		else
			it++;		
	}

	if (!found)  return;
	
	str.Empty();
	for (std::vector<CString>::iterator it = prj.begin(); it != prj.end();++it)
	{
		//��bug����ΪCString�洢����ʽ��һ����unicode���ᵼ�¸��ֱ���任�������ַ���������һ��������ֵ��һ��
		str += *it;
		str += L",";
	}
	pMainFrm->sys.m_Prj_Vector = str;
	pMainFrm->sys.SaveParaFile(PARA_PRJ);

	pBox->DeleteString(nSel);
	pBox->SetCurSel(0);	
	OnSelchangeComboPrjSel();
}

void RS232Dialg::OnBnClickedButtonSave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	win_tool  wt;
	CMainFrame * pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	//CString str;
	CString newprjname;
	GetDlgItem(IDC_EDIT_NEWPRJ)->GetWindowText(newprjname);
	if (-1 != newprjname.ReverseFind(','))
	{
		AfxMessageBox(L"���ܰ������·���: ,");
		return;
	}

	if (newprjname.IsEmpty()) return;

	std::vector<CString> s = wt.SplitCString(pMainFrm->sys.m_Prj_Vector, L",");
	for (size_t i = 0; i < s.size(); i++)
	{
		if (!newprjname.Compare(s[i]))
		{
			AfxMessageBox(L"�������б�������");
			return;
		}
	}

	std::lock_guard<std::mutex> lck(m_mtx);
	

	pMainFrm->sys.m_Prj_Name = newprjname;
	CString NewPrj;
	if (s.size() == 0)
		NewPrj = newprjname;
	else
		NewPrj = L"," + newprjname;

	pMainFrm->sys.m_Prj_Vector += NewPrj;
	pMainFrm->sys.m_Prj_Index = (int)s.size(); //�պõ����һ����Ŀ����
	pMainFrm->sys.SaveParaFile(PARA_PRJ);

	CString AppPath = wt.AppPath();

	CString PrjPath = wt.AppPath() + L"\\" + newprjname;
	pMainFrm->ini.SetIniDir(PrjPath, false);
	if (0 == pMainFrm->ini.SetIniFile(L"PrgParameter.ini"))
	{
		pMainFrm->ini.initData();
		pMainFrm->ini.SaveParaFile(PARA_ALL);
	}
	pMainFrm->ini.LoadParaFile(PARA_ALL);
	CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBO_PRJ_SEL);
	pBox->AddString(newprjname);
	vector<CString> prj = wt.SplitCString(pMainFrm->sys.m_Prj_Vector, L",");
	pBox->SetCurSel(pMainFrm->sys.m_Prj_Index);
	UpdateUI();
	//global::GetIns()->txt->Save();
	delete global::GetIns()->txt;
	global::GetIns()->txt = new CTxt(pMainFrm->sys.m_Prj_Name);
}

UINT RS232Dialg::DelayInit(LPVOID lp)
{
	RS232Dialg* pDlg = (RS232Dialg*)lp;
	pDlg->OnBnClickedButtonRefreshcam();
	Sleep(1500);
	if(pDlg->is_serial_open == true)
		pDlg->SetTextColor(1);
	else
		pDlg->SetTextColor(0);
	return 0;
}

/*
AST �﷨��
github     llvm
Dialog widget
classs dialogQwiget
{
string funname;
Qt
}
class SVM:Qwight
string swm
*/

BOOL RS232Dialg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	win_tool wt;
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CComboBox* pBox1 = (CComboBox*)GetDlgItem(IDC_COMBO_PRJ_SEL);// ->AddString(L"1");
	
	vector<CString> prj = wt.SplitCString(pMainFrm->sys.m_Prj_Vector, L",");
	for (size_t i = 0; i < prj.size(); i++)
	{
		pBox1->AddString(prj[i]);
	}
	pBox1->SetCurSel(pMainFrm->sys.m_Prj_Index);

	//���¿ؼ�����
	UpdateUI();














	DWORD selcom = -1;
	int comserial = _ttoi(pMainFrm->sys.m_COM.Right(pMainFrm->sys.m_COM.Find(L"M")));// == "COM"

	CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBO_COMSEL);
	CString port = L"COM";
	int pBoxIndex = 0;
	for (int index = 0; index < 50; index++)
	{
		CString portnr;
		portnr.Format(L"%d", index);
		HANDLE hComm = CreateFile((LPCWSTR)(CString)(CStringA)(port+portnr),				// communication port string (COMX)
			GENERIC_READ | GENERIC_WRITE,	// read/write types
			0,								// comm devices must be opened with exclusive access
			NULL,							// no security attributes
			OPEN_EXISTING,					// comm devices must use OPEN_EXISTING
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,			// Async I/O
			NULL);
		if (INVALID_HANDLE_VALUE == hComm) continue;
		else
		{
			CloseHandle(hComm);
			pBox->AddString(port + portnr);
			if (comserial == index)
			{
				selcom = pBoxIndex;
			}
			++pBoxIndex;
		}
		
	}
	if (-1 == selcom)
	{
		CWinThread * T = AfxBeginThread(DelayInit, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		AfxMessageBox(L"δѡ�񴮿ڻ��ߵ��Բ����ڴ��ڣ�");
		return TRUE;
	}
	else
		global::GetIns()->system &= ~NOCOM;

	pBox->SetCurSel(selcom);
	CString str;
	pBox->GetLBText(selcom, str);
	InitPort((char*)(LPCSTR)(CStringA)str);

	
	CWinThread * T = AfxBeginThread(DelayInit, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);



	return TRUE;
	

}




void RS232Dialg::OnBnClickedButtonComfirm()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	m_serialport.StopMonitoring();
	m_serialport.Close();
	Sleep(100);
	CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBO_COMSEL);
	int nSel = pBox->GetCurSel();
	CString str;
	pBox->GetLBText(nSel, str);
	InitPort((char*)(LPCSTR)(CStringA)str);
}


void RS232Dialg::OnBnClickedButtonCloseport()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_serialport.Close();
	SetTextColor();
}

void RS232Dialg::SetTextColor(int sel)
{
	CRect rect;
	GetDlgItem(IDC_STATIC_INDICATE)->GetWindowRect(&rect);//��ȡ�ؼ�����ȫƵ��λ��
	int radiusx = rect.Width() / 2;
	int radiusxy = rect.Height() / 2;
	ScreenToClient(rect);//ת��Ϊ�Ի����ϵ����λ��
	int cx = rect.left + radiusx;
	int cy = rect.top + radiusxy;
	CClientDC dc(this); //CPaintDC dc(this);�����ﲻ������
	CPen* pPenBlue = new CPen;                                  //�������ʶ���
	pPenBlue->CreatePen(PS_SOLID, 3+radiusx, RGB(0, 255, 0));     //��ɫ����
	CPen* pPenRed= new CPen;                                 //�������ʶ���
	pPenRed->CreatePen(PS_SOLID, 3+radiusx, RGB(255, 0, 0)); //��ɫ����
	
													 //ѡ�к�ɫ���ʣ������浱ǰ����
	CGdiObject* pOldPen = NULL;
	if(0==sel)
		pOldPen = dc.SelectObject(pPenRed);
	else
		pOldPen = dc.SelectObject(pPenBlue);
	int i = 0;
	CString str;
	//��������ϵ
	CPoint OPos(cx, cy), NowPos;
	//CPoint OPos(40, 500), NowPos;
	//����x������
	dc.MoveTo(OPos);
	dc.Ellipse(cx - radiusx +3, cy - radiusxy+3, cx + radiusx-3, cy + radiusxy-3);
	dc.SelectObject(pOldPen);
	delete pPenBlue;
	delete pPenRed;
}

void RS232Dialg::InitPort(const char * str)
{
	CMainFrame * pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	if (m_serialport.InitPort(this,str , 9600, "NONE", 8U, "1"))
	{
		m_serialport.EnableSerialMessage(TRUE, FALSE, FALSE, FALSE, FALSE, FALSE);
		m_serialport.StartMonitoring();
		pMainFrm->sys.m_COM = str;
		pMainFrm->sys.SaveParaFile(PARA_PRJ);
		SetTextColor(1);
		is_serial_open = true;
	}
	else
	{
		SetTextColor();
		is_serial_open = false;
		AfxMessageBox(L"���ڳ�ʼ��ʧ��!");
	}
}


int RS232Dialg::firstProcedure()
{
	using namespace Halcon;
	CImageCard * ic = CImageCard::GetIns();
	CMainFrame * pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	int w = 0;
	int h = 0;
	int ch = 1;
	unsigned char * data = NULL;
	//��Ϊ�������ܲ��Ǻܺã����Բ������snap����ȡͼƬ
	Snap(w, h, &data, ch, 0, 0, pMainFrm->sys.m_nSnapTimeDelay);
	if (0 == w || 0 == h || NULL == data)	return OBJECT_NO_PIC;

	Halcon::set_check("~give_error");
	int r1 = static_cast<int>(pMainFrm->ini.trect.r1);
	int c1 = static_cast<int>(pMainFrm->ini.trect.c1);
	int width = static_cast<int>(pMainFrm->ini.trect.c2) - c1;
	int height = static_cast<int>(pMainFrm->ini.trect.r2) - r1;
	cv::Rect rt(c1, r1, width, height);

	cv::Mat src;
	if (1 == ch)
		src = cv::Mat(h, w, CV_8UC1);
	else
		src = cv::Mat(h, w, CV_8UC3);
	//��Ϊ��sdk���Ѿ��л����ˣ����û�����Ū
	src.data = data;
	Halcon::set_tposition(CImageCard::GetIns()->disp_hd, 0, 1);
	float res = cv_predict_inv(src, rt, false);
	CImageCard::GetIns()->MatToHImage(src,CImageCard::GetIns()->Image);
	CImageCard::GetIns()->Disp_Obj(CImageCard::GetIns()->Image, CImageCard::GetIns()->disp_hd);

	int isInv = false;
	Hobject ROI;
	char str[256] = { 0 };

	if (res > 0)
	{
		sprintf(str, "���� Ԥ��ֵ:%f", res);
		Halcon::set_color(CImageCard::GetIns()->disp_hd, "green");
	}
	else
	{ 
		sprintf(str, "��������� Ԥ��ֵ:%f", res);
		Halcon::set_color(CImageCard::GetIns()->disp_hd, "red");
		isInv = true;
	}
	set_draw(CImageCard::GetIns()->disp_hd, "margin");
	gen_rectangle1(&ROI,
		pMainFrm->ini.trect.r1,
		pMainFrm->ini.trect.c1,
		pMainFrm->ini.trect.r2,
		pMainFrm->ini.trect.c2);
	disp_obj(ROI, CImageCard::GetIns()->disp_hd);

	write_string(CImageCard::GetIns()->disp_hd, str);
	if (isInv)
		return OBJECT_INV;
	return OBJECT_OK;
}

int RS232Dialg::secondProcedure()
{
	using namespace Halcon;
	CImageCard * ic = CImageCard::GetIns();
	CMainFrame * pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	int w = 0;
	int h = 0;
	int ch = 1;
	unsigned char * data = NULL;
	//��Ϊ�������ܲ��Ǻܺã����Բ������snap����ȡͼƬ
	Snap(w, h, &data, ch, 0, 1, pMainFrm->sys.m_nSnapTimeDelay);
	if (0 == w || 0 == h || NULL == data)	return OBJECT_NO_PIC;
	Halcon::set_check("~give_error");
	//��Ϊ��sdk���Ѿ��л����ˣ����û�����Ū
	try
	{
		CImageCard::GetIns()->GenHobject(w, h, data, CImageCard::GetIns()->Image1, ch);
		CImageCard::GetIns()->Disp_Obj(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1);
		Halcon::set_tposition(CImageCard::GetIns()->disp_hd1, 0, 1);
		double r = pMainFrm->ini.bcircle.r;
		double c = pMainFrm->ini.bcircle.c;
		double radius = pMainFrm->ini.bcircle.radius;
		Herror error = CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, r, c, radius, 1, pMainFrm->ini.accurcy);
		if (H_MSG_TRUE != error)
			return OBJECT_VOID;
		return OBJECT_OK;
	}
	catch (HException &message)
	{
		write_string(CImageCard::GetIns()->disp_hd1, message.message);
		return OBJECT_VOID;
	}

	return OBJECT_VOID;
}


int RS232Dialg::Procedure(int camera_num)
{
	HWND hwnd = ::FindWindowEx(NULL, NULL, NULL, L"MSerialsUI");
	if (hwnd)	return PARAS_HASWINDOW;

	if (0 == camera_num)
	{
		return firstProcedure();
	}
	else if(1 == camera_num)
	{
		return secondProcedure();
	}
	return PARAS_ERROR;
}

void RS232Dialg::parse(const char * str)
{

	
	char msg[512] = "R:s199s29999\r\n\0\0";
	//�ҵ÷���ascii��PLC
	//char cstr[512] = { 'M','0',':','\0' };
	
	bool isSend = false;
	win_tool wt;
	std::vector<CString> strs = wt.SplitCString(str, L",");

	/**
	if (strs.size() == 0) return;
	else if (strs[0].Compare(L"data"))
	{

	}
	else if (strs[0].Compare(L"command"))
	{
	}
	else
	{

	}
	*/
	for (std::vector<CString>::iterator it = strs.begin(); it != strs.end(); it++)
	{
		int pos = -1;
		pos = it->Find(L"s1");
		if (-1 != pos)
		{
			char data_res[10] = {0};
			sprintf(data_res, "%2d", Procedure(0));
			memcpy(msg + 4, data_res, 2);
			isSend = true;
		}
		pos = it->Find(L"s2");
		if (-1 != pos)
		{
			char data_res[10] = { 0 };
			sprintf(data_res, "%2d", Procedure(1));
			memcpy(msg + 8, data_res, 2);
			isSend = true;
		}
	}
	//���ڷ��ظ�PLCȷ���Ƿ�Ͽ�����ʹ��
	static unsigned int counter = 0;
	char data_res[10] = { 0 };
	sprintf(data_res, "%2d", 1+(counter++%9));
	memcpy(msg + 10, data_res, 2);

	if (is_serial_open && isSend)
	{
		m_serialport.WriteToPort(msg, static_cast<DWORD>(strlen(msg)));
	}
	//������bug
	
#ifdef _TEST
//	std::cout << "cutted:" << strs.size() << std::endl;
	std::cout << msg << std::endl;
#endif
}

LRESULT RS232Dialg::OnCommunication(WPARAM ch, LPARAM port)
{
	static char msg[BUFFSIZE] = {0};
	static int msgIndex = 0;///<���ܴ��������ַ�
	static BOOL Detected = FALSE;
	
	std::lock_guard<std::mutex> lck(m_mtx);
	//ע���������unicode�ж��ģ�һ������´��ڲ���ʹ��Unicode����
	//static char _unicode_ch = ch;
	char _ch = static_cast<char>(ch);
	if (msgIndex >= (BUFFSIZE - 1))
		msgIndex = 0; //���������Ժ��ͷ����
	msg[msgIndex] = _ch;
	++msgIndex;
	//ȥ������������͵Ķ���\n_\0����ȥ����bug
	if ('\r' == _ch || '\n' == _ch || '\0' == _ch)
	{
		//��������ڼ�¼�����ĸ��ַ���Ϊ��β���ģ�
		//CR LF��CR��Ϊ��β ֻ�� LF LF��Ϊ��β�� ֻ�� '\0' ��ô'\0'��Ϊ��β
		//static char record = ch;
		//if (record == _ch)
		//if(msgIndexRecord == msgIndex;)
		//doting something
		if (!Detected)
		{
			//unicode ������Ҫ����0��β
			msg[msgIndex++] = '\0';
			msg[msgIndex++] = '\0';
			Parse(msg, msgIndex);
			msgIndex = 0;
		}
		Detected = TRUE;
	}
	else
	{
		Detected = FALSE;
	}
	return 0;
}

UINT RS232Dialg::StartDeal(LPVOID lp)
{
	RS232Dialg* pDlg = (RS232Dialg*)lp;
	pDlg->isParse = true;
	pDlg->parse(pDlg->cmd);
	pDlg->isParse = false;
	return 0;
}

UINT RS232Dialg::Parse(const char* str, int _strlen)
{
	if (isParse) return 0;
	//unicodeʵ����ֻ��Ҫ2��0x0��β
	memset(cmd, 0x0, _strlen+8);
	memcpy(cmd, str, _strlen);
	CWinThread * T = AfxBeginThread(StartDeal, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	return 0;
}



void RS232Dialg::OnBnClickedButtonRefreshcam()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	std::lock_guard<std::mutex> lck(m_mtx);
	int camera_num = enum_cameras();
	if (0 == camera_num)
	{
		AfxMessageBox(L"û���ҵ����");
		return;
	}
	if (1 == camera_num)
	{
		global::GetIns()->camera_error = (1 << 1);
	}
	if (2 == camera_num)
	{
		global::GetIns()->camera_error = 0;
	}

	int vendors = get_vendors_num();
#ifdef _TEST
	printf("vendors %d\n", vendors);
	for (int vendorIndex = 0; vendorIndex < vendors; vendorIndex++)
	{
		int cam_num = get_cameras_num_by_vendor(vendorIndex);
		const char* sdk_ver = get_sdk_ver_by_vendor(vendorIndex);
		printf("vendor name: %s , cmameras number %d\n", get_vendor_name(vendorIndex), cam_num);
		printf("sdk version :%s\n",sdk_ver);
		for (int c = 0; c < cam_num; c++)
		{
			printf(get_camera_info_by_index(vendorIndex, c));
			printf("\n");
		}
	}
#endif
}


void RS232Dialg::OnBnClickedButtonSaveparaprj()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	CMainFrame * pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	GetDlgItemText(IDC_EDIT_SNAPDELAY, str);
	pMainFrm->sys.m_nSnapTimeDelay = _ttoi(str);
	GetDlgItemText(IDC_EDIT_EXPLOSURE, str);
	pMainFrm->sys.m_nExposure = _ttof(str);
	pMainFrm->sys.SaveParaFile(PARA_PRJ);
}


void RS232Dialg::OnSelchangeComboPrjSel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMainFrame * pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	win_tool wt;
	CComboBox* pBox = (CComboBox*)GetDlgItem(IDC_COMBO_PRJ_SEL);
	int nSel = pBox->GetCurSel();
	// ����ѡ����������ȡ�����ַ���
	CString str;
	pBox->GetLBText(nSel, str);
	//bug 1
	pMainFrm->sys.m_Prj_Index = nSel;
	pMainFrm->sys.m_Prj_Name = str;
	CString PrjPath = wt.AppPath();
	CString AppPath = PrjPath + L"\\" + L"sys";

	pMainFrm->sys.SaveParaFile(PARA_PRJ);
	PrjPath += L"\\" + pMainFrm->sys.m_Prj_Name;
	pMainFrm->ini.SetIniDir(PrjPath, false);
	if (0 == pMainFrm->ini.SetIniFile(L"PrgParameter.ini"))
	{
		pMainFrm->ini.initData();
		pMainFrm->ini.SaveParaFile(PARA_IMAGE);
	}
	pMainFrm->ini.LoadParaFile(PARA_IMAGE);

	UpdateUI();
	//global::GetIns()->txt->Save();
	delete global::GetIns()->txt;
	global::GetIns()->txt = new CTxt(pMainFrm->sys.m_Prj_Name);

	if (H_MSG_TRUE != CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, 0, 0, 0,0, 2, 0, (char*)(LPCSTR)(CStringA)pMainFrm->sys.m_Prj_Name))
	{
		AfxMessageBox(L"δ�ܶ�ȡ�����ݵ�ģ���ļ�");
	}

	CString fileName = pMainFrm->sys.m_Prj_Name;
	if (false == load_train_data((char*)(LPCSTR)(CStringA)fileName))
	{
		AfxMessageBox(L"û�ж�ȡ����������Ĳ���");
	}

	
}

void RS232Dialg::UpdateUI()
{
	CString str;
	CMainFrame * pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	str.Format(L"%d", pMainFrm->sys.m_nSnapTimeDelay);
	SetDlgItemText(IDC_EDIT_SNAPDELAY, str);
	str.Format(L"%f", pMainFrm->sys.m_nExposure);
	SetDlgItemText(IDC_EDIT_EXPLOSURE, str);
	str.Format(L"%f", pMainFrm->ini.accurcy);
	SetDlgItemText(IDC_EDIT_ACCURCY, str);
}

void RS232Dialg::OnBnClickedButtonSaveacc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMainFrame * pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString str;
	GetDlgItemText(IDC_EDIT_ACCURCY, str);
	pMainFrm->ini.accurcy = _ttof(str);
	if (pMainFrm->ini.accurcy <= 0 || pMainFrm->ini.accurcy > 0.99)
	{
		pMainFrm->ini.accurcy = 0.5;
		SetDlgItemText(IDC_EDIT_ACCURCY, L"0.5");
	}


	pMainFrm->ini.SaveParaFile(PARA_IMAGE);
	
}
