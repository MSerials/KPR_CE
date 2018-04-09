// CalSerials.cpp : 实现文件
//
#include "VerifyAccess.h"
#include "stdafx.h"
#include "MSerialsUI.h"
#include "CalSerials.h"
#include "SerialsVerify.h"
#include "afxdialogex.h"


// CalSerials 对话框

IMPLEMENT_DYNAMIC(CalSerials, CDialogEx)

CalSerials::CalSerials(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VERITY_ACCESS_TEST, pParent)
{

}

CalSerials::~CalSerials()
{
}

void CalSerials::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CalSerials, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CAL, &CalSerials::OnBnClickedButtonCal)
END_MESSAGE_MAP()


// CalSerials 消息处理程序


void CalSerials::OnBnClickedButtonCal()
{
	// TODO: 在此添加控件通知处理程序代码
	SerialsVerify * pw = new SerialsVerify();
	GetDlgItem(IDC_EDIT_SERIAL_T)->SetWindowText(pw->GetHDserial());
	GetDlgItem(IDC_EDIT_PWD_T)->SetWindowText(pw->get_SN());
}
