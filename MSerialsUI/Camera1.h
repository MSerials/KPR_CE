#pragma once


// Camera1 �Ի���

class Camera1 : public CDialogEx
{
	DECLARE_DYNAMIC(Camera1)

public:
	Camera1(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Camera1();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMERA1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonDrawRect();
	afx_msg void OnBnClickedButtonDrawGetVenodr();
	afx_msg void OnBnClickedButtonReadimg1();
	afx_msg void OnBnClickedButtonDealTest();
	afx_msg void OnBnClickedButtonShowRect();
	afx_msg void OnBnClickedButtonSaveimage();
	afx_msg void OnBnClickedButtonTrainImages();
	afx_msg void OnBnClickedButtonPredict();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSnap1();
	afx_msg void OnBnClickedButtonSaveasf();
	afx_msg void OnBnClickedButtonSaveasb();
	afx_msg void OnBnClickedButtonOpenf();
	afx_msg void OnBnClickedButtonOpenb();
	afx_msg void OnBnClickedButtonSaveimageincheck();
};
