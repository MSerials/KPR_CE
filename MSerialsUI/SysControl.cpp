// SysControl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MSerialsUI.h"
#include "SysControl.h"
#include "afxdialogex.h"
#include "TensorFlow.h"
#include "Opencvprocess.h"


// SysControl �Ի���

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


// SysControl ��Ϣ�������


void SysControl::OnBnClickedButtonComfirm()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItem(IDC_STATIC_INPUTSTR)->GetWindowText(str);
	char * command = (char*)(LPCSTR)(CStringA)str;
//	VarsFunction(command);
}


BOOL SysControl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN))
	{
		OnBnClickedButtonComfirm();
		return TRUE;
	}
		        
	return CDialogEx::PreTranslateMessage(pMsg);
}


void SysControl::OnBnClickedButtonLoadpic()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
		CFileDialog fileDlg(TRUE, (LPCTSTR)"BMP", L"", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"All Files (*.*)|*.*|λͼ�ļ� (*.bmp)|*.bmp||");
		if (fileDlg.DoModal() != IDOK) return;

	try {
//		load_image_by_path((char*)(LPCSTR)(CStringA)fileDlg.GetPathName());
		return;

		//��ȡ�����ļ�
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

}
