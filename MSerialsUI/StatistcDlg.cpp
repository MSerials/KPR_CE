// StatistcDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MSerialsUI.h"
#include "StatistcDlg.h"
#include "afxdialogex.h"


// CStatistcDlg �Ի���

IMPLEMENT_DYNAMIC(CStatistcDlg, CDialogEx)

CStatistcDlg::CStatistcDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DATA_LOOKUP, pParent)
{

}

CStatistcDlg::~CStatistcDlg()
{
}

void CStatistcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStatistcDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_HISTORY_DATA, &CStatistcDlg::OnBnClickedButtonHistoryData)
	ON_BN_CLICKED(IDC_BUTTON_STATISTIC, &CStatistcDlg::OnBnClickedButtonStatistic)
END_MESSAGE_MAP()


// CStatistcDlg ��Ϣ�������

void CStatistcDlg::OnBnClickedButtonStatistic()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	global::GetIns()->m_dlg_statistic->ShowWindow(SW_SHOWNORMAL);
	global::GetIns()->m_dlg_statistic->UpdateUI(global::GetIns()->m_cesus_data);
}

void CStatistcDlg::OnBnClickedButtonHistoryData()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	global::GetIns()->m_dlg_history->ShowWindow(SW_SHOWNORMAL);
}



