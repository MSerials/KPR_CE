// Camera2.cpp : 实现文件
//

#include "stdafx.h"
#include "MSerialsUI.h"
#include "Camera2.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "halcon.h"
#include "MSerialsCam.h"


// Camera2 对话框

IMPLEMENT_DYNAMIC(Camera2, CDialogEx)

Camera2::Camera2(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CAMERA2, pParent)
{

}

Camera2::~Camera2()
{
}

void Camera2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Camera2, CDialogEx)
	ON_BN_CLICKED(IDC_TRAIN, &Camera2::OnBnClickedTrain)
	ON_BN_CLICKED(IDC_BUTTON_TRYTRAIN, &Camera2::OnBnClickedButtonTrytrain)
	ON_BN_CLICKED(IDC_BUTTON_BREADIMG, &Camera2::OnBnClickedButtonBreadimg)
	ON_BN_CLICKED(IDC_BUTTON_BDEAL_TEST, &Camera2::OnBnClickedButtonBdealTest)
	ON_BN_CLICKED(IDC_BUTTON_BSAVEIMAGE, &Camera2::OnBnClickedButtonBsaveimage)
	ON_BN_CLICKED(IDC_BUTTON_DRAW_BRECT, &Camera2::OnBnClickedButtonDrawBrect)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_BRECT, &Camera2::OnBnClickedButtonShowBrect)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_MODEL, &Camera2::OnBnClickedButtonSaveModel)
	ON_BN_CLICKED(IDC_BUTTON_READ_MODEL, &Camera2::OnBnClickedButtonReadModel)
	ON_BN_CLICKED(IDC_BUTTON_SNAP2, &Camera2::OnBnClickedButtonSnap2)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_PARA2, &Camera2::OnBnClickedButtonSavePara2)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_MODEL, &Camera2::OnBnClickedButtonClearModel)
	//ON_BN_CLICKED(IDC_BUTTON_CLEAR_MODEL5, &Camera2::OnBnClickedButtonClearModel5)
	ON_BN_CLICKED(IDC_BUTTON_READ_MODEL2, &Camera2::OnBnClickedButtonReadModel2)
	ON_BN_CLICKED(IDC_BUTTON_READ_MODEL3, &Camera2::OnBnClickedButtonReadModel3)
	ON_BN_CLICKED(IDC_BUTTON_READ_MODEL4, &Camera2::OnBnClickedButtonReadModel4)
	ON_BN_CLICKED(IDC_BUTTON_READ_MODEL5, &Camera2::OnBnClickedButtonReadModel5)
END_MESSAGE_MAP()


// Camera2 消息处理程序


void Camera2::OnBnClickedTrain()
{
	// TODO: 在此添加控件通知处理程序代码


}



void Camera2::OnBnClickedButtonTrytrain()
{
	// TODO: 在此添加控件通知处理程序代码
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

	int r1 = static_cast<int>(pMainFrm->ini.brect.r1);
	int c1 = static_cast<int>(pMainFrm->ini.brect.c1);
	int width = static_cast<int>(pMainFrm->ini.brect.c2) - c1;
	int height = static_cast<int>(pMainFrm->ini.brect.r2) - r1;
	Rect rt(c1, r1, width, height);
#ifdef _TEST
	#include <iostream>
	using namespace std;
	cout << "c1 " << c1 << endl;
	cout << "r1 " << r1 << endl;
	cout << "width " << width << endl;
	cout << "height " << height << endl;
#endif
	/*

	for (size_t i = 0; i < fdir.size(); i++)
	{
		AfxMessageBox(fdir[i]);
	}
	for (size_t i = 0; i < bdir.size(); i++)
	{
		AfxMessageBox(bdir[i]);
	}
	*/
}

//主动更新界面。。。



BOOL Camera2::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//读取参数
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	if (H_MSG_TRUE != CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, 0, 0, 0,0, 2, 0, (char*)(LPCSTR)(CStringA)pMainFrm->sys.m_Prj_Name))
	{
		AfxMessageBox(L"未能载入到匹配模板");
	}
	UpdateUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void Camera2::OnBnClickedButtonBreadimg()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fileDlg(TRUE, (LPCTSTR)"BMP", L"", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"All Files (*.*)|*.*|位图文件 (*.bmp)|*.bmp||");	//打开读取文件对话框
	if (fileDlg.DoModal() != IDOK) return;
	try {
		Halcon::set_check("~give_error");
		Halcon::read_image(&CImageCard::GetIns()->Image1, (char*)(LPCSTR)CStringA(fileDlg.GetPathName()));
		CImageCard::GetIns()->Disp_Obj(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1);
	}
	catch (...)
	{
		Halcon::set_check("~give_error");
	}
}


