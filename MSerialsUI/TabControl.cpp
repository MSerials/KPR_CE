// TabControl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MSerialsUI.h"
#include "TabControl.h"


// CTabControl

IMPLEMENT_DYNAMIC(CTabControl, CTabCtrl)

CTabControl::CTabControl()
{

}

CTabControl::~CTabControl()
{
}


BEGIN_MESSAGE_MAP(CTabControl, CTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, &CTabControl::OnTcnSelchange)
END_MESSAGE_MAP()



// CTabControl ��Ϣ�������




void CTabControl::OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	switch (GetCurSel())
	{
	case 0:
		m_r232.ShowWindow(SW_SHOW);
		m_c1.ShowWindow(SW_HIDE);
		m_c2.ShowWindow(SW_HIDE);
		m_StaticDlg.ShowWindow(SW_HIDE);
		break;
	case 1:
		m_r232.ShowWindow(SW_HIDE);
		m_c1.ShowWindow(SW_SHOW);
		m_c2.ShowWindow(SW_HIDE);
		m_StaticDlg.ShowWindow(SW_HIDE);
		break;
	case 2:
		m_r232.ShowWindow(SW_HIDE);
		m_c1.ShowWindow(SW_HIDE);
		m_c2.ShowWindow(SW_SHOW);	
		m_StaticDlg.ShowWindow(SW_HIDE);
		break;
	case 3:
		m_r232.ShowWindow(SW_HIDE);
		m_c1.ShowWindow(SW_HIDE);
		m_c2.ShowWindow(SW_HIDE);
		m_StaticDlg.ShowWindow(SW_SHOW);

	default:
		break;
	}
	*pResult = 0;
}

int CTabControl::init()
{
	InsertItem(0, _T("��Ŀ"));
	InsertItem(1, _T("���1"));
	InsertItem(2, _T("���2"));
	InsertItem(3, _T("����"));
	CRect rt;
	GetClientRect(&rt);
	rt.top += 25;
	rt.bottom -= 1;
	rt.left += 1;
	rt.right -= 1;
	CTabControl* t = this;

	m_r232.Create(IDD_RS232, t);
	m_c1.Create(IDD_CAMERA1, t);
	m_c2.Create(IDD_CAMERA2, t);
	m_StaticDlg.Create(IDD_DATA_LOOKUP,t);

	m_r232.MoveWindow(rt);
	m_c1.MoveWindow(rt);
	m_c2.MoveWindow(rt);
	m_StaticDlg.MoveWindow(rt);
	
	m_r232.ShowWindow(SW_SHOW);
	SetCurSel(0);
	return 0;
}
