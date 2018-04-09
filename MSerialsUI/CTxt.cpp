

#include "stdafx.h"
#include "CTxt.h"
//#include "fstream.h"
//#include "IniFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/*
1.add txt = new CTxt();		to where to initial
2.add txt->Init();	        to where to initial
3.add #include "CTxt.h"                to where to use and where to initial
4.调用函数时候直接          txt->WriteContent(ID);
*/


//我是在cmainframe构造里初始化了一下
//在析构里要delete CTxt 目的是为了保存
//每次选择项目也是要delete一下在构造下保存位置

CTxt::CTxt()
:TotalCheck(0)
{
	m_strTxtPath = GetProgramPath() + L"\\DefectiveRecord";

	TickStart = GetTickCount();

	m_CurrentDate = GetCurrentDate();

	FileName = m_strTxtPath + L"\\" + m_CurrentDate + L".csv";

	path = FileName;

	Init();
}

CTxt::CTxt(CString FilePath)
	:TotalCheck(0)
{
	m_strTxtPath = GetProgramPath() + L"\\" + FilePath + L"\\DefectiveRecord";

	TickStart = GetTickCount();
	m_CurrentDate = GetCurrentDate();

	FileName = m_strTxtPath + L"\\" + m_CurrentDate + L".csv";

	path = FileName;

	Init();
}


CTxt::~CTxt()
{
	recordFile.Flush();
	recordFile.Close();
	m_strTxtPath = "";
}

void CTxt::Save()
{
//	recordFile.WriteString(L"Save FIle\n");
	recordFile.Flush();
	recordFile.Close();
	m_strTxtPath = "";
}


void CTxt::WriteContent(int ID)
{
	CString str;
	str.Format(_T("%d,"),m_lastline );
	str = str + getCurrentTime();
	switch(ID)
	{
	case RTDAJI:			recordFile.WriteString(str+",打机,顶部\n");			break;
	case RTLIANJIEPIAN:		recordFile.WriteString(str+",连接片装反,顶部\n");	break;
	case RTDAOKOU:			recordFile.WriteString(str+",刀口,顶部\n");			break;
	case RTZIFU:			recordFile.WriteString(str+",混料,顶部\n");			break;
	case RTSHUANGDAOLI:		recordFile.WriteString(str+",双刀粒,顶部\n");		break;
	case RSIJIE1:			recordFile.WriteString(str+",死结进,顶部\n");		break;
	case RSIJIE2:			recordFile.WriteString(str+",死结出,顶部\n");		break;
	case RBDAJI:			recordFile.WriteString(str+",打机,底部\n");			break;
	case RBLIANJIEPIAN:		recordFile.WriteString(str+",连接片装反,底部\n");	break;
	case RBDAOKOU:			recordFile.WriteString(str+",刀口,底部\n");			break;
	case RBZIFU:			recordFile.WriteString(str+",混料,底部\n");			break;
	default:break;
	}

	m_lastline += 1;
}

void CTxt::WriteContent(int ID, CString str)

{
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	CString tstr;
	tstr.Format(_T("%d,"),m_lastline );
	tstr = tstr + getCurrentTime();
	switch(ID)
	{
	case RTDAJI:			recordFile.WriteString(tstr+",打机,顶部\n");		break;
	case RTLIANJIEPIAN:		recordFile.WriteString(tstr+",连接片装反" + str + ",顶部" + "\n");	break;
	case RTDAOKOU:			recordFile.WriteString(tstr+",刀口,顶部\n");		break;
	case RTZIFU:			recordFile.WriteString(tstr+",混料,顶部\n");	break;
	case RTSHUANGDAOLI:		recordFile.WriteString(tstr+",双刀粒,顶部\n");		break;
	case RSIJIE1:			recordFile.WriteString(tstr+",死结进,顶部\n");		break;
	case RSIJIE2:			recordFile.WriteString(tstr+",死结出,顶部\n");		break;
	case RSIJIE3:			recordFile.WriteString(tstr+",桌面死结,顶部\n");		break;
	case RBDAJI:			recordFile.WriteString(tstr+",打机,底部\n");		break;
	case RBLIANJIEPIAN:		recordFile.WriteString(tstr+",连接片装反" + str + ",底部" + "\n");	break;
	case RBDAOKOU:			recordFile.WriteString(tstr+",刀口,底部\n");		break;
	case RBZIFU:			recordFile.WriteString(tstr+",混料,底部\n");		break;
	case CLOSE :            recordFile.WriteString(tstr+",软件关闭,*****,\n");		break;
	case MYDEFINE:			recordFile.WriteString(tstr + "," + str + "\n");		break;
	default:break;
	}
	recordFile.Flush();
	m_lastline += 1;

	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
}

