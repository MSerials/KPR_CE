#pragma once


// CStaticsShowDlg �Ի���

class CStaticsShowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStaticsShowDlg)

public:
	CStaticsShowDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStaticsShowDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STATICS_DATA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	void showImgStatics(int Cesus[512]);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	void UpdateUI(int Cesus[512]);
	void UpdateUI();
	
};
