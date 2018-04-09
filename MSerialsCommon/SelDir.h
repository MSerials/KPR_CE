#ifndef SELECT_FOLDER_DLG_H  
#define SELECT_FOLDER_DLG_H  

#define NO_WIN32_LEAN_AND_MEAN // 从 Windows 头中排除极少使用的资料
#include <shlobj.h>

#ifndef BIF_NEWDIALOGSTYLE  
#define  BIF_NEWDIALOGSTYLE  0x0040  
#endif  

class CSelectFolderDlg
{
public:
	//创建一个选择文件夹的对话框，返回所选路径  
	//usage   str = Show();
	CString Show()
	{
		TCHAR           szFolderPath[MAX_PATH] = { 0 };
		CString         strFolderPath = TEXT("");
		BROWSEINFO      sInfo;
		::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
		sInfo.pidlRoot = 0;
		sInfo.lpszTitle = _T("请选择一个文件夹：");
		sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;
		sInfo.lpfn = NULL;

		// 显示文件夹选择对话框  
		LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
		if (lpidlBrowse != NULL)
		{
			// 取得文件夹名  
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
			//继续遍历目录
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
				//处理图片
				//...
				strPath.push_back(strFilePath);
			}
		}
		fileFinder.Close();
		return strPath;
	}



};

#endif