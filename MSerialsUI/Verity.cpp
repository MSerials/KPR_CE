// Verity.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MSerialsUI.h"
#include "Verity.h"
#include "afxdialogex.h"
#include "Global.h"


// Verity �Ի���

IMPLEMENT_DYNAMIC(Verity, CDialogEx)

Verity::Verity(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VERITY, pParent)
{

}

Verity::~Verity()
{
}

void Verity::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Verity, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_COMFIRM_SECRET, &Verity::OnBnClickedButtonComfirmSecret)
END_MESSAGE_MAP()


// Verity ��Ϣ�������


void Verity::OnBnClickedButtonComfirmSecret()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowText(str);
	if (0 != str.Compare(L"123456"))
	{
		AfxMessageBox(L"��֤����123456");
		EndDialog(0);
		return;
	}
	global::GetIns()->SoftWareTick = GetTickCount();
	EndDialog(0);
}
