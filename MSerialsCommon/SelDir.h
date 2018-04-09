#ifndef SELECT_FOLDER_DLG_H  
#define SELECT_FOLDER_DLG_H  

#define NO_WIN32_LEAN_AND_MEAN // �� Windows ͷ���ų�����ʹ�õ�����
#include <shlobj.h>

#ifndef BIF_NEWDIALOGSTYLE  
#define  BIF_NEWDIALOGSTYLE  0x0040  
#endif  

class CSelectFolderDlg
{
public:
	//����һ��ѡ���ļ��еĶԻ��򣬷�����ѡ·��  
	//usage   str = Show();
	CString Show()
	{
		TCHAR           szFolderPath[MAX_PATH] = { 0 };
		CString         strFolderPath = TEXT("");
		BROWSEINFO      sInfo;
		::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
		sInfo.pidlRoot = 0;
		sInfo.lpszTitle = _T("��ѡ��һ���ļ��У�");
		sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;
		sInfo.lpfn = NULL;

		// ��ʾ�ļ���ѡ��Ի���  
		LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
		if (lpidlBrowse != NULL)
		{
			// ȡ���ļ�����  
			if (::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
			{
				strFolderPath = szFolderPath;
			}
		}
		if (lpidlBrowse != NULL)
		{
			::CoTaskMemFree(lpidlBrowse);
		}

		return strFolderPath;

	}

	//usage std::vector<CString> = Get_FileNames()
	std::vector<CString> Get_FileNames(CString strPicDir)
	{
		CFileFind fileFinder;
		std::vector<CString> strPath;
		if (strPicDir.Right(1) == TEXT("\\"))
		{
			int nPos = strPicDir.ReverseFind(TEXT('\\'));
			strPicDir = strPicDir.Left(nPos);
		}

		CString strPicFile = TEXT("");
		strPicFile.Format(TEXT("%s\\%s"), strPicDir, TEXT("*.*"));

		BOOL bWorking = fileFinder.FindFile(strPicFile);
		while (bWorking)
		{
			bWorking = fileFinder.FindNextFile();
			if (fileFinder.IsDots())
			{
				continue;
			}

			CString strFilePath = fileFinder.GetFilePath();
			//		if (fileFinder.IsDirectory())
			//		{
			//��������Ŀ¼
			//			ProcPicDir(strFilePath);
			//		}
			//		else
			//		{
			int nPos = strFilePath.ReverseFind(TEXT('.'));
			CString strExt = strFilePath.Right(strFilePath.GetLength() - nPos - 1);
			if (strExt.CompareNoCase(TEXT("jpg")) == 0 ||
				strExt.CompareNoCase(TEXT("jpeg")) == 0 ||
				strExt.CompareNoCase(TEXT("bmp")) == 0	||
				strExt.CompareNoCase(TEXT("png")) == 0
				)
			{
				//����ͼƬ
				//...
				strPath.push_back(strFilePath);
			}
		}
		fileFinder.Close();
		return strPath;
	}



};

#endif