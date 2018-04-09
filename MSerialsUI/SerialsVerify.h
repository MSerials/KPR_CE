#pragma once
#include "MD5.h"
#include "AES.h"
#include "stdafx.h"
#include <windows.h>
#include <winioctl.h>

#pragma warning(disable:4996)






class GetHDSerial
{
public:
	GetHDSerial() {};
	~GetHDSerial() {};

	//usaage
	static CString GetSerials()
	{
		TCHAR szModelNo[48], szSerialNo[24];
		if (GetPhyDriveSerial(szModelNo, szSerialNo))
		{
			//TrimStart(szSerialNo);
			CString sNo(szSerialNo);
			sNo.Replace(L" ", L"");
			return sNo;
		}
		else
		{
			return L"";
		}
	}


	static BOOL GetPhyDriveSerial(LPTSTR pModelNo, LPTSTR pSerialNo)
	{
		//-1是因为 SENDCMDOUTPARAMS 的结尾是 BYTE bBuffer[1];
		BYTE IdentifyResult[sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];
		DWORD dwBytesReturned;
		GETVERSIONINPARAMS get_version;
		SENDCMDINPARAMS send_cmd = { 0 };

		HANDLE hFile = CreateFile(_T("\\\\.\\PHYSICALDRIVE0"), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return FALSE;

		//get version
		DeviceIoControl(hFile, SMART_GET_VERSION, NULL, 0,
			&get_version, sizeof(get_version), &dwBytesReturned, NULL);

		//identify device
		send_cmd.irDriveRegs.bCommandReg = (get_version.bIDEDeviceMap & 0x10) ? ATAPI_ID_CMD : ID_CMD;
		DeviceIoControl(hFile, SMART_RCV_DRIVE_DATA, &send_cmd, sizeof(SENDCMDINPARAMS) - 1,
			IdentifyResult, sizeof(IdentifyResult), &dwBytesReturned, NULL);
		CloseHandle(hFile);

		//adjust the byte order
		PUSHORT pWords = (USHORT*)(((SENDCMDOUTPARAMS*)IdentifyResult)->bBuffer);
		ToLittleEndian(pWords, 27, 46, pModelNo);
		ToLittleEndian(pWords, 10, 19, pSerialNo);
		return TRUE;
	}

	//把WORD数组调整字节序为little-endian，并滤除字符串结尾的空格。
	static void ToLittleEndian(PUSHORT pWords, int nFirstIndex, int nLastIndex, LPTSTR pBuf)
	{
		int index;
		LPTSTR pDest = pBuf;
		for (index = nFirstIndex; index <= nLastIndex; ++index)
		{
			pDest[0] = pWords[index] >> 8;
			pDest[1] = pWords[index] & 0xFF;
			pDest += 2;
		}
		*pDest = 0;

		//trim space at the endof string; 0x20: _T(' ')
		--pDest;
		while (*pDest == 0x20)
		{
			*pDest = 0;
			--pDest;
		}
	}

	//滤除字符串起始位置的空格
	static void TrimStart(LPTSTR pBuf)
	{
		if (*pBuf != 0x20)
			return;

		LPTSTR pDest = pBuf;
		LPTSTR pSrc = pBuf + 1;
		while (*pSrc == 0x20)
			++pSrc;

		while (*pSrc)
		{
			*pDest = *pSrc;
			++pDest;
			++pSrc;
		}
		*pDest = 0;
	}
};



enum
{
	NOT_OFFICIAL_INSTALL = 1, OUT_DATE = 2
};

class SerialsVerify
{
public:
	SerialsVerify() {};
	~SerialsVerify() {};

	bool CheckTrial() { return CheckBeforeUse(); }

	CString get_SN()
	{
		CString HDSerial = GetHDserial();
		MD5 md5((std::string)(CStringA)HDSerial);
		const char* md5string = md5.toString().c_str();
		return (CString)(CStringA)md5string;
	}

	int isSNOK()
	{
		CString HDSerial = GetHDserial();
		CString Serial = GetSerial();
		MD5 md5((std::string)(CStringA)HDSerial);
		const char* md5string = md5.toString().c_str();
		CString res = (CString)(CStringA)md5string;
		return res.Compare(Serial);
	};

	CString GetHDserial()
	{
		return GetHDSerial::GetSerials();
	}

	int GetDate()
	{
		ATL::CRegKey key;
		DWORD dw = 512;
		char sValue[512] = { 0 };// = L"";
		if (key.Open(HKEY_CURRENT_USER, L"Software\\ImageVision"))
		{
			key.Close();
			if (!RegDate())
				return 0; //可能没有管理员权限运行，返回一个0
			else
				return 1;//表示已经管理员权限运行了，今天才弄
		}
		else {
			key.QueryValue((LPTSTR)sValue, L"T", &dw);
			key.Close();
			CString s = (CString)sValue;
			if (s.IsEmpty())
			{
				if (!RegDate())
					return 0; //可能没有管理员权限运行，返回一个0
				else
					return 1;//表示已经管理员权限运行了，今天才弄
			}
		}

		vector<CString> strValue = Split((LPTSTR)sValue);
		if (3 != strValue.size()) return -2;//格式错误

		int nYear = StrToInt(strValue[0]);
		int nMonth = StrToInt(strValue[1]);
		int nDay = StrToInt(strValue[2]);

		CTime time = CTime::GetCurrentTime();
		int tYear = time.GetYear();
		int tMonth = time.GetMonth();
		int tDay = time.GetDay();

		long newyearday = GetThisYearDay(tYear, tMonth, tDay);
		long oldyearday = GetThisYearDay(nYear, nMonth, nDay);

		int _data = newyearday - oldyearday;
		return 1;
		//	if ((_data < 90))
		if ((_data < 90) && (_data >= 0))
			return 1; //ok
		else
			return -1;//outdate
	}


