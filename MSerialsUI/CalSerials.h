#pragma once


// CalSerials �Ի���

class CalSerials : public CDialogEx
{
	DECLARE_DYNAMIC(CalSerials)

public:
	CalSerials(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CalSerials();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VERITY_ACCESS_TEST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCal();
};
