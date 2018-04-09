/*
**	文件名			CSerialPort.h
**  created  by lipeng 2010-9-12  in GW Corperation
**  functions :此类主要使用 windows的底层API函数来对串口进行了封装，用于串口通信，非阻塞，
**  可选择使用消息机制或直接访问的方式和应用程序交互，对用用户只需要实现用户自定义的消息和调用串口类的
**  public 方法来对串口操作
*/

#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__
#include "stdafx.h"
#pragma  warning(disable: 4996)
/*如下为用户自定义的消息类型 */
#define WM_COMM_BREAK_DETECTED		WM_USER+1	// 
#define WM_COMM_CTS_DETECTED		WM_USER+2	// 
#define WM_COMM_DSR_DETECTED		WM_USER+3	// 
#define WM_COMM_ERR_DETECTED		WM_USER+4	// 
#define WM_COMM_RING_DETECTED		WM_USER+5	//
#define WM_COMM_RLSD_DETECTED		WM_USER+6	// 
#define WM_COMM_RXCHAR				WM_USER+7	//
#define WM_COMM_RXFLAG_DETECTED		WM_USER+8	// 
#define WM_COMM_TXEMPTY_DETECTED	WM_USER+9	// 


class CSerialPort
{														 
public:
	CSerialPort();
	virtual		~CSerialPort();

	/*串口初始化*/										
	BOOL		InitPort(CWnd* pPortOwner,               //the owner (CWnd) of the port (receives message)
						 const char* portn, //const char* portnr="COM1",      //portnumber (1..4)
						 UINT baud=9600,                 //baudrate
					  	 const char* parity="NONE",      //parity
						 UINT databits=8,                //databits
						 const char* stopsbits="1",      //stopbits
						 DWORD dwCommEvents=EV_RXCHAR,  //EV_RXCHAR, EV_CTS etc
						 UINT nWriteBufferSize=2048,      //size to the writebuffer
						 UINT nReadBufferSize=2048);      //size to the readbuffer
    
	
	BOOL		StartMonitoring();   /*开始串口监视 */
	BOOL		RestartMonitoring(); /*从新开始串口监视*/
	BOOL		StopMonitoring();    /*停止串口监视*/
	
	DWORD		GetWriteBufferSize();/*获得写缓冲区长度*/
	DWORD       GetReadBufferSize(); /*获得用户自定义的接收数据缓冲区的大小*/
	BOOL        GetReadBufferData(BYTE *ch,DWORD length);  /*获得接收缓冲区的数据*/
	DWORD		GetMonitorEvents();	 /*获得事件*/
	DWORD		GetCommPort();		 /*获得端口号*/
	DCB			GetDCB();			 /*获得dcb结构体*/
	HANDLE		GetCommHandle();	 /*获得串口操作句柄*/
    CWnd*       GetWindowOwner();    /*获得串口的窗口拥有者*/

	BOOL        SetBaudRate(DWORD rate);  /*设置波特率*/
	BOOL        SetParity(const char* ch);      /*设置奇偶校验*/
	BOOL        SetStopbit(const char* ch);     /*设置停止位*/
	BOOL        SetDatabit(BYTE bits);    /*设置数据位*/
	BOOL        SetWriteBufSize(DWORD len);   /*设置缓冲区的大小*/
	BOOL		SetReadBufSize(DWORD length);
	BOOL		SetMonitorEvent(DWORD);   /*设置串口监视事件*/
	BOOL		ClearReadBuffer();		//清除读缓冲区
	void        EnableSerialMessage(BOOL EnRxchar=FALSE,BOOL EnCts=FALSE,BOOL EnRxFlag=FALSE,\
									BOOL EnBreak=FALSE,BOOL EnErr=FALSE,BOOL EnRing=FALSE);
	void        Close();				  /*关闭端口 并结束线程*/
  
	static UINT	CommThreadProc(LPVOID pParam); /*线程处理函数*/
	BOOL		WriteToPort(char* string,DWORD length);/*向串口写消息*/

	BYTE*				szReadBuffer;     /*读缓冲区*/
	HANDLE				hHandle;

protected:
	// protected memberfunctions
	void			ProcessErrorMessage(char* ErrorText);
	static void	    ReceiveChar(CSerialPort* port);
    
	HANDLE				m_Thread;                     //线程句柄

	CRITICAL_SECTION	m_csCommunicationSync;		  //定义了临界区
	BOOL				m_bThreadAlive;

	HANDLE				m_hComm;            //操作Comm口的句柄
	COMMTIMEOUTS		m_CommTimeouts;		//超时信息设置
	DCB					m_dcb;
	CWnd*				m_pOwner;          // owner window
    OVERLAPPED			m_ovWrite;		   //声明写重叠结构体
	OVERLAPPED          m_ovRead;          //读事件
	OVERLAPPED			m_ovCommEv;        //串口等待事件





private:
	UINT				m_nPortNr;          /*通信的端口号*/
	char*				m_szWriteBuffer;    /*写缓冲区*/
	DWORD				m_dwCommEvents;     /*对通信设备监视的事件类型*/
	DWORD				m_nWriteBufferSize; /*写缓冲区长度*/

	DWORD				m_dwReadCount;       /*读指针*/
	BYTE*				m_szReadBuffer;     /*读缓冲区*/
	DWORD				m_nReadBufferSize;  //读缓冲区大小

	BOOL				m_bWriteIsAlive;    //正在写标志；
	BOOL				m_bReadIsAlive;     //正在读标志 
/*************************************************************
** 一下为用户定义的串口消息，当设为TRUE时表示，监视线程将向此类的窗口拥有着发送这些串口消息，
** 此时用户将在窗口中实现自定义的窗口消息映射，否则当初始化串口监视这些消息时，程序会出错
** 在默认状况下，这些消息将被禁止，可通过函数EnableSerialMessage()函数来使能或禁止这些消息
***************************************************************/
private:
	BOOL m_evRxchar;
	BOOL m_evCts;
	BOOL m_evRxFlag;
	BOOL m_evBreak;
	BOOL m_evErr;
	BOOL m_evRing;

private:
	UINT parityAnalyze(const char* ch);
	UINT stopbitAnalyze(const char* ch);
	BOOL userBuildCommDCB(DCB* dcb,UINT baud,UINT parity,UINT stopbit,UINT databit);/*用户自定义建立DCB表*/




};

#endif __SERIALPORT_H__


