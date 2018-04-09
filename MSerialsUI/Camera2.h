#pragma once


// Camera2 对话框

class Camera2 : public CDialogEx
{
	DECLARE_DYNAMIC(Camera2)

public:
	Camera2(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Camera2();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMERA2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedTrain();
	afx_msg void OnBnClickedButtonTrytrain();
	
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonBreadimg();
	afx_msg void OnBnClickedButtonBdealTest();
	afx_msg void OnBnClickedButtonBsaveimage();
	afx_msg void OnBnClickedButtonDrawBrect();
	void UpdateUI();
	afx_msg void OnBnClickedButtonShowBrect();
	afx_msg void OnBnClickedButtonSaveModel();
	afx_msg void OnBnClickedButtonReadModel();
	afx_msg void OnBnClickedButtonSnap2();
	afx_msg void OnBnClickedButtonSavePara2();
	afx_msg void OnBnClickedButtonClearModel();
	//afx_msg void OnBnClickedButtonClearModel5();
	afx_msg void OnBnClickedButtonReadModel2();
	afx_msg void OnBnClickedButtonReadModel3();
	afx_msg void OnBnClickedButtonReadModel4();
	afx_msg void OnBnClickedButtonReadModel5();
};
