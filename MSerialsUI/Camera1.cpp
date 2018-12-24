// Camera1.cpp : 实现文件
//

#include "stdafx.h"
#include "MSerialsUI.h"
#include "Camera1.h"
#include "afxdialogex.h"
#include "MSerialsCam.h"
#include "halcon.h"
#include <iostream>
#include "MainFrm.h"
#include "MSerialsCV.h"
#include "Verity.h"
// Camera1 对话框

IMPLEMENT_DYNAMIC(Camera1, CDialogEx)

Camera1::Camera1(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CAMERA1, pParent)
{

}

Camera1::~Camera1()
{
}

void Camera1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Camera1, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_DRAW_RECT, &Camera1::OnBnClickedButtonDrawRect)
	ON_BN_CLICKED(IDC_BUTTON_DRAW_GET_VENODR, &Camera1::OnBnClickedButtonDrawGetVenodr)
	ON_BN_CLICKED(IDC_BUTTON_READIMG1, &Camera1::OnBnClickedButtonReadimg1)
	ON_BN_CLICKED(IDC_BUTTON_DEAL_TEST, &Camera1::OnBnClickedButtonDealTest)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_RECT, &Camera1::OnBnClickedButtonShowRect)
	ON_BN_CLICKED(IDC_BUTTON_SAVEIMAGE, &Camera1::OnBnClickedButtonSaveimage)
	ON_BN_CLICKED(IDC_BUTTON_TRAIN_IMAGES, &Camera1::OnBnClickedButtonTrainImages)
	ON_BN_CLICKED(IDC_BUTTON_PREDICT, &Camera1::OnBnClickedButtonPredict)
	ON_BN_CLICKED(IDC_BUTTON_SNAP1, &Camera1::OnBnClickedButtonSnap1)
	ON_BN_CLICKED(IDC_BUTTON_SAVEASF, &Camera1::OnBnClickedButtonSaveasf)
	ON_BN_CLICKED(IDC_BUTTON_SAVEASB, &Camera1::OnBnClickedButtonSaveasb)
	ON_BN_CLICKED(IDC_BUTTON_OPENF, &Camera1::OnBnClickedButtonOpenf)
	ON_BN_CLICKED(IDC_BUTTON_OPENB, &Camera1::OnBnClickedButtonOpenb)
	ON_BN_CLICKED(IDC_BUTTON_SAVEIMAGEINCHECK, &Camera1::OnBnClickedButtonSaveimageincheck)
END_MESSAGE_MAP()


// Camera1 消息处理程序


void Camera1::OnBnClickedButtonDrawRect()
{
	// TODO: 在此添加控件通知处理程序代码
	using namespace Halcon;
	CImageCard * ic = CImageCard::GetIns();
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();

	if (CImageCard::NO_RECT != ic->RectInfo) { AfxMessageBox(ic->ShowRectInfo(ic->RectInfo)); return; }
	Halcon::HTuple  Exception;
	set_check("~give_error");
	try
	{
		ic->RectInfo = CImageCard::TRECT;
		double row, col, row2, col2;
		Hobject ROI;
		Halcon::set_color(ic->disp_hd,"green");
		set_draw(ic->disp_hd, "margin");
		draw_rectangle1(ic->disp_hd, &row, &col, &row2, &col2);
		gen_rectangle1(&ROI, row, col, row2, col2);
		disp_obj(ROI, ic->disp_hd);
		pMainFrm->ini.trect.c1 = col;
		pMainFrm->ini.trect.r1 = row;
		pMainFrm->ini.trect.c2 = col2;
		pMainFrm->ini.trect.r2 = row2;
		pMainFrm->ini.SaveParaFile(PARA_IMAGE);
		ic->RectInfo = CImageCard::NO_RECT;
	}
	catch (HException except)
	{
		write_string(ic->disp_hd, except.message);
		ic->RectInfo = CImageCard::NO_RECT;
		AfxMessageBox(L"设定搜索范围失败！");
	}
	return;

}


