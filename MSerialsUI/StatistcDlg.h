#pragma once


// CStatistcDlg �Ի���

class CStatistcDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStatistcDlg)

public:
	CStatistcDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStatistcDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DATA_LOOKUP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonHistoryData();
	afx_msg void OnBnClickedButtonStatistic();
};
