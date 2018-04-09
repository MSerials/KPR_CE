// CalSerials.cpp : ʵ���ļ�
//
#include "VerifyAccess.h"
#include "stdafx.h"
#include "MSerialsUI.h"
#include "CalSerials.h"
#include "SerialsVerify.h"
#include "afxdialogex.h"


// CalSerials �Ի���

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


// CalSerials ��Ϣ�������


void CalSerials::OnBnClickedButtonCal()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SerialsVerify * pw = new SerialsVerify();
	GetDlgItem(IDC_EDIT_SERIAL_T)->SetWindowText(pw->GetHDserial());
	GetDlgItem(IDC_EDIT_PWD_T)->SetWindowText(pw->get_SN());
}
