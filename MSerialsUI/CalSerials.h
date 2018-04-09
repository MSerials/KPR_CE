#pragma once


// CalSerials 对话框

class CalSerials : public CDialogEx
{
	DECLARE_DYNAMIC(CalSerials)

public:
	CalSerials(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CalSerials();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VERITY_ACCESS_TEST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCal();
};
