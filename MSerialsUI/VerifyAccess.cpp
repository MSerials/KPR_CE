// VerifyAccess.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MSerialsUI.h"
#include "VerifyAccess.h"
#include "afxdialogex.h"
#include "SerialsVerify.h"

// VerifyAccess �Ի���

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


// VerifyAccess ��Ϣ�������


void VerifyAccess::OnBnClickedButtonCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (0 != pw->isSNOK())
	{
		exit(0);
	}
}


void VerifyAccess::OnBnClickedButtonOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItem(IDC_EDIT_PWD)->GetWindowText(str);
	LPCTSTR tmp = (LPCTSTR)str;
	pw->RegSerial(tmp);

	if (0 != pw->isSNOK())
	{
		AfxMessageBox(L"ע�������");
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

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	GetDlgItem(IDC_EDIT_SERIAL)->SetWindowText(pw->GetHDserial());
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
