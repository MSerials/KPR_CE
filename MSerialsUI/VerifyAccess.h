#pragma once

#include "SerialsVerify.h"
// VerifyAccess �Ի���

class VerifyAccess : public CDialogEx
{
	DECLARE_DYNAMIC(VerifyAccess)

public:
	VerifyAccess(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~VerifyAccess();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VERITY_ACCESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();

	SerialsVerify * pw;// = new SerialsVerify();
	virtual BOOL OnInitDialog();
};
