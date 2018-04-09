//����

//
//#include "stdafx.h"

#include <windows.h> //���ʺ�64λ����


#include <tlhelp32.h>
#include <atlstr.h>
#include  <vector>
#include "shellapi.h"



#ifndef __TOOLS__
#define __TOOLS__


class win_tool
{
public:
	win_tool() {};
	virtual ~win_tool() {};
	int GetProcessIDByName(PWSTR pname)
	{
		HANDLE SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		SHFILEINFO shSmall;
		BOOL Status = FALSE;
		PROCESSENTRY32 ProcessInfo;//����������Ϣ����
		DWORD pid = 0;
		if (SnapShot == NULL)
		{
			return -1;
		}


		ProcessInfo.dwSize = sizeof(ProcessInfo);//����ProcessInfo�Ĵ�С
												 //����ϵͳ�е�һ�����̵���Ϣ
		Status = Process32First(SnapShot, &ProcessInfo);

		while (Status)
		{
			//��ȡ�����ļ���Ϣ
			SHGetFileInfo(ProcessInfo.szExeFile, 0, &shSmall,
				sizeof(shSmall), SHGFI_ICON | SHGFI_SMALLICON);
			//���б�ؼ������ӳ������
			if (StrStrI(ProcessInfo.szExeFile, pname) != NULL)
			{
				pid = ProcessInfo.th32ProcessID;
				break;
			}
			//��ȡ��һ�����̵���Ϣ
			Status = Process32Next(SnapShot, &ProcessInfo);

		}
		return pid;
	}

	CString AppPath()
	{
		CString AppPath;
		::GetModuleFileName(GetModuleHandle(NULL), AppPath.GetBuffer(300), 300);
		AppPath.ReleaseBuffer();
		AppPath = AppPath.Left(AppPath.ReverseFind('\\'));
		return AppPath;
	}


	CString Get_Date()
	{
		SYSTEMTIME st;
		CString strDate;
		GetLocalTime(&st);
		strDate.Format(_T("%4d-%d-%d"), st.wYear, st.wMonth, st.wDay);
		return strDate;
	}

	CString Get_Time()
	{
		SYSTEMTIME st;
		CString strTime;
		GetLocalTime(&st);
		strTime.Format(_T("%d-%d-%d"), st.wHour, st.wMinute, st.wSecond);
		return  strTime;
	}

	CString Get_Date_Time()
	{
		CString str;
		SYSTEMTIME st;
		CString strDate, strTime;
		GetLocalTime(&st);
		strDate.Format(_T("%4d-%d-%d_"), st.wYear, st.wMonth, st.wDay);
		strTime.Format(_T("%d-%d-%d"), st.wHour, st.wMinute, st.wSecond);
		str = strDate + strTime;
		return str;
	}

	std::vector<CString> SplitCString(const char* strSource, CString ch)
	{
		size_t string_len = strlen(strSource) + 4;//ע��ΪʲôҪ���Ƴ���,��ΪUnicodeת������⣬�����Ƴ����������һ����β���ţ��������bug
		char *p = (char*)malloc(string_len);
		memcpy(p, strSource, string_len);
		p[string_len - 1] = '\0';//
		CString src = (CString)(CStringA)(LPCSTR)p;
		std::vector <CString> vecString;
		int iPos = 0;
		CString strTmp;
		strTmp = src.Tokenize(ch, iPos);
		while (strTmp.Trim() != _T(""))
		{
			vecString.push_back(strTmp);
			strTmp = src.Tokenize(ch, iPos);
		}
		free(p);
		return vecString;
	}

	std::vector<CString> SplitCString(CString strSource, CString ch)
	{
		std::vector <CString> vecString;
		int iPos = 0;
		CString strTmp;
		strTmp = strSource.Tokenize(ch, iPos);
		while (strTmp.Trim() != _T(""))
		{
			vecString.push_back(strTmp);
			strTmp = strSource.Tokenize(ch, iPos);
		}
		return vecString;
	}

	char* UnicodeToUtf8(CString unicode)
	{
		int len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)unicode, -1, NULL, 0, NULL, NULL);
		static char *szUtf8 = NULL;
		if (NULL != szUtf8) { delete szUtf8; szUtf8 = NULL; }
		szUtf8 = new char[len * 2 + 4];
		memset(szUtf8, 0, len * 2 + 4);
		WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)unicode, -1, szUtf8, len, NULL, NULL);
		return szUtf8;
	}


};


#endif


