#pragma once


// CStatistcDlg 对话框

class CStatistcDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStatistcDlg)

public:
	CStatistcDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStatistcDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DATA_LOOKUP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonHistoryData();
	afx_msg void OnBnClickedButtonStatistic();
};
