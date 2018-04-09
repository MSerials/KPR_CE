// Settings.cpp : 实现文件
//

#include "stdafx.h"
#include "MSerialsUI.h"
#include "Settings.h"
#include "afxdialogex.h"


// CSettings 对话框

IMPLEMENT_DYNAMIC(CSettings, CDialogEx)

CSettings::CSettings(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SETTINGS, pParent)
{

}

CSettings::~CSettings()
{
}

void CSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSettings, CDialogEx)
END_MESSAGE_MAP()


// CSettings 消息处理程序
