#pragma once
#ifndef AES_H_
#define AES_H_

#include <bitset>
#include <utility>
#include <iostream>

using namespace std;
namespace exmath
{
	template<typename _T>
	void swap(_T& Left, _T& Right)
	{
		_T tmp = Left;
		Left = Right;
		Right = tmp;
	}
}

#if 0



usage example

void main(int argc, char *argv[])
{
AES aes;
const unsigned char *key = new unsigned char[16];
memcpy((void*)key, "lsdkfj\0",6);


const unsigned char* plainText = new unsigned char[160];
/*memcpy*/strcpy((char*)plainText, "mybirthdaymybirthdaymybirthdaymybirthdaymybirthdaymybirthday\0");


cout << plainText << endl << endl << endl;
const unsigned char* cipherText;
cipherText = aes.Cipher(plainText, key, 6);
cout << cipherText << endl << endl << endl;



const unsigned char* plainText1;
plainText1 = aes.InVCipher(cipherText, key, 6); 
cout << plainText1 << endl << endl << endl;

delete[] key;
delete[] plainText;
}



#endif


























class AES
{
public:
	typedef unsigned char    byte;
	static const int KEY_SIZE = 16;    //    ��Կ����Ϊ128λ
	static const int N_ROUND = 11;
	byte plainText[160];    //    ����
	byte state[16];    //    ��ǰ���顣
	byte cipherKey[16];    //    ��Կ
	byte roundKey[N_ROUND][16];    //����Կ
	byte cipherText[160];    //����
	byte SBox[16][16];    //    S��
	byte InvSBox[16][16];    //    ��S��   
	void EncryptionProcess();
	void DecryptionProcess();
	void Round(const int& round);
	void InvRound(const int& round);
	void FinalRound();
	void InvFinalRound();
	void KeyExpansion();
	void AddRoundKey(const int& round);
	void SubBytes();
	void InvSubBytes();
	void ShiftRows();
	void InvShiftRows();
	void MixColumns();
	void InvMixColumns();
	void BuildSBox();
	void BuildInvSBox();
	void InitialState(const byte* text);
	void InitialCipherText();
	void InitialplainText();
	byte GFMultplyByte(const byte& left, const byte& right);
	const byte* GFMultplyBytesMatrix(const byte* left, const byte* right);
public:
	AES();
	const byte* Cipher(const byte* text, const byte* key, const int& keySize);
	const byte* InVCipher(const byte* text, const byte* key, const int& keySize);
};


#endif
//////////////////////////////AES.h/////////////////////////////////////////