	BOOL RegSerial(LPCTSTR Reg)
	{
		//if (ERROR_SUCCESS == ::RegCreateKeyEx(hCKey, _T("testreg"), 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL, &hAKey, &dw))
		CRegKey key;
		if (key.Open(HKEY_LOCAL_MACHINE, L"HARDWARE"))
		{
			if (key.Create(HKEY_LOCAL_MACHINE, L"HARDWARE"))
				return 0;
			key.SetValue(Reg, L"Serial");
		}
		else
		{
			key.SetValue(Reg, L"Serial");
		}
		key.Close();
		return 1;

	}


private:
	GetHDSerial getHDSerial;
	wchar_t strReg[1024];
	TCHAR szModelNo[48], szSerialNo[24];
	bool CheckBeforeUse() { return (0 == CheckVersion().Compare(L"1.0.0")); }

	CString CheckVersion()
	{
		CRegKey key;
		DWORD dw = 1024;
		char sValue[512] = { 0 };// = L"";
		if (key.Open(HKEY_LOCAL_MACHINE, L"HARDWARE"))
		{
			key.Close();
			return L"";
		}
		else
		{
			memset(strReg, 0, 1024);
			key.QueryValue(strReg, L"Verion", &dw);
			key.Close();
			return (CString)strReg;
		}
	}
	public:
	CString GetSerial()
	{
		CRegKey key;
		DWORD dw = 1024;
		char sValue[512] = { 0 };// = L"";
		if (key.Open(HKEY_LOCAL_MACHINE, L"HARDWARE"))
		{
			key.Close();
			return L"";
		}
		else
		{
			memset(strReg, 0, 1024);
			key.QueryValue(strReg, L"Serial", &dw);
			key.Close();
			return (CString)strReg;
		}
	}
	private:
	CString GetAES(CString str = L"")
	{
		AES aes;
#define keylen 7
		const unsigned char *key = new unsigned char[16];
		memcpy((void*)key, "lsdkfj\0", keylen);


		const unsigned char* plainText = new unsigned char[160];
		/*memcpy*/strcpy((char*)plainText, "mybirthdaymybirthdaymybirthdaymybirthdaymybirthdaymybirthday\0");


		cout << plainText << endl << endl << endl;
		const unsigned char* cipherText;
		cipherText = aes.Cipher(plainText, key, keylen);
		cout << cipherText << endl << endl << endl;


		const unsigned char* plainText1;
		plainText1 = aes.InVCipher(cipherText, key, keylen);
		cout << plainText1 << endl << endl << endl;

		delete[] key;
		delete[] plainText;
		return L"";

	}











	BOOL RegDate()
	{
		CTime time = CTime::GetCurrentTime();
		int Year = time.GetYear();
		int Month = time.GetMonth();
		int Day = time.GetDay();
		CString str;
		str.Format(_T("%d-%d-%d"), Year, Month, Day);
		LPCTSTR l_time = (LPCTSTR)str;
		ATL::CRegKey key;
		if (key.Open(HKEY_CURRENT_USER, L"Software\\ImageVision"))
		{
			if (key.Create(HKEY_CURRENT_USER, L"Software\\ImageVision"))
				return 0;
			key.SetValue(l_time, L"T");
		}
		else
		{
			key.SetValue(l_time, L"T");
		}
		key.Close();
		return 1;
	}





	long GetThisYearDay(int year, int month, int day)
	{
		int counterOfOddYear = (year / 4);
		if (!(year % 4)) {
			counterOfOddYear--;
			long allday = (year - 1) * 365 + counterOfOddYear;
			switch (month)
			{
			case 1: return (day + allday);
			case 2: return (day + allday + 31);
			case 3: return (day + allday + 60);
			case 4: return (day + allday + 91);
			case 5: return (day + allday + 121);
			case 6: return (day + allday + 152);
			case 7: return (day + allday + 182);
			case 8: return (day + allday + 213);
			case 9: return (day + allday + 244);
			case 10: return (day + allday + 274);
			case 11: return (day + allday + 305);
			case 12: return (day + allday + 335);
			default:return 0;
			}
		}
		else
		{
			long allday = (year) * 365 + counterOfOddYear;
			switch (month)
			{
			case 1: return (day + allday);
			case 2: return (day + allday + 31);
			case 3: return (day + allday + 59);
			case 4: return (day + allday + 90);
			case 5: return (day + allday + 120);
			case 6: return (day + allday + 151);
			case 7: return (day + allday + 181);
			case 8: return (day + allday + 212);
			case 9: return (day + allday + 243);
			case 10: return (day + allday + 273);
			case 11: return (day + allday + 304);
			case 12: return (day + allday + 334);
			default:return 0;
			}

		}
	}


	vector<CString> Split(CString string)
	{
		CString oriStr = string;
		vector<CString> strVec;
		while (true)
		{
			CString n = oriStr.SpanExcluding(L"-");
			strVec.push_back(n);
			oriStr = oriStr.Right(oriStr.GetLength() - n.GetLength() - 1);
			if (oriStr.IsEmpty())
			{
				break;
			}
		}
		return strVec;
	}

};