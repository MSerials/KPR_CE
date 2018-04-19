#include  <iostream>
#include "stdafx.h"
#if !defined(AFX_DMC1380_)
#define AFX_DMC1380_
#pragma once

#include "third_party\DMC1380\Dmc1380.h"

#ifdef _WIN64
#pragma comment(lib,"third_party/DMC1380/x64/Dmc1380.lib")
#else
#pragma comment(lib,"third_party/DMC1380/x86/Dmc1380.lib")
#endif

class DMC1380Card 
{
public:
	DMC1380Card() {};
	~DMC1380Card() {};

	//override
	int Init() {
		if (0  == d1000_board_init()) return 0;
		d1000_in_enable(0, 0);
		return 1;
	};
	//override
	std::string get_vendor() { return "no version"; };
	//override
	BOOL WriteOutPutBit(short BitNo, short BitData) { return (!d1000_out_bit(BitNo, BitData)); }

	DWORD ReadOutPutBit(short BitNo) { 
		return d1000_get_outbit(BitNo); 
	}
	//override
	DWORD ReadInPutBit(short BitNo) {
		return (!d1000_in_bit(BitNo));
	}

	bool WaitSensor(short Bits, DWORD TimeOut = 1000) {
		DWORD tk = GetTickCount();
		for (; d1000_in_bit(Bits);)
		{
			Sleep(1);
			if ((GetTickCount() - tk) > TimeOut)
				return false;
		}
		return true;
	}

	bool WaitSensorInv(short Bits, DWORD TimeOut = 1000) {
		DWORD tk = GetTickCount();
		for (; !d1000_in_bit(Bits);)		//�ߵ�ƽ״̬
		{
			Sleep(1);
			if ((GetTickCount() - tk) > TimeOut)
				return false;
		}
		return true;
	}

	void board_close() {
		d1000_board_close();
	};


private:

};


#endif
