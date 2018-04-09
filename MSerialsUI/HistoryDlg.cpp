// HistoryDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MSerialsUI.h"
#include "HistoryDlg.h"
#include "afxdialogex.h"


// CHistoryDlg 对话框

IMPLEMENT_DYNAMIC(CHistoryDlg, CDialogEx)

CHistoryDlg::CHistoryDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_HISTORY, pParent)
{

}

CHistoryDlg::~CHistoryDlg()
{
}

void CHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHistoryDlg, CDialogEx)
END_MESSAGE_MAP()


// CHistoryDlg 消息处理程序
