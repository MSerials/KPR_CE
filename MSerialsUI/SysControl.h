#pragma once


// SysControl �Ի���

class SysControl : public CDialogEx
{
	DECLARE_DYNAMIC(SysControl)

public:
	SysControl(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~SysControl();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYSCTRL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonComfirm();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonLoadpic();
	afx_msg void OnBnClickedButtonReloadscript();
};
