#pragma once


// CStaticsShowDlg 对话框

class CStaticsShowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStaticsShowDlg)

public:
	CStaticsShowDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStaticsShowDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STATICS_DATA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	void showImgStatics(int Cesus[512]);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	void UpdateUI(int Cesus[512]);
	void UpdateUI();
	
};