void Camera1::OnBnClickedButtonDrawGetVenodr()
{
	// TODO: 在此添加控件通知处理程序代码
	using namespace Halcon;
	CImageCard * ic = CImageCard::GetIns();
	CMainFrame * pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	int w = 0;
	int h = 0;
	int ch = 1;
	unsigned char * data = NULL;
	//因为电脑性能不是很好，所以采用这个snap来采取图片
	Snap(w, h, &data, ch, 0, 0, pMainFrm->sys.m_nSnapTimeDelay);
	if (NULL == data) return ;
	Halcon::set_check("~give_error");

	int r1 = static_cast<int>(pMainFrm->ini.trect.r1);
	int c1 = static_cast<int>(pMainFrm->ini.trect.c1);
	int width = static_cast<int>(pMainFrm->ini.trect.c2) - c1;
	int height = static_cast<int>(pMainFrm->ini.trect.r2) - r1;
	cv::Rect rt(c1, r1, width, height);


	if (0 == w || 0 == h || NULL == data)	return ;
	cv::Mat src;
	if (1 == ch)
		src = cv::Mat(h, w, CV_8UC1);
	else
		src = cv::Mat(h, w, CV_8UC3);
	//因为在sdk里已经有缓存了，利用缓存来弄
	src.data = data;

}


void Camera1::OnBnClickedButtonReadimg1()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fileDlg(TRUE, (LPCTSTR)"BMP", L"", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"All Files (*.*)|*.*|位图文件 (*.bmp)|*.bmp||");	//打开读取文件对话框
	if (fileDlg.DoModal() != IDOK) return;
	try {
		const char* imgPath = (char*)(LPCSTR)CStringA(fileDlg.GetPathName());
		CImageCard::GetIns()->mimg = cv::imread(imgPath);
		Halcon::set_check("~give_error");
		CImageCard::GetIns()->MatToHImage(CImageCard::GetIns()->mimg, CImageCard::GetIns()->Image);
		//Halcon::read_image(&CImageCard::GetIns()->Image, imgPath);
		CImageCard::GetIns()->Disp_Obj(CImageCard::GetIns()->Image, CImageCard::GetIns()->disp_hd);
	}
	catch (...)
	{
		Halcon::set_check("give_error");
	}
}


void Camera1::OnBnClickedButtonDealTest()
{
	// TODO: 在此添加控件通知处理程序代码
	using namespace Halcon;
	CImageCard * ic = CImageCard::GetIns();
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();

	if (CImageCard::GetIns()->mimg.empty()) return;

	int r1 = static_cast<int>(pMainFrm->ini.trect.r1);
	int c1 = static_cast<int>(pMainFrm->ini.trect.c1);
	int width = static_cast<int>(pMainFrm->ini.trect.c2) - c1;
	int height = static_cast<int>(pMainFrm->ini.trect.r2) - r1;
	cv::Rect rt(c1, r1, width, height);

	float res = cv_predict_inv(CImageCard::GetIns()->mimg, rt, false);
	CImageCard::GetIns()->MatToHImage(CImageCard::GetIns()->mimg, CImageCard::GetIns()->Image);
	CImageCard::GetIns()->Disp_Obj(CImageCard::GetIns()->Image, CImageCard::GetIns()->disp_hd);
	set_tposition(CImageCard::GetIns()->disp_hd, 10, 10);
	Hobject ROI;
	if (res > 0)
		Halcon::set_color(CImageCard::GetIns()->disp_hd, "green");
	else
		Halcon::set_color(CImageCard::GetIns()->disp_hd, "red");
	set_draw(CImageCard::GetIns()->disp_hd, "margin");
	gen_rectangle1(&ROI,
		pMainFrm->ini.trect.r1,
		pMainFrm->ini.trect.c1,
		pMainFrm->ini.trect.r2,
		pMainFrm->ini.trect.c2);
	disp_obj(ROI, CImageCard::GetIns()->disp_hd);


	char str[256] = { 0 };
	sprintf(str, "预测值:%f", res);
	write_string(CImageCard::GetIns()->disp_hd, str);
}