void CTxt::WriteContent(int ID, int ChainNum)
{
	CString tstr,nstr;
	tstr.Format(_T("%d,"),m_lastline );
	tstr = tstr + getCurrentTime();
	nstr.Format(L"%d",ChainNum);
	switch(ID)
	{
	case RTDAJI:			recordFile.WriteString(tstr+",打机,顶部\n");		break;
	case RTLIANJIEPIAN:		recordFile.WriteString(tstr+",连接片装反" + ",顶部" + "\n");	break;
	case RTDAOKOU:			recordFile.WriteString(tstr+",刀口,顶部\n");		break;
	case RTZIFU:			recordFile.WriteString(tstr+",混料,顶部\n");	break;
	case RTSHUANGDAOLI:		recordFile.WriteString(tstr+",双刀粒,顶部\n");		break;
	case RSIJIE1:			recordFile.WriteString(tstr+",死结进,顶部\n");		break;
	case RSIJIE2:			recordFile.WriteString(tstr+",死结出,顶部\n");		break;
	case RBDAJI:			recordFile.WriteString(tstr+",打机,底部\n");		break;
	case RBLIANJIEPIAN:		recordFile.WriteString(tstr+",连接片装反" + ",底部" + "\n");	break;
	case RBDAOKOU:			recordFile.WriteString(tstr+",刀口,底部\n");		break;
	case RBZIFU:			recordFile.WriteString(tstr+",混料,底部\n");		break;
	case CLOSE :            recordFile.WriteString(tstr+",软件关闭,检测条数,"+nstr+"条"+"\n");		break;
	default:break;
	}

	m_lastline += 1;
}


void CTxt::WriteContent(CString str)
{	
	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");
	recordFile.WriteString(str);
	m_lastline += 1;
	recordFile.Flush();
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);

}

BOOL CTxt::Init(){

	if (!OpenFile())
		return FALSE;
	return TRUE;
}


//打开文件
BOOL CTxt::OpenFile()
{
	CFileFind finder;

	CreateDirectory(m_strTxtPath, NULL);
	

	if(!finder.FindFile(path))
	{
		
#ifdef UNICODE
			recordFile.Open(path, CFile::typeBinary | CFile::modeCreate | CFile::modeReadWrite | CFile::typeUnicode);
#else
			recordFile.Open(path, CFile::typeText | CFile::modeCreate | CFile::modeReadWrite);
#endif
		
		CString str = _T("编号,检测时间,不合格种类,顶部底部\n"); //统计:,总数,0,打机,0,顶部连接片装反,0,顶部刀口未磨,0,顶部打机,0,底部连接片装反,0,底部刀口未磨,0,混料,0,双刀粒,0,死结,0\n
		WriteContent(str);
		//recordFile.Flush();
		m_lastline = 1;
		return TRUE;
	}
	else
	{
#ifdef UNICODE
		recordFile.Open(path, CFile::typeBinary | CFile::modeCreate | CFile::modeReadWrite | CFile::typeUnicode);
#else
		recordFile.Open(path, CFile::typeText | CFile::modeCreate | CFile::modeReadWrite);
#endif
		m_lastline = GetLastLine();
		m_lastline--;
		return TRUE;
	}
	return FALSE;
}
//找到记录文件的最后一行
int CTxt::GetLastLine()
{
	int line = 1;
	CString strContent;

	recordFile.SeekToBegin();

	while(recordFile.ReadString(strContent))
		line += 1;

	return line;
}

BOOL CTxt::SetLineValue(CString str, int line)
{
	recordFile.SeekToBegin();

	recordFile.WriteString(str);

	return TRUE;
}


BOOL CTxt::SetCellValue(int row, int col, CString value)
{

	return TRUE;
}




void CTxt::OpenTxtFile()
{
	::ShellExecute(NULL,L"open",path,NULL,NULL,SW_SHOW);
}

//获取当天日期
CString CTxt::GetCurrentDate()
{
	CString str;
    CTime time;
    time=CTime::GetCurrentTime();				//获取当前时间日期
    str=time.Format(_T("%Y-%m-%d"));			//格式化日期时间
	return str;
}

//获取当前时间
CString CTxt::getCurrentTime()
{
	CString str;
    CTime time;
    time=CTime::GetCurrentTime();             //获取当前时间日期
    str=time.Format(_T("%H:%M:%S"));   //格式化日期时间
	return str;
}

//获取程序已经运行的时间
CString CTxt::GetProgramRunTime()
{
	CString str;
	TickEnd = GetTickCount();
	str.Format(L"%d",TickEnd-TickStart);
	return str;
}

//获取当前程序路径名
CString CTxt::GetProgramPath()
{
	TCHAR szModulePath[MAX_PATH] = {0};
	CString strCurDir;
	::GetModuleFileName(NULL,szModulePath,_MAX_PATH);
	strCurDir  = szModulePath;
	strCurDir  = strCurDir.Left(strCurDir.ReverseFind(TEXT('\\')));
	return strCurDir;
}
