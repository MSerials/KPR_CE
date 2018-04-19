// VerifyAccess.cpp : 实现文件
//

#include "stdafx.h"
#include "MSerialsUI.h"
#include "VerifyAccess.h"
#include "afxdialogex.h"
#include "SerialsVerify.h"

// VerifyAccess 对话框

IMPLEMENT_DYNAMIC(VerifyAccess, CDialogEx)

VerifyAccess::VerifyAccess(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VERITY_ACCESS, pParent)
{
	pw = new SerialsVerify();
}

VerifyAccess::~VerifyAccess()
{
}

void VerifyAccess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VerifyAccess, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &VerifyAccess::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, &VerifyAccess::OnBnClickedButtonOk)
END_MESSAGE_MAP()


// VerifyAccess 消息处理程序


void VerifyAccess::OnBnClickedButtonCancel()
{
	// TODO: 在此添加控件通知处理程序代码

	if (0 != pw->isSNOK())
	{
		exit(0);
	}
}


void VerifyAccess::OnBnClickedButtonOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	GetDlgItem(IDC_EDIT_PWD)->GetWindowText(str);
	LPCTSTR tmp = (LPCTSTR)str;
	pw->RegSerial(tmp);

	if (0 != pw->isSNOK())
	{
		AfxMessageBox(L"注册码错误！");
	}
	else
	{
		delete pw;
		EndDialog(0);
	}
	
}


BOOL VerifyAccess::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	GetDlgItem(IDC_EDIT_SERIAL)->SetWindowText(pw->GetHDserial());
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
