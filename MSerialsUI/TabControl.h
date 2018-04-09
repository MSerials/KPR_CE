#pragma once

#include "Camera1.h"
#include "Camera2.h"
#include "RS232Dialg.h"
#include "StatistcDlg.h"

// CTabControl

class CTabControl : public CTabCtrl
{
	DECLARE_DYNAMIC(CTabControl)

public:
	CTabControl();
	virtual ~CTabControl();
	int init();


private:

	Camera1 m_c1;
	Camera2 m_c2;
	RS232Dialg m_r232;
	CStatistcDlg m_StaticDlg;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult);
	
};


