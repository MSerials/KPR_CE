

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
4.���ú���ʱ��ֱ��          txt->WriteContent(ID);
*/


//������cmainframe�������ʼ����һ��
//��������Ҫdelete CTxt Ŀ����Ϊ�˱���
//ÿ��ѡ����ĿҲ��Ҫdeleteһ���ڹ����±���λ��

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
	case RTDAJI:			recordFile.WriteString(str+",���,����\n");			break;
	case RTLIANJIEPIAN:		recordFile.WriteString(str+",����Ƭװ��,����\n");	break;
	case RTDAOKOU:			recordFile.WriteString(str+",����,����\n");			break;
	case RTZIFU:			recordFile.WriteString(str+",����,����\n");			break;
	case RTSHUANGDAOLI:		recordFile.WriteString(str+",˫����,����\n");		break;
	case RSIJIE1:			recordFile.WriteString(str+",�����,����\n");		break;
	case RSIJIE2:			recordFile.WriteString(str+",�����,����\n");		break;
	case RBDAJI:			recordFile.WriteString(str+",���,�ײ�\n");			break;
	case RBLIANJIEPIAN:		recordFile.WriteString(str+",����Ƭװ��,�ײ�\n");	break;
	case RBDAOKOU:			recordFile.WriteString(str+",����,�ײ�\n");			break;
	case RBZIFU:			recordFile.WriteString(str+",����,�ײ�\n");			break;
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
	case RTDAJI:			recordFile.WriteString(tstr+",���,����\n");		break;
	case RTLIANJIEPIAN:		recordFile.WriteString(tstr+",����Ƭװ��" + str + ",����" + "\n");	break;
	case RTDAOKOU:			recordFile.WriteString(tstr+",����,����\n");		break;
	case RTZIFU:			recordFile.WriteString(tstr+",����,����\n");	break;
	case RTSHUANGDAOLI:		recordFile.WriteString(tstr+",˫����,����\n");		break;
	case RSIJIE1:			recordFile.WriteString(tstr+",�����,����\n");		break;
	case RSIJIE2:			recordFile.WriteString(tstr+",�����,����\n");		break;
	case RSIJIE3:			recordFile.WriteString(tstr+",��������,����\n");		break;
	case RBDAJI:			recordFile.WriteString(tstr+",���,�ײ�\n");		break;
	case RBLIANJIEPIAN:		recordFile.WriteString(tstr+",����Ƭװ��" + str + ",�ײ�" + "\n");	break;
	case RBDAOKOU:			recordFile.WriteString(tstr+",����,�ײ�\n");		break;
	case RBZIFU:			recordFile.WriteString(tstr+",����,�ײ�\n");		break;
	case CLOSE :            recordFile.WriteString(tstr+",����ر�,*****,\n");		break;
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
	case RTDAJI:			recordFile.WriteString(tstr+",���,����\n");		break;
	case RTLIANJIEPIAN:		recordFile.WriteString(tstr+",����Ƭװ��" + ",����" + "\n");	break;
	case RTDAOKOU:			recordFile.WriteString(tstr+",����,����\n");		break;
	case RTZIFU:			recordFile.WriteString(tstr+",����,����\n");	break;
	case RTSHUANGDAOLI:		recordFile.WriteString(tstr+",˫����,����\n");		break;
	case RSIJIE1:			recordFile.WriteString(tstr+",�����,����\n");		break;
	case RSIJIE2:			recordFile.WriteString(tstr+",�����,����\n");		break;
	case RBDAJI:			recordFile.WriteString(tstr+",���,�ײ�\n");		break;
	case RBLIANJIEPIAN:		recordFile.WriteString(tstr+",����Ƭװ��" + ",�ײ�" + "\n");	break;
	case RBDAOKOU:			recordFile.WriteString(tstr+",����,�ײ�\n");		break;
	case RBZIFU:			recordFile.WriteString(tstr+",����,�ײ�\n");		break;
	case CLOSE :            recordFile.WriteString(tstr+",����ر�,�������,"+nstr+"��"+"\n");		break;
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


//���ļ�
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
		
		CString str = _T("���,���ʱ��,���ϸ�����,�����ײ�\n"); //ͳ��:,����,0,���,0,��������Ƭװ��,0,��������δĥ,0,�������,0,�ײ�����Ƭװ��,0,�ײ�����δĥ,0,����,0,˫����,0,����,0\n
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
//�ҵ���¼�ļ������һ��
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

//��ȡ��������
CString CTxt::GetCurrentDate()
{
	CString str;
    CTime time;
    time=CTime::GetCurrentTime();				//��ȡ��ǰʱ������
    str=time.Format(_T("%Y-%m-%d"));			//��ʽ������ʱ��
	return str;
}

//��ȡ��ǰʱ��
CString CTxt::getCurrentTime()
{
	CString str;
    CTime time;
    time=CTime::GetCurrentTime();             //��ȡ��ǰʱ������
    str=time.Format(_T("%H:%M:%S"));   //��ʽ������ʱ��
	return str;
}

//��ȡ�����Ѿ����е�ʱ��
CString CTxt::GetProgramRunTime()
{
	CString str;
	TickEnd = GetTickCount();
	str.Format(L"%d",TickEnd-TickStart);
	return str;
}

//��ȡ��ǰ����·����
CString CTxt::GetProgramPath()
{
	TCHAR szModulePath[MAX_PATH] = {0};
	CString strCurDir;
	::GetModuleFileName(NULL,szModulePath,_MAX_PATH);
	strCurDir  = szModulePath;
	strCurDir  = strCurDir.Left(strCurDir.ReverseFind(TEXT('\\')));
	return strCurDir;
}
