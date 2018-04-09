#pragma once


// Verity 对话框

class Verity : public CDialogEx
{
	DECLARE_DYNAMIC(Verity)

public:
	Verity(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Verity();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VERITY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonComfirmSecret();
};
