#pragma once


// Verity �Ի���

class Verity : public CDialogEx
{
	DECLARE_DYNAMIC(Verity)

public:
	Verity(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Verity();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VERITY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonComfirmSecret();
};
