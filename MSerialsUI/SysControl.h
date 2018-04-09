#pragma once


// SysControl 对话框

class SysControl : public CDialogEx
{
	DECLARE_DYNAMIC(SysControl)

public:
	SysControl(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SysControl();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYSCTRL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonComfirm();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonLoadpic();
	afx_msg void OnBnClickedButtonReloadscript();
};
