// SysControl.cpp : 实现文件
//

#include "stdafx.h"
#include "MSerialsUI.h"
#include "SysControl.h"
#include "afxdialogex.h"
#include "TensorFlow.h"
#include "Opencvprocess.h"


// SysControl 对话框

IMPLEMENT_DYNAMIC(SysControl, CDialogEx)

SysControl::SysControl(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SYSCTRL, pParent)
{

}

SysControl::~SysControl()
{
}

void SysControl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SysControl, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_COMFIRM, &SysControl::OnBnClickedButtonComfirm)
	ON_BN_CLICKED(IDC_BUTTON_LOADPIC, &SysControl::OnBnClickedButtonLoadpic)
	ON_BN_CLICKED(IDC_BUTTON_RELOADSCRIPT, &SysControl::OnBnClickedButtonReloadscript)
END_MESSAGE_MAP()


// SysControl 消息处理程序


void SysControl::OnBnClickedButtonComfirm()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	GetDlgItem(IDC_STATIC_INPUTSTR)->GetWindowText(str);
	char * command = (char*)(LPCSTR)(CStringA)str;
//	VarsFunction(command);
}


BOOL SysControl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN))
	{
		OnBnClickedButtonComfirm();
		return TRUE;
	}
		        
	return CDialogEx::PreTranslateMessage(pMsg);
}


void SysControl::OnBnClickedButtonLoadpic()
{
	// TODO: 在此添加控件通知处理程序代码
		CFileDialog fileDlg(TRUE, (LPCTSTR)"BMP", L"", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"All Files (*.*)|*.*|位图文件 (*.bmp)|*.bmp||");
		if (fileDlg.DoModal() != IDOK) return;

	try {
//		load_image_by_path((char*)(LPCSTR)(CStringA)fileDlg.GetPathName());
		return;

		//读取磁盘文件
		cv::Mat img;
		img = cv::imread((char*)(LPCSTR)(CStringA)fileDlg.GetPathName(), -1);

		//g.opencv.ColorInvert(g.opencv.m_RawMatImg);
		COpencvprocess cvproc;
		
		cvproc.MonoInvert(img);

//		load_image(img.rows, img.cols, img.channels(), img.data);

		
	}
	catch (...)
	{

		return;
	}
}


void SysControl::OnBnClickedButtonReloadscript()
{
	// TODO: 在此添加控件通知处理程序代码

}
