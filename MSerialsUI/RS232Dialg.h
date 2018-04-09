#pragma once
#include "SerialPort.h"
#include <mutex>

// RS232Dialg 对话框

class  RS232Dialg : public CDialogEx
{
	DECLARE_DYNAMIC(RS232Dialg)
#define BUFFSIZE	513
public:
	RS232Dialg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~RS232Dialg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RS232 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()	
	afx_msg void OnBnClickedButtonComfirm();
	afx_msg LRESULT OnCommunication(WPARAM _ch, LPARAM port);
	afx_msg void OnBnClickedButtonCloseport();

	std::mutex m_mtx;
	void SetTextColor(int sel = 0);
	void InitPort(const char* str);
	int firstProcedure();
	int secondProcedure();
public:
	bool is_serial_open;
	CSerialPort m_serialport;
	
	char msg[BUFFSIZE];// = { 0 };
	char cmd[BUFFSIZE];///<控制命令
	void OnBnClickedButtonSave();
	
	virtual BOOL OnInitDialog();
	static UINT DelayInit(LPVOID lp);
	static UINT StartDeal(LPVOID lp);
	UINT Parse(const char * str, int _strlen);
	bool isParse;
	int Procedure(int camera_num);
	void parse(const char * str);
	///<图像处理流程
	afx_msg void OnBnClickedButtonRefreshcam();
	afx_msg void OnBnClickedButtonSaveparaprj();
	afx_msg void OnSelchangeComboPrjSel();
	void UpdateUI();
	afx_msg void OnBnClickedButtonDelPrj();
	afx_msg void OnBnClickedButtonSaveacc();
};
