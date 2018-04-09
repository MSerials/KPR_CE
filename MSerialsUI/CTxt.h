//Ctxt.h

#include "stdafx.h"
#include <vector>
#include <map>
//using namespace std;

#ifndef __TXT_EDIT__
#define __TXT_EDIT__


#define MAX_PATH 260

enum{
	RESERVED,TOTAL,RDAJI, RLIANJIEPIAN, RDAOKOU, RZIFU, RSHUANGDAOLI, RSIJIE1, RSIJIE2,	RSIJIE3, RTDAJI, RTLIANJIEPIAN, RTDAOKOU, RTZIFU, RTSHUANGDAOLI, RTSIJIE, RBDAJI, RBLIANJIEPIAN, RBDAOKOU, RBZIFU, RBSHUANGDAOLI, RBSIJIE,QUIT,SAVE,XLSVISABLE,FIX,CLOSE,MYDEFINE
	}; 

class CTxt
{
	public:
	CTxt();
	CTxt(CString FilePath);
	~CTxt();

	void Save();
	
	CStdioFile recordFile;
	private:
	CString m_strTxtPath;
	long TickStart;
	long TickEnd;
	CString GetProgramPath();
	CString GetCurrentDate();
	CString getCurrentTime();
	CString GetProgramRunTime();
//	BOOL CreateMultipleDirectory(CString szPath);
	int GetLastLine();
	int m_lastline;
	int DefectiveRecord[RBSIJIE+1];
	CString strDefRec[RBSIJIE+1];
	public:
	void WriteContent(int ID);
	void WriteContent(int ID, CString str);
	void WriteContent(int ID, int ChainNum);
	void WriteContent(CString str);
	int TotalCheck;//记录了今天检测了多少链条
	
		
	public:
		BOOL SetCellValue(int row, int col, CString value);
		CString GetCellValue(int row, int col);
		CFileException fileException;
		//CFile TxtFile;
		CStdioFile TxtFile;
		void CreateTxtFile();
		BOOL ReadTxtFile();
		BOOL OpenFile();
		BOOL SetLineValue(CString str, int line);
		CString error;
		CString m_CurrentDate;
		CString FileName1;
		CString FileName;
		CString path;
	BOOL ReadFile();
	BOOL ReadFile(CString FileName);
	void OpenTxtFile();
	
	
	

	BOOL Init();
	void FirstWrite();
	
	CString m_strBuffer;
};

//extern CTxt *txt;

#endif