#pragma once

#include "SerialsVerify.h"
// VerifyAccess 对话框

class VerifyAccess : public CDialogEx
{
	DECLARE_DYNAMIC(VerifyAccess)

public:
	VerifyAccess(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~VerifyAccess();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VERITY_ACCESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();

	SerialsVerify * pw;// = new SerialsVerify();
	virtual BOOL OnInitDialog();
};