void Camera1::OnBnClickedButtonShowRect()
{
	// TODO: 在此添加控件通知处理程序代码
	using namespace Halcon;
	CImageCard * ic = CImageCard::GetIns();
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();

	if (CImageCard::NO_RECT != ic->RectInfo) { AfxMessageBox(ic->ShowRectInfo(ic->RectInfo)); return; }
	Halcon::HTuple  Exception;
	set_check("~give_error");
	try
	{
		clear_window(ic->disp_hd);
		CImageCard::GetIns()->Disp_Obj(CImageCard::GetIns()->Image, CImageCard::GetIns()->disp_hd);
		Hobject ROI;
		Halcon::set_color(ic->disp_hd, "green");
		set_draw(ic->disp_hd, "margin");
		gen_rectangle1(&ROI, 
			pMainFrm->ini.trect.r1,
			pMainFrm->ini.trect.c1, 
			pMainFrm->ini.trect.r2, 
			pMainFrm->ini.trect.c2);
		disp_obj(ROI, ic->disp_hd);

	}
	catch (HException except)
	{
		write_string(ic->disp_hd, except.message);
	}
}


void Camera1::OnBnClickedButtonSaveimage()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	using namespace Halcon;
	win_tool wt;
	CString time = wt.Get_Date_Time();
	CString imagename = pMainFrm->sys.m_Prj_Name + L"\\"+ time;
	set_check("~give_error");
	try {
		write_image(CImageCard::GetIns()->Image, "bmp", 0, (char*)(LPCSTR)(CStringA)imagename);
	}
	catch (HException except)
	{
		write_string(CImageCard::GetIns()->disp_hd, except.message);
	}
}


void Camera1::OnBnClickedButtonTrainImages()
{
	// TODO: 在此添加控件通知处理程序代码
	if (AfxMessageBox(L"确认更新数据？", MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)  return;
	using namespace cv;
	win_tool wt;
	CSelectFolderDlg CDir;
	CString AppPath = wt.AppPath();
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();

	CString FrontsDir = AppPath + L"\\" + pMainFrm->sys.m_Prj_Name + L"\\HOLEFRONT";
	CString BacksDir = AppPath + L"\\" + pMainFrm->sys.m_Prj_Name + L"\\HOLEBACK";

	::CreateDirectory(FrontsDir, NULL);
	::CreateDirectory(BacksDir, NULL);

	std::vector<CString> fdir = CDir.Get_FileNames(FrontsDir);
	std::vector<CString> bdir = CDir.Get_FileNames(BacksDir);

	int r1 = static_cast<int>(pMainFrm->ini.trect.r1);
	int c1 = static_cast<int>(pMainFrm->ini.trect.c1);
	int width = static_cast<int>(pMainFrm->ini.trect.c2) - c1;
	int height = static_cast<int>(pMainFrm->ini.trect.r2) - r1;
	Rect rt(c1, r1, width, height);

	CString xml_path = AppPath + L"\\" + pMainFrm->sys.m_Prj_Name;

	CString fcdir;// = CDir.Get_FileNames(FrontsDir);
	CString bcdir;// = CDir.Get_FileNames(BacksDir);
	for (size_t i = 0; i < fdir.size(); i++)
	{
		fcdir += fdir[i] + L"|";
	}

	for (size_t i = 0; i < bdir.size(); i++)
	{
		bcdir += bdir[i] + L"|";
	}


	float res = cv_train_inv((char*)(LPCSTR)(CStringA)fcdir, (char*)(LPCSTR)(CStringA)bcdir, rt, (char*)(LPCSTR)(CStringA)xml_path, false);
#ifdef _TEST
	printf("traing result is %f\n",res);
#endif
}


void Camera1::OnBnClickedButtonPredict()
{
	// TODO: 在此添加控件通知处理程序代码
	using namespace Halcon;
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	int r1 = static_cast<int>(pMainFrm->ini.trect.r1);
	int c1 = static_cast<int>(pMainFrm->ini.trect.c1);
	int width = static_cast<int>(pMainFrm->ini.trect.c2) - c1;
	int height = static_cast<int>(pMainFrm->ini.trect.r2) - r1;
	cv::Rect rt(c1, r1, width, height);

	int w = 0;
	int h = 0;
	int ch = 1;
	unsigned char * data = NULL;
	//因为电脑性能不是很好，所以采用这个snap来采取图片
	Snap(w, h, &data, ch, 0, 0, 35);
	if (0==w || 0==h || NULL==data)	return;
	cv::Mat src;
	if(1==ch)
		src = cv::Mat(h,w,CV_8UC1);
	else
		src = cv::Mat(h,w, CV_8UC3);
	//因为在sdk里已经有缓存了，利用缓存来弄
	src.data = data;
	float res = cv_predict_inv(src, rt, false);
	CImageCard::GetIns()->MatToHImage(src, CImageCard::GetIns()->Image);
	CImageCard::GetIns()->Disp_Obj(CImageCard::GetIns()->Image, CImageCard::GetIns()->disp_hd);
	set_tposition(CImageCard::GetIns()->disp_hd, 10, 10 );
	Hobject ROI;
	if (res > 0)
		Halcon::set_color(CImageCard::GetIns()->disp_hd, "green");
	else
		Halcon::set_color(CImageCard::GetIns()->disp_hd, "red");
	set_draw(CImageCard::GetIns()->disp_hd, "margin");
	gen_rectangle1(&ROI,
		pMainFrm->ini.trect.r1,
		pMainFrm->ini.trect.c1,
		pMainFrm->ini.trect.r2,
		pMainFrm->ini.trect.c2);
	disp_obj(ROI, CImageCard::GetIns()->disp_hd);

	
	char str[256] = { 0 };
	sprintf(str,"预测值:%f", res);
	write_string(CImageCard::GetIns()->disp_hd,str);
}


BOOL Camera1::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString fileName =  pMainFrm->sys.m_Prj_Name;

	if (pMainFrm->sys.SaveImgInCheck == 0)
	{
		SetDlgItemText(IDC_BUTTON_SAVEIMAGEINCHECK, L"处于保存图片状态");
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_SAVEIMAGEINCHECK, L"不在保存图片状态");
	}


	if (false == load_train_data((char*)(LPCSTR)(CStringA)fileName))
	{
		AfxMessageBox(L"没有读取到检测正反的参数");
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void Camera1::OnBnClickedButtonSnap1()
{
	// TODO: 在此添加控件通知处理程序代码
	using namespace Halcon;
	CImageCard * ic = CImageCard::GetIns();
	CMainFrame * pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	int w = 0;
	int h = 0;
	int ch = 1;
	unsigned char * data = NULL;
	//因为电脑性能不是很好，所以采用这个snap来采取图片
	int res = Snap(w, h, &data, ch, 0, 0, pMainFrm->sys.m_nSnapTimeDelay);
	printf("error to grab image code is %d\n", res);
	if (NULL == data) return;
	Halcon::set_check("~give_error");

	int r1 = static_cast<int>(pMainFrm->ini.trect.r1);
	int c1 = static_cast<int>(pMainFrm->ini.trect.c1);
	int width = static_cast<int>(pMainFrm->ini.trect.c2) - c1;
	int height = static_cast<int>(pMainFrm->ini.trect.r2) - r1;
	cv::Rect rt(c1, r1, width, height);


	if (0 == w || 0 == h || NULL == data)	return;
	cv::Mat src;
	if (1 == ch)
		src = cv::Mat(h, w, CV_8UC1);
	else
		src = cv::Mat(h, w, CV_8UC3);
	//因为在sdk里已经有缓存了，利用缓存来弄
	src.data = data;
	CImageCard::GetIns()->mimg = src.clone();
	CImageCard::GetIns()->MatToHImage(src, CImageCard::GetIns()->Image);
	CImageCard::GetIns()->Disp_Obj(CImageCard::GetIns()->Image, CImageCard::GetIns()->disp_hd);
}


void Camera1::OnBnClickedButtonSaveasf()
{
	// TODO: 在此添加控件通知处理程序代码
	if ((GetTickCount() - global::GetIns()->SoftWareTick) > 20000)
	{
		Verity v;
		v.DoModal();
		//没更新好时间就返回
		if ((GetTickCount() - global::GetIns()->SoftWareTick) > 20000) return;
	}
	global::GetIns()->SoftWareTick = GetTickCount();
	win_tool wt;
	CString AppPath = wt.AppPath();
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();

	CString FrontsDir = AppPath + L"\\" + pMainFrm->sys.m_Prj_Name + L"\\HOLEFRONT";
	::CreateDirectory(FrontsDir, NULL);

	CString time = wt.Get_Date_Time();
	CString imagename = FrontsDir + L"\\" + time;
	set_check("~give_error");
	try {
		write_image(CImageCard::GetIns()->Image, "bmp", 0, (char*)(LPCSTR)(CStringA)imagename);
	}
	catch (HException except)
	{
		write_string(CImageCard::GetIns()->disp_hd, except.message);
	}
}


void Camera1::OnBnClickedButtonSaveasb()
{
	// TODO: 在此添加控件通知处理程序代码
	if ((GetTickCount() - global::GetIns()->SoftWareTick) > 20000)
	{
		Verity v;
		v.DoModal();
		//没更新好时间就返回
		if ((GetTickCount() - global::GetIns()->SoftWareTick) > 20000) return;
	}
	global::GetIns()->SoftWareTick = GetTickCount();
	win_tool wt;
	CString AppPath = wt.AppPath();
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();

	CString BacksDir = AppPath + L"\\" + pMainFrm->sys.m_Prj_Name + L"\\HOLEBACK";
	::CreateDirectory(BacksDir, NULL);

	CString time = wt.Get_Date_Time();
	CString imagename = BacksDir + L"\\" + time;
	set_check("~give_error");
	try {
		write_image(CImageCard::GetIns()->Image, "bmp", 0, (char*)(LPCSTR)(CStringA)imagename);
	}
	catch (HException except)
	{
		write_string(CImageCard::GetIns()->disp_hd, except.message);
	}
}


void Camera1::OnBnClickedButtonOpenf()
{
	// TODO: 在此添加控件通知处理程序代码

	win_tool wt;
	CString AppPath = wt.AppPath();
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString FrontsDir = AppPath + L"\\" + pMainFrm->sys.m_Prj_Name + L"\\HOLEFRONT";
	::CreateDirectory(FrontsDir, NULL);
	ShellExecute(NULL, _T("open"), _T("explorer.exe"), FrontsDir, NULL, SW_SHOWNORMAL);
}


void Camera1::OnBnClickedButtonOpenb()
{
	// TODO: 在此添加控件通知处理程序代码
	win_tool wt;
	CString AppPath = wt.AppPath();
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString BacksDir = AppPath + L"\\" + pMainFrm->sys.m_Prj_Name + L"\\HOLEBACK";
	::CreateDirectory(BacksDir, NULL);
	ShellExecute(NULL, _T("open"), _T("explorer.exe"), BacksDir, NULL, SW_SHOWNORMAL);
}


void Camera1::OnBnClickedButtonSaveimageincheck()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	if (pMainFrm->sys.SaveImgInCheck != 0)
	{
		pMainFrm->sys.SaveImgInCheck = 0;
		SetDlgItemText(IDC_BUTTON_SAVEIMAGEINCHECK, L"处于保存图片状态");
	}
	else
	{
		pMainFrm->sys.SaveImgInCheck = 1;
		SetDlgItemText(IDC_BUTTON_SAVEIMAGEINCHECK, L"不在保存图片状态");
	}



}
