// Preferences.h: interface for the CPreferences class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREFERENCES_H__4133CB8C_D2DA_4B24_9A3E_66C33ABA291E__INCLUDED_)
#define AFX_PREFERENCES_H__4133CB8C_D2DA_4B24_9A3E_66C33ABA291E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4996)

#include <afxcoll.h>  // for CStringList
#include <afxtempl.h>
#include <iostream>
#include <vector>
using namespace std;


//string encoding parameters
#define C1            52845
#define C2            22719
#define KEY           1315
#define SHIFT         120


//ini macros
#define ITEM_ROW       8
#define ITEM_COL       10
//ini macros

#define PARA_ALL            0xffffffff
#define PARA_PRJ            (0x1<<0)
#define PARA_IMAGE			(0x1<<1)
#define PARA_IO				(0x1<<2)
#define PARA_SN				(0x1<<3)
class CPreferences  
{
public:
	CPreferences();
	virtual ~CPreferences();
	
//***********************************************************************************************************
	//all private variables
private:
	//stores pathname of ini file to read/write
	CString path;
	CString dir;
	//all keys are of this time
	struct key
	{
		//list of values in key
		CArray<CString, CString> values; 
		//corresponding list of value names
		CArray<CString, CString> names;
	};
	//list of keys in ini
	CArray<key, key> keys; 
	//corresponding list of key names
	CArray<CString, CString> names; 		
	//all private functions
private:
	//overloaded to take CString
	wistream & getline( wistream & is, CString & str );
	//returns index of specified value, in the specified key, or -1 if not found
	int FindValue(int keynum, CString valuename);
	//returns index of specified key, or -1 if not found
	int FindKey(CString keyname);
	//public variables
public:
	//will contain error info if one occurs
	//ended up not using much, just in ReadFile and GetValue
	CString error;
	//public functions
public:
	//***************************************
	bool SetIniDir(CString filedir, bool bmodule=true);
	void ToUnicode(CString & str);
	int CharToUnicode(char * pchIn, CString * pstrOut);
	void CreateIniFile(CString fileName);
	void DeleteIniFile(CString fileName);
	void CopyIniFile(CString srcName, CString dstName);
	
	int SetIniFile(CString fileName);
	void LoadParaFile(UINT item);                    //item=IMAGE|LOAD|PRJ.....
	void SaveParaFile(UINT item);	
	
	CString GetAllFileName(CString suffix,int &nCnt,TCHAR separ='|');//such as: suffix=".ini" separ='|'
	CString SplitString(CString srcStr, int nIndex, TCHAR separ ='|');
	
	CString GetModulePath();
	CString GetFilePath();
	CString GetFileDir();
	bool    CreateMultipleDirectory(CString szPath);
	int     SelFolder(CWnd *pWnd, CString &strFolder);
	void    OpenIniFile();
	
	CString Encrypt(CString S, WORD Key=KEY); // 加密函数
	CString Decrypt(CString S, WORD Key=KEY); // 解密函数
	
	//gets value of [keyname] valuename = 
	//overloaded to return CString, int, and double,
	//returns "", or 0 if key/value not found.  Sets error member to show problem
	CString GetValue(CString keyname, CString valuename); 
	int		GetValueI(CString keyname, CString valuename); 
	double  GetValueF(CString keyname, CString valuename);
	bool	GetValuebool(CString keyname,CString valuename);
	CString GetValueStr(CString keyname, CString valuename);
	//sets value of [keyname] valuename =.
	//specify the optional parameter as false (0) if you do not want it to create
	//the key if it doesn't exist. Returns true if data entered, false otherwise
	//overloaded to accept CString, int, and double
	bool SetValue(CString key, CString valuename, CString value, bool create = 1);
	bool SetValueI(CString key, CString valuename, int value, bool create = 1);
	bool SetValueF(CString key, CString valuename, double value, bool create = 1);
	bool SetValuebool(CString keyname,CString valuename,bool value,bool create=1);
	bool SetValueStr(CString keyname, CString valuename,CString value,bool create=1);
	//returns true if successful, false otherwise
	bool ReadFile();
	//writes data stored in class to ini file
	void WriteFile(); 
	//deletes all stored ini data
	void Reset();
	//returns number of keys currently in the ini
	int GetNumKeys();
	//returns number of values stored for specified key
	int GetNumValues(CString keyname);
	
	//deletes specified value
	//returns true if value existed and deleted, false otherwise
	bool DeleteValue(CString keyname, CString valuename);
	//deletes specified key and all values contained within
	//returns true if key existed and deleted, false otherwise
	bool DeleteKey(CString keyname);

	//For Project Parameter list
	//Image parameter
	//Parameter

	void initData();

	//IMAGE #define PARA_IMAGE			(0x1<<1)
	struct _Rect
	{
		double c1;
		double r1;
		double c2;
		double r2;
	};

	struct _Circle
	{
		//
		double r;
		double c;
		double radius;
	};
	//相机的两个搜索框
	_Rect trect, brect;
	_Circle tcircle, bcircle;
	double accurcy;

	//#define PARA_PRJ            (0x1<<0)
	CString m_Prj_Vector;
	CString m_Prj_Name;
	CString m_COM;
	int m_Prj_Index;
	int m_nESCPixel;


	int m_nSnapTimeDelay;
	double m_nExposure;
	unsigned int m_markdelay;		//软件打标延迟
	double m_shutter;				//相机曝光时间
	CString m_password;			//密码

	int m_nTotal, m_nUp, m_nDown, m_nLeft, m_nRight, m_nVoid;	//统计上下左右检测到的数目和没有物品的数目	


	//IO卡 #define PARA_IO				(0x1<<2)

	
	int m_mode1;








	struct _ImgPara
	{
		double thresholdvalue;
		double minarea, maxarea;

		double knife_threshold;
		double minlen, maxlen;

		//字符匹配精度
		double match_acc;
	
	}TopPara,BottomPara;

};
//extern CPreferences *ini;
#endif // !defined(AFX_PREFERENCES_H__4133CB8C_D2DA_4B24_9A3E_66C33ABA291E__INCLUDED_)