void Camera2::OnBnClickedButtonBdealTest()
{
	// TODO: 在此添加控件通知处理程序代码
	using namespace Halcon;
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
#ifdef NEIXINGLUN
	double r = pMainFrm->ini.bcircle.r;
	double c = pMainFrm->ini.bcircle.c;
	double radius = pMainFrm->ini.bcircle.radius;
	set_tposition(CImageCard::GetIns()->disp_hd1,0,1);
	CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, r, c, radius, 1,pMainFrm->ini.accurcy);
#endif
#ifdef CHANGER
	double r1 = pMainFrm->ini.brect.r1;
	double c1 = pMainFrm->ini.brect.c1;
	double r2 = pMainFrm->ini.brect.r2;
	double c2 = pMainFrm->ini.brect.c2;
	set_tposition(CImageCard::GetIns()->disp_hd1, 0, 1);
	CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, r1, c1, r2,c2, 1, pMainFrm->ini.accurcy);
#endif
}


void Camera2::OnBnClickedButtonBsaveimage()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	using namespace Halcon;
	win_tool wt;
	CString time = wt.Get_Date_Time();
	CString imagename = pMainFrm->sys.m_Prj_Name + L"\\" + time;
	try {
		write_image(CImageCard::GetIns()->Image1, "bmp", 0, (char*)(LPCSTR)(CStringA)imagename);
	}
	catch (HException except)
	{
		write_string(CImageCard::GetIns()->disp_hd1, except.message);
	}
}


void Camera2::OnBnClickedButtonDrawBrect()
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
		ic->RectInfo = CImageCard::BRECT;
#ifdef NEIXINGLUN
		double row, col,radius;
		Hobject ROI;
		Halcon::set_color(ic->disp_hd1, "green");
		set_draw(ic->disp_hd1, "margin");
		draw_circle(ic->disp_hd1, &row, &col, &radius);
		gen_circle(&ROI, row, col, radius);
		disp_obj(ROI, ic->disp_hd1);
		pMainFrm->ini.bcircle.r = row;
		pMainFrm->ini.bcircle.c = col;
		pMainFrm->ini.bcircle.radius = radius;
#endif

#ifdef CHANGER
		double row, col, row2, col2;
		Hobject ROI;
		Halcon::set_color(ic->disp_hd1, "green");
		set_draw(ic->disp_hd1, "margin");
		draw_rectangle1(ic->disp_hd1, &row, &col, &row2, &col2);
		gen_rectangle1(&ROI, row, col, row2, col2);
		disp_obj(ROI, ic->disp_hd1);
		pMainFrm->ini.brect.c1 = col;
		pMainFrm->ini.brect.r1 = row;
		pMainFrm->ini.brect.c2 = col2;
		pMainFrm->ini.brect.r2 = row2;
#endif
		pMainFrm->ini.SaveParaFile(PARA_IMAGE);
		ic->RectInfo = CImageCard::NO_RECT;
	}
	catch (HException except)
	{
		write_string(ic->disp_hd1, except.message);
		ic->RectInfo = CImageCard::NO_RECT;
		AfxMessageBox(L"设定搜索范围失败！");
	}

}

void Camera2::UpdateUI()
{
	int num = CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, 0, 0, 0,0, 4, 0, "", 0);
	CString str;
	str.Format(L"当前模板数量：%d", num);
	SetDlgItemText(IDC_STATIC_MODEL_NUM,str);
}

void Camera2::OnBnClickedButtonShowBrect()
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
		clear_window(ic->disp_hd1);
		CImageCard::GetIns()->Disp_Obj(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1);
		Hobject ROI;
		Halcon::set_color(ic->disp_hd1, "green");
		Halcon::set_draw(ic->disp_hd1, "margin");
#ifdef CHANGER
		gen_rectangle1(&ROI,
			pMainFrm->ini.brect.r1,
			pMainFrm->ini.brect.c1,
			pMainFrm->ini.brect.r2,
			pMainFrm->ini.brect.c2
		);
#endif
#ifdef NEIXINGLUN
		gen_circle(&ROI,
			pMainFrm->ini.bcircle.r,
			pMainFrm->ini.bcircle.c,
			pMainFrm->ini.bcircle.radius);
#endif
		disp_obj(ROI, ic->disp_hd1);

	}
	catch (HException except)
	{
		write_string(ic->disp_hd1, except.message);
	}
}


void Camera2::OnBnClickedButtonSaveModel()
{
	// TODO: 在此添加控件通知处理程序代码
	using namespace Halcon;
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
#ifdef NEIXINGLUN
	double r = pMainFrm->ini.bcircle.r;
	double c = pMainFrm->ini.bcircle.c;
	double radius = pMainFrm->ini.bcircle.radius;
	CImageCard::GetIns()->Disp_Obj(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1);
	CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1,r,c,radius,0,0,(char*)(LPCSTR)(CStringA)pMainFrm->sys.m_Prj_Name);
#endif 

#ifdef CHANGER
	double r1 = pMainFrm->ini.brect.r1;
	double c1 = pMainFrm->ini.brect.c1;
	double r2 = pMainFrm->ini.brect.r2;
	double c2 = pMainFrm->ini.brect.c2;
	CImageCard::GetIns()->Disp_Obj(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1);
	CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, r1, c1, r2,c2, 0, 0, (char*)(LPCSTR)(CStringA)pMainFrm->sys.m_Prj_Name);
#endif
	UpdateUI();
}





void Camera2::OnBnClickedButtonSnap2()
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
	int res = Snap(w, h, &data, ch, 0, 1, pMainFrm->sys.m_nSnapTimeDelay);
	printf("error to grab image code is %d\n", res);
	if (NULL == data) return ;
	Halcon::set_check("~give_error");
	if (0 == w || 0 == h || NULL == data)	return ;
	CImageCard::GetIns()->GenHobject(w, h, data, CImageCard::GetIns()->Image1, ch);
	//因为在sdk里已经有缓存了，利用缓存来弄
	CImageCard::GetIns()->Disp_Obj(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1);
}


void Camera2::OnBnClickedButtonSavePara2()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame * pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString str;
	GetDlgItemText(IDC_EDIT_THRESHOLD_VALUE, str);
	pMainFrm->ini.accurcy = _ttof(str);
	pMainFrm->ini.SaveParaFile(PARA_IMAGE);
}


void Camera2::OnBnClickedButtonClearModel()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
#ifdef NEIXINGLUN
	CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, 0, 0, 0, 3, 0, (char*)(LPCSTR)(CStringA)pMainFrm->sys.m_Prj_Name, 0);
#endif

#ifdef CHANGER
	CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, 0, 0, 0,0, 3, 0, (char*)(LPCSTR)(CStringA)pMainFrm->sys.m_Prj_Name, 0);
#endif
	UpdateUI();
}

void Camera2::OnBnClickedButtonReadModel()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
#ifdef NEIXINGLUN
	Herror error = CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, 0, 0, 0, 5, 0, (char*)(LPCSTR)(CStringA)pMainFrm->sys.m_Prj_Name);
#endif

#ifdef CHANGER
	Herror error = CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, 0, 0,0, 0, 5, 0, (char*)(LPCSTR)(CStringA)pMainFrm->sys.m_Prj_Name);
#endif
#ifdef _TEST
	printf("%d\n", error);
#endif 
}

void Camera2::OnBnClickedButtonReadModel2()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
#ifdef NEIXINGLUN
	CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, 0, 0, 0, 5, 0, (char*)(LPCSTR)(CStringA)pMainFrm->sys.m_Prj_Name, 1);
#endif
#ifdef CHANGER
	CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, 0, 0, 0, 0, 5, 0, (char*)(LPCSTR)(CStringA)pMainFrm->sys.m_Prj_Name, 1);
#endif
	
}


void Camera2::OnBnClickedButtonReadModel3()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
#ifdef NEIXINGLUN
	CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, 0, 0, 0, 5, 0, (char*)(LPCSTR)(CStringA)pMainFrm->sys.m_Prj_Name, 2);
#endif
#ifdef CHANGER
	CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, 0, 0, 0,0, 5, 0, (char*)(LPCSTR)(CStringA)pMainFrm->sys.m_Prj_Name, 2);
#endif
}


void Camera2::OnBnClickedButtonReadModel4()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
#ifdef NEIXINGLUN
	CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, 0, 0, 0, 5, 0, (char*)(LPCSTR)(CStringA)pMainFrm->sys.m_Prj_Name, 3);
#endif
#ifdef CHANGER
	CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, 0, 0, 0, 0, 5, 0, (char*)(LPCSTR)(CStringA)pMainFrm->sys.m_Prj_Name, 3);
#endif
}


void Camera2::OnBnClickedButtonReadModel5()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame *pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
#ifdef NEIXINGLUN
	CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, 0, 0, 0, 5, 0, (char*)(LPCSTR)(CStringA)pMainFrm->sys.m_Prj_Name, 4);
#endif
#ifdef CHANGER
	CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, 0, 0, 0, 0, 5, 0, (char*)(LPCSTR)(CStringA)pMainFrm->sys.m_Prj_Name, 4);
#endif
}
