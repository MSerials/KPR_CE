/*
**	文件名	CSerialPort.h
**  created  by lipeng 2010-9-12  in GW Corperation
**  functions :此类主要使用 windows的底层API函数来对串口进行了封装，用于串口通信，非阻塞，
**  可选择使用消息机制或直接访问的方式和应用程序交互，对用用户只需要实现用户自定义的消息和调用串口类的
**  public 方法来对串口操作
*/

#include "stdafx.h"
#include "SerialPort.h"
#include "afx.h"		//使用CString类的头文件
#include <assert.h>
//#include "RS232Dlg.h"
//#include "resource.h"
#pragma warning(disable:4995)
#pragma warning(disable:4996)
#pragma warning(disable:4477)

CSerialPort::CSerialPort()
{
	m_hComm = NULL;
    //缓冲区设为空
	m_szWriteBuffer = NULL;
    //线程创建标志为假
	m_bThreadAlive = FALSE;
    //线程句柄设为空
	m_Thread = NULL ;
    //重叠IO操作
	m_ovWrite.hEvent = NULL;
	m_ovRead.hEvent=NULL;
	m_ovCommEv.hEvent=NULL;

	m_szReadBuffer=NULL;
	m_dwReadCount=0;      /*读指针 开始设置为0*/

	m_evRxchar=FALSE;
	m_evCts=FALSE;
	m_evRxFlag=FALSE;
	m_evBreak=FALSE;
	m_evErr=FALSE;
	m_evRing=FALSE;
	m_ovWrite.Offset=0;
	m_ovWrite.OffsetHigh = 0;
	m_bWriteIsAlive=FALSE;


	//CString str[] = { _T("300"), _T("600"), _T("1200"), _T("2400"), _T("4800"), _T("9600"), _T("19200"), _T("38400"), _T("43000"), _T("56000"), _T("57600"), _T("115200") };
	//m_strWaveSelect = str;
}


/*析构函数，删除分配的内存，结束线程，关闭句柄*/
CSerialPort::~CSerialPort()
{
	if(m_bThreadAlive==TRUE)
	{
		/*结束线程*/
         DWORD dwExitCode ;
		 if( GetExitCodeThread( m_Thread, &dwExitCode ) ) 
		 {
			TerminateThread(m_Thread, dwExitCode ) ;
			CloseHandle(m_Thread);
		 }
		 m_Thread = NULL ;
		m_bThreadAlive=FALSE;
	}

	/*关闭串口设备*/
	if(m_hComm!=NULL)
	{
		CloseHandle(m_hComm);
		m_hComm=NULL;
	}

	/*使窗口句柄无效*/
	if(m_pOwner!=NULL)
	{
		m_pOwner=NULL;
	}

	/*删除发送缓冲区*/
	if (m_szWriteBuffer != NULL)
	{
		delete [] m_szWriteBuffer;
        m_szWriteBuffer=NULL;
	}
	if(m_szReadBuffer!=NULL)
	{
		delete [] m_szReadBuffer;
		m_szReadBuffer=NULL;
	}
    //关闭事件
	if(m_ovWrite.hEvent!=NULL)
	{
		ResetEvent(m_ovWrite.hEvent);
		CloseHandle(m_ovWrite.hEvent);
		m_ovWrite.hEvent=NULL;
	}
	if(m_ovRead.hEvent!=NULL)
	{
		ResetEvent(m_ovRead.hEvent);
		CloseHandle(m_ovRead.hEvent);
		m_ovRead.hEvent=NULL;
	}
	if(m_ovCommEv.hEvent!=NULL)
	{
		ResetEvent(m_ovCommEv.hEvent);
		CloseHandle(m_ovCommEv.hEvent);
		m_ovCommEv.hEvent=NULL;	
	}
}

/**************************************************************************************
**func:初始化串口
**pPortOwner:窗口句柄，portnr:端口号，baud:波特率，parity:NONE;OOD;EVEN
**databits:6,7,8; stopbits:停止位"1"位;"1.5"位；"2"位 
**dwCommEvent：串口事件； writebuffersize:发送缓冲区的长度
**
**************************************************************************************/
BOOL CSerialPort::InitPort(CWnd*  pPortOwner,	  // the owner (CWnd) of the port (receives message)
						   const char* portnr,    // portnumber (1..4)
						   UINT   baud,			  // baudrate
						   const char* parity,	  // parity 
						   UINT  databits,		  // databits 
						   const char* stopbits,  // stopbits 
						   DWORD dwCommEvents,	  // EV_RXCHAR, EV_CTS etc
						   UINT  nWriteBufferSize,// size to the writebuffer
						   UINT  nReadBufferSize) //size to the readbuffer
{
	/*取出数字位转换成数字*/
	assert((portnr[3]-0x30) > 0 &&(portnr[3]-0x30) < 5);
	assert((pPortOwner != NULL));

	//初始化临界区
	InitializeCriticalSection(&m_csCommunicationSync);
	
	// 创建写缓冲，保存窗口句柄
	m_pOwner = pPortOwner;

	//d开辟写缓冲区
	if (m_szWriteBuffer != NULL)
	{
		delete [] m_szWriteBuffer;
		m_szWriteBuffer=NULL;
	}
	m_szWriteBuffer = new char[nWriteBufferSize];
	memset(m_szWriteBuffer,0,nWriteBufferSize);
	m_nWriteBufferSize = nWriteBufferSize;/*缓冲区大小*/
	m_dwCommEvents = dwCommEvents;      /*全局变量来保存初始化的事件类型*/

	//开辟读缓冲区
	if(m_szReadBuffer!=NULL)
	{
		delete [] m_szReadBuffer;
        m_szReadBuffer=NULL;
	}
    m_szReadBuffer=new BYTE[nReadBufferSize];
	memset(m_szReadBuffer,0,nReadBufferSize);  //清空缓冲区
	m_nReadBufferSize =nReadBufferSize;

	if (m_hComm != NULL)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
	}
    // 创建自动重置，初始状态为有信号状态
	memset(&m_ovWrite,0,sizeof(OVERLAPPED));
	m_ovWrite.hEvent = CreateEvent(NULL, FALSE,TRUE, NULL);
	
	memset(&m_ovRead,0,sizeof(OVERLAPPED));
	m_ovRead.hEvent=CreateEvent(NULL,FALSE,TRUE,NULL);
	// 创建一个人工重置，初始状态为有信号状态
	memset(&m_ovCommEv,0,sizeof(OVERLAPPED));
	m_ovCommEv.hEvent=CreateEvent(NULL,TRUE,TRUE,NULL);
/*
	CString sz ;
	sz.Format( _T("%s"), portnr ) ;
	AfxMessageBox( sz ) ;
*/
	EnterCriticalSection(&m_csCommunicationSync);
#if old
	m_hComm = CreateFile((LPCTSTR)(CString)portnr,				// communication port string (COMX)
					     GENERIC_READ | GENERIC_WRITE,	// read/write types
					     0,								// comm devices must be opened with exclusive access
					     NULL,							// no security attributes
						 OPEN_ALWAYS,					// comm devices must use OPEN_EXISTING
					     FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,			// Async I/O
					     NULL);							// template must be 0 for comm devices
#else
	m_hComm = CreateFile((LPCWSTR)(CString)(CStringA)portnr,				// communication port string (COMX)
		GENERIC_READ | GENERIC_WRITE,	// read/write types
		0,								// comm devices must be opened with exclusive access
		NULL,							// no security attributes
		OPEN_ALWAYS,					// comm devices must use OPEN_EXISTING
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,			// Async I/O
		NULL);
#endif

	LeaveCriticalSection(&m_csCommunicationSync);

	if (m_hComm == INVALID_HANDLE_VALUE) 
	{
		ProcessErrorMessage("INVALID_HANDLE");
		m_hComm=NULL;	/*把句柄设为空*/
		m_nPortNr=0;
		return FALSE;
	}

    /*保留端口号*/
	m_nPortNr = portnr[3]-0x30;

	//AfxMessageBox(portnr);
	// set the timeout values
	m_CommTimeouts.ReadIntervalTimeout = 50;
	m_CommTimeouts.ReadTotalTimeoutMultiplier = 500;
	m_CommTimeouts.ReadTotalTimeoutConstant = 1000;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 500;
	m_CommTimeouts.WriteTotalTimeoutConstant = 1000;

   
	// 配置端口
	UINT tempParity;
	UINT tempStopBit;
    tempParity=parityAnalyze(parity);
	tempStopBit=stopbitAnalyze(stopbits);
	//CString strTemp;
	//strTemp.Format("tempStopBit=%d",tempStopBit);
	//AfxMessageBox(strTemp);

	if (TRUE==SetCommTimeouts(m_hComm, &m_CommTimeouts))		//对指定的输入输入出设备设置超时事件
	{						   
		if (TRUE==SetCommMask(m_hComm, dwCommEvents))         //指定监视的事件类型
		{
			if (TRUE==GetCommState(m_hComm, &m_dcb))          //获得串口的状态
			{
				m_dcb.fRtsControl = RTS_CONTROL_ENABLE;		// set RTS bit high!
			    if(TRUE==userBuildCommDCB(&m_dcb,baud,tempParity,\
					tempStopBit,databits))
				{
					if (FALSE==SetCommState(m_hComm, &m_dcb))
					{
						ProcessErrorMessage("SetCommState()");
						return FALSE;
					}
				}
				else
				{
					ProcessErrorMessage("userBuildCommDCB()");
					return FALSE;
				}
			}
			else
			{
				ProcessErrorMessage("GetCommState()");
				return FALSE;
			}
		}
		else
		{
			ProcessErrorMessage("SetCommMask()");
			return FALSE;
		}
	}
	else
	{
		ProcessErrorMessage("SetCommTimeouts()");
		return FALSE;
	}

    SetupComm(m_hComm, 16384, 16384);		/* 设置接收与发送的缓冲区大小 */
	// 清除所有的缓冲区
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	//printf("Initialisation for communicationport %d completed.\nUse Startmonitor to communicate.\n", portnr);
	TRACE("Initialisation for communicationport %d completed.\nUse Startmonitor to communicate.\n", portnr);
	
	return TRUE;
}

//
//  The CommThread Function.
//
UINT CSerialPort::CommThreadProc(LPVOID pParam)
{
	CSerialPort *port = (CSerialPort*)pParam;
	port->m_bThreadAlive = TRUE;    /*线程在运行标志 真*/	
		
	DWORD BytesTransfered = 0; 
	DWORD Event = 0;
	DWORD CommEvent = 0;
	DWORD dwError = 0;
	BOOL  bResult = FALSE;
		
	//在线程开始时清除缓冲区
	while ((port->m_hComm != NULL)) 
	{ 
		Sleep(2);
		//SetCommMask(port->m_hComm,port->m_dwCommEvents);
		bResult = WaitCommEvent(port->m_hComm, &Event,&port->m_ovCommEv);//若失败Event 的值为0
		if(bResult==FALSE)
		{
			if(ERROR_IO_PENDING==GetLastError())//表示操作转到后台运行。
			{
				WaitForSingleObject(port->m_ovCommEv.hEvent,INFINITE);//等到说明指定的串口事件发生
				//if(TRUE==GetOverlappedResult(port->m_hComm,&port->m_ovCommEv,NULL,TRUE))
				//{
				//	bResult=TRUE;
				//}
                 bResult=TRUE;
			}
			else
			{
				continue;
			}
			
		}
		if (TRUE==bResult)   
		{ 
			bResult=FALSE;    //等待串口事件成功，清除标志
			if(TRUE==GetCommMask(port->m_hComm, &CommEvent))
			{
				if((Event & CommEvent & EV_RXCHAR)==EV_RXCHAR)
				{
					ReceiveChar(port);
				}
				else if (Event & CommEvent & EV_CTS)
				{  //清除发送
					if(TRUE==port->m_evCts)
					{
						::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_CTS_DETECTED, \
								     (WPARAM) 0, (LPARAM) port->m_nPortNr);
					}
				}
				else if (Event & CommEvent & EV_RXFLAG)
				{
					if(TRUE==port->m_evRxFlag)
					{
						::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_RXFLAG_DETECTED,\
								     (WPARAM) 0, (LPARAM) port->m_nPortNr);
					}
				}
				else if (Event & CommEvent & EV_BREAK)
				{	//输入打断
					if(TRUE==port->m_evBreak)
					{
						::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_BREAK_DETECTED, \
							         (WPARAM) 0, (LPARAM) port->m_nPortNr);
					}
				}
				else if (Event & CommEvent & EV_ERR)//A line-status error occurred. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY.
				{	
					if(TRUE==port->m_evErr)
					{
						::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_ERR_DETECTED,\
								     (WPARAM) 0, (LPARAM) port->m_nPortNr);
					}
				}
				else if (Event & CommEvent & EV_RING)//串口回环检测
				{	
					if(TRUE==port->m_evRing)
					{
						::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_RING_DETECTED,\
								     (WPARAM) 0, (LPARAM) port->m_nPortNr);
					}
				}
				else
				{
					//port->ProcessErrorMessage("串口未知事件！");	//报告错误 
				}
			}
			else
			{
				port->ProcessErrorMessage("GetCommMask(port->m_hComm, &CommEvent)");	//报告错误 
			}
		}
		else
		{
			if(ERROR_IO_PENDING==GetLastError())
			{
				Sleep(2);	 //操作正在后台执行，此线程睡一会儿，等等		
			}
			else
			{
				//获得错误信息
				//port->ProcessErrorMessage("WaitCommEvent()");		
			}
		} 
    
	} 
	
    CloseHandle(port->m_Thread);
	port->m_Thread=NULL;
	port->m_bThreadAlive = FALSE;
	return 0;
}

/******************************************************************
** name:StartMonitoring()
** func:创建线程开始监视串口
*******************************************************************/
BOOL CSerialPort::StartMonitoring()
{

	m_Thread=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CommThreadProc,(LPVOID)this,0,NULL);
	if( m_Thread == NULL ) return FALSE ;
	TRACE(_T("Thread started\n"));
	return TRUE ;
}

/******************************************************************
** name:RestartMonitoring()
** func:从新恢复线程监视状态，此函数必须在StopMonitoring()调用后使用
*******************************************************************/
BOOL CSerialPort::RestartMonitoring()
{
	TRACE(_T("Thread resumed\n"));
	ResumeThread(m_Thread);
	return TRUE;	
}


/******************************************************************
** name:StopMonitoring()
** func:挂起线程
*******************************************************************/
BOOL CSerialPort::StopMonitoring()
{
	TRACE(_T("Thread suspended\n"));
	SuspendThread(m_Thread);
	return TRUE;	
}


//
// If there is a error, give the right message
//
void CSerialPort::ProcessErrorMessage(char* ErrorText)
{
	wchar_t *Temp = new wchar_t[256];
	LPVOID lpMsgBuf;

	FormatMessage( 
		FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		100,
		NULL 
	);

	_stprintf(Temp, L", \nFailed with the following error: %s\n Port: %d\n", lpMsgBuf, m_nPortNr);
	CString str(ErrorText); str += L"WARNING:" + str;
	CString str1(Temp);
	str += str1;
	
	MessageBox(m_pOwner->m_hWnd,str, _T("Application Error"), MB_ICONSTOP);

	LocalFree(lpMsgBuf);

	delete [] Temp;
}
BOOL CSerialPort::userBuildCommDCB(DCB* dcb,UINT baud,UINT parity,UINT stopbit,UINT databit)
{
	EnterCriticalSection(&m_csCommunicationSync);
	if (TRUE==GetCommState(m_hComm, &m_dcb)){		
		m_dcb.BaudRate=baud;
		m_dcb.Parity=parity;
		m_dcb.StopBits=stopbit;
		m_dcb.ByteSize=databit;
		if (FALSE==SetCommState(m_hComm, &m_dcb)){
			LeaveCriticalSection(&m_csCommunicationSync);
			return FALSE;	; 
		}
		LeaveCriticalSection(&m_csCommunicationSync);
		return TRUE;//正常操作，继续
	}
	else{
		LeaveCriticalSection(&m_csCommunicationSync);
		return FALSE;
	}
}


void  CSerialPort::ReceiveChar(CSerialPort* port)
{
	BOOL  bResult = FALSE;
	DWORD dwError = 0;
	DWORD dwReadLen=0;   //缓冲区接收的字节长度
	DWORD BytesRead = 0;
	COMSTAT comstat;
	memset(&comstat,0,sizeof(comstat));   //初始化全局变量

	BYTE* recvBuf=new BYTE[port->m_nReadBufferSize];    //临时接收缓冲区
	memset(recvBuf,0,port->m_nReadBufferSize);
	assert(port->m_hComm != 0);

	EnterCriticalSection(&port->m_csCommunicationSync);
	bResult = ClearCommError(port->m_hComm, &dwError, &comstat); //获得串口状态信息
	if(bResult==FALSE)
	{
		port->ProcessErrorMessage("ClearCommError()"); 
	}
	LeaveCriticalSection(&port->m_csCommunicationSync);
	
	dwReadLen=comstat.cbInQue;      //获得输入缓冲区接收的字节长度
	if (dwReadLen == 0)
	{
		return;
	}
	//if(comstat.cbInQue<10) return ;  
	bResult=FALSE; 				
	EnterCriticalSection(&port->m_csCommunicationSync);
	bResult = ReadFile(port->m_hComm,		// Handle to COMM port 
						   recvBuf,			// received Buffer Pointer
						   dwReadLen,		// The maximum number of bytes to be read
						   NULL,			// Stores number of bytes read
						   &port->m_ovRead);	
	LeaveCriticalSection(&port->m_csCommunicationSync);
	if (FALSE==bResult)  
	{  
		if(ERROR_IO_PENDING==GetLastError())
		{
			bResult=TRUE;
		}
		else
		{
			port->ProcessErrorMessage("ReadFile()");
			return;
		}
	}

	if(bResult==TRUE)
	{
		 BOOL rtnValue=FALSE;
		 while(rtnValue==FALSE)
		 {
			 rtnValue=GetOverlappedResult(port->m_hComm,&port->m_ovRead,&dwReadLen,TRUE); // Wait flag
			 if(rtnValue==FALSE)
			 {
					if(ERROR_IO_INCOMPLETE==GetLastError())
					{
						continue;   //继续等待
					}
					else
					{
						return ;   //若失败，则直接返回
					}
			 }
		 }
	    //打印调试信息
/*		CString str;
		str.Format(_T("%d\n"),port->m_nReadBufferSize);
		((CSmartUpdateDlg*)(port->m_pOwner))->m_MyRichEditCtrl.OutputText(str); */  

		//此处有两种方式1：把读到的数据放到CSerialPort类的接收缓冲区中
		//              2:把收到的字符以消息的方式发送到指定的窗口
		PurgeComm(port->m_hComm, PURGE_RXCLEAR | PURGE_RXABORT );
	    EnterCriticalSection(&port->m_csCommunicationSync);
		for(UINT i=0;i<dwReadLen;i++)
		{
			if(port->m_dwReadCount==port->m_nReadBufferSize)
			{
				port->m_dwReadCount=0;    //若读缓冲区满，从头存放，覆盖原来的值
			}
			port->m_szReadBuffer[port->m_dwReadCount]=recvBuf[i];
			port->m_dwReadCount++;
		}
        LeaveCriticalSection(&port->m_csCommunicationSync);

		//方式二：把读到的字节一个个抛到指定的窗口

        if(TRUE==port->m_evRxchar)
		{
			for(UINT i=0;i<dwReadLen;i++)
			{
			//	::PostMessage(port->m_pOwner->m_hWnd, WM_COMM_RXCHAR, (WPARAM) recvBuf[i], (LPARAM)port->m_nPortNr);
				::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_RXCHAR, (WPARAM)recvBuf[i], (LPARAM)port->m_nPortNr);
			}
		}
	}
	delete[] recvBuf;  //删除临时内存
	recvBuf = NULL;
}

//清除读缓冲区
BOOL CSerialPort::ClearReadBuffer()
{
	memset(m_szReadBuffer,0,m_nReadBufferSize);
	m_dwReadCount=0;
	return TRUE;
}

BOOL CSerialPort::WriteToPort(char* string,DWORD length)
{	

	BOOL bResult = FALSE;
	DWORD BytesSent = 0;
	//DWORD dwError=0;
	//COMSTAT comstat;
    //ClearCommError(m_hComm, &dwError, &comstat); //获得串口状态信息
	if((m_hComm ==NULL) ||(length== 0)){
		ProcessErrorMessage("m_hComm==NULL|| 不能发送数据长度为0的字节");
		return FALSE;
		
	}
 //   while(m_bWriteIsAlive==TRUE)   //若程序还在写操作则等待串口写完
	//{
	//	Sleep(2);
	//}
    m_bWriteIsAlive=TRUE;   //对串口写操作
	//memset(m_szWriteBuffer, 0, m_nWriteBufferSize); //清除缓冲
	//memcpy(m_szWriteBuffer, string,length);
	
	//CString temp;
	//temp.Format("%s",m_szWriteBuffer);
    //AfxMessageBox(temp);

	
	EnterCriticalSection(&m_csCommunicationSync);
	//清除设备缓冲区
	//PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR);
	PurgeComm(m_hComm, PURGE_TXCLEAR| PURGE_TXABORT);
	bResult = WriteFile(m_hComm,						// Handle to COMM Port
						string,				// Pointer to message buffer in calling finction
						length,							//Length of message to send
						NULL,						// Where to store the number of bytes sent
						&m_ovWrite);
    
	LeaveCriticalSection(&m_csCommunicationSync);
    
	if (FALSE==bResult)  
	{
		if(ERROR_IO_PENDING==GetLastError())
		{
			bResult=TRUE;
		}
		else
		{
			ProcessErrorMessage("WriteFile()");
			//m_bWriteIsAlive=FALSE;   //写退出标志
			return FALSE;
		}

	} 
    if(bResult==TRUE)   //写io pending 
	{
		BOOL rtnValue=FALSE;
		while(rtnValue==FALSE)
		{
			rtnValue=GetOverlappedResult(m_hComm,&m_ovWrite,&BytesSent,FALSE); 		
			if(rtnValue==FALSE)
			{
				if(ERROR_IO_INCOMPLETE==GetLastError())
				{
					continue;
				}
				else
				{
					m_bWriteIsAlive=FALSE;   //写退出标志
					return FALSE;   //若失败，则直接返回假
			    }
			}
		}
	}

    //向窗口打印写的字符个数 
	//CSmartUpdateDlg* dlg=(CSmartUpdateDlg*)m_pOwner;
	//CString str;
	//str.Format(_T("串口发送字节长度：BytesSent=%d,实际发送字节长度：length=%d\n"),BytesSent,length);
	//dlg->m_MyRichEditCtrl.OutputText(str);
	
	if (BytesSent != length)
	{
		m_bWriteIsAlive=FALSE;
		bResult=FALSE;
	}
	m_bWriteIsAlive=FALSE;//写退出标志
	return bResult;

}


DCB CSerialPort::GetDCB()// Return the device control block
{
	return m_dcb;
}

DWORD CSerialPort::GetMonitorEvents()// Return the communication event masks
{
	return m_dwCommEvents;
}

DWORD CSerialPort::GetWriteBufferSize()// Return the output buffer size
{
	return m_nWriteBufferSize;
}

DWORD CSerialPort::GetCommPort()
{
	return  m_nPortNr;
}

void CSerialPort::Close()
{   
	/*结束线程*/
	if(m_bThreadAlive==TRUE)
	{
		/*结束线程*/
         DWORD dwExitCode ;
		 if( GetExitCodeThread( m_Thread, &dwExitCode ) ) 
		 {
			TerminateThread(m_Thread, dwExitCode ) ;
			CloseHandle(m_Thread);
		 }
		 else
		 {
			//AfxMessageBox(_T("GetExitCodeThread() error!"));
		 }
		 m_Thread = NULL ;
		m_bThreadAlive=FALSE;
	}
	/*关闭串口设备*/
	if(m_hComm!=NULL){
		CloseHandle(m_hComm);
		m_hComm=NULL;
	}

	/*使窗口句柄无效*/
	if(m_pOwner!=NULL){
		m_pOwner=NULL;
	}
	/*删除发送缓冲区*/
	if (m_szWriteBuffer != NULL)
	{
		delete[]  m_szWriteBuffer;
		m_szWriteBuffer=NULL;
	}
	if (m_szReadBuffer != NULL)
	{
		delete[] m_szReadBuffer;
		m_szReadBuffer=NULL;
	}
    //关闭事件句柄
	if(m_ovWrite.hEvent!=NULL)
	{
		ResetEvent(m_ovWrite.hEvent);
		CloseHandle(m_ovWrite.hEvent);
		m_ovWrite.hEvent=NULL;
	}
	if(m_ovRead.hEvent!=NULL)
	{
		ResetEvent(m_ovRead.hEvent);
		CloseHandle(m_ovRead.hEvent);
		m_ovRead.hEvent=NULL;
	}
	if(m_ovCommEv.hEvent!=NULL)
	{
		ResetEvent(m_ovCommEv.hEvent);
		CloseHandle(m_ovCommEv.hEvent);
		m_ovCommEv.hEvent=NULL;	
	}

}
UINT CSerialPort::parityAnalyze(const char* ch)
{

	if(0==strncmp("NONE",ch,5)){
		return 0; 
	}
	if(0==strncmp("ODD",ch,5)){
		return 1;
	}
	if(0==strncmp("EVEN",ch,5)){
		return 2;
	}
	if(0==strncmp("MARK",ch,5)){
	    return 3;
	}
	if(0==strncmp("SPACE",ch,5)){
		return 4;
	}
	else{
		return 0;
	}
}

UINT CSerialPort::stopbitAnalyze(const char* ch)
{
	if(0==strncmp("1",ch,5))
	{

		return 0; 
	}
	if(0==strncmp("1.5",ch,5))
	{

		return 1;
	}
	if(0==strncmp("2",ch,5))
	{
		return 2;
	}
	else
	{
		return 0;
	}
}

BOOL CSerialPort::SetBaudRate(DWORD rate)  /*设置波特率*/
{
	//CString str;
	//str.Format("rate=%d",rate);
	//AfxMessageBox(str);
	EnterCriticalSection(&m_csCommunicationSync);
	if (TRUE==GetCommState(m_hComm, &m_dcb)){		
		m_dcb.BaudRate=rate;
		if (FALSE==SetCommState(m_hComm, &m_dcb)){
			LeaveCriticalSection(&m_csCommunicationSync);
			return FALSE;	; //正常操作，继续
		}
		LeaveCriticalSection(&m_csCommunicationSync);
		return TRUE;
	}
	else{
		LeaveCriticalSection(&m_csCommunicationSync);
		return FALSE;
	}
}

BOOL CSerialPort::SetParity(const char* ch)      /*设置奇偶校验*/
{
	EnterCriticalSection(&m_csCommunicationSync);
	if (TRUE==GetCommState(m_hComm, &m_dcb)){		
		m_dcb.Parity=parityAnalyze(ch);
		if (FALSE==SetCommState(m_hComm, &m_dcb)){
			LeaveCriticalSection(&m_csCommunicationSync);
			return FALSE;	; 
		}
		LeaveCriticalSection(&m_csCommunicationSync);
		return TRUE;//正常操作，继续
	}
	else{
		LeaveCriticalSection(&m_csCommunicationSync);
		return FALSE;
	}
}

BOOL CSerialPort::SetStopbit(const char* ch)     /*设置停止位*/
{
	EnterCriticalSection(&m_csCommunicationSync);
	if (TRUE==GetCommState(m_hComm, &m_dcb)){		
		m_dcb.StopBits=stopbitAnalyze(ch);
		if (FALSE==SetCommState(m_hComm, &m_dcb)){
			LeaveCriticalSection(&m_csCommunicationSync);
			return FALSE;	; 
		}
		LeaveCriticalSection(&m_csCommunicationSync);
		return TRUE;//正常操作，继续
	}
	else{
		LeaveCriticalSection(&m_csCommunicationSync);
		return FALSE;
	}
}

/*设置数据位*/
BOOL CSerialPort::SetDatabit(BYTE bits)   
{
	EnterCriticalSection(&m_csCommunicationSync);
	if (TRUE==GetCommState(m_hComm, &m_dcb)){		
		m_dcb.ByteSize=bits;
		if (FALSE==SetCommState(m_hComm, &m_dcb)){
			LeaveCriticalSection(&m_csCommunicationSync);
			return FALSE;	; 
		}
		LeaveCriticalSection(&m_csCommunicationSync);
		return TRUE;//正常操作，继续
	}
	else{
		LeaveCriticalSection(&m_csCommunicationSync);
		return FALSE;
	}
}
/*设置缓冲区的大小*/
BOOL CSerialPort::SetWriteBufSize(DWORD len)   
{
	EnterCriticalSection(&m_csCommunicationSync);
	if(m_szWriteBuffer!=NULL)
	{
		delete [] m_szWriteBuffer;
        m_szWriteBuffer=NULL;
	}
	m_szWriteBuffer=new char[len];
	m_nWriteBufferSize=len;
	memset(m_szWriteBuffer,0,len);
	LeaveCriticalSection(&m_csCommunicationSync);
	return TRUE;

} 
BOOL CSerialPort::SetReadBufSize(DWORD length)
{
	EnterCriticalSection(&m_csCommunicationSync);
	if(m_szReadBuffer!=NULL)
	{
		delete [] m_szReadBuffer;
	}
	m_szReadBuffer=new BYTE[length];
	m_nReadBufferSize=length;
	memset(m_szReadBuffer,0,length);
	LeaveCriticalSection(&m_csCommunicationSync);
	return TRUE;

}
/*设置串口监视事件*/  
BOOL CSerialPort::SetMonitorEvent(DWORD tmpEvent)  
{
	EnterCriticalSection(&m_csCommunicationSync);
	if(NULL!=m_hComm){
		if(TRUE==SetCommMask(m_hComm,tmpEvent)){
			LeaveCriticalSection(&m_csCommunicationSync);
			m_dwCommEvents=tmpEvent;   //回传所设置的串口监视事件
			return TRUE;
		}
		LeaveCriticalSection(&m_csCommunicationSync);
		return FALSE;
	}
	LeaveCriticalSection(&m_csCommunicationSync);
	return FALSE;
}
/*获得串口操作句柄*/
HANDLE CSerialPort::GetCommHandle()   
{
	return m_hComm;
}

CWnd* CSerialPort::GetWindowOwner()   /*获得串口的窗口拥有者*/
{
	return m_pOwner;
}

/*************************************************************
**funcname：EnableSerialMessage
**para:当传递为TRUE或1表示允许发送对应的消息
**功能：当对应的位当设为TRUE时表示，监视线程将向此类的窗口拥有着发送这些串口消息，
** 此时用户将在窗口中实现自定义的窗口消息映射，否则当调用InitPort()初始化串口监视这些消息时，
**程序会跑飞 在默认状况下，这些消息将被禁止;
***************************************************************/
void CSerialPort::EnableSerialMessage(BOOL EnRxchar,BOOL EnCts,BOOL EnRxFlag,\
									BOOL EnBreak,BOOL EnErr,BOOL EnRing)
{
	m_evRxchar=EnRxchar;
	m_evCts=EnCts;
	m_evRxFlag=EnRxFlag;
	m_evBreak=EnBreak;
	m_evErr=EnErr;
	m_evRing=EnRing;
}
/*获得用户自定义的接收数据缓冲区的大小*/
DWORD  CSerialPort::GetReadBufferSize()
{
	return m_nReadBufferSize;
}

/*获得接收缓冲区的数据*/
BOOL  CSerialPort::GetReadBufferData(BYTE *ch,DWORD length)
{
	if((m_hComm ==NULL) ||(length <m_nReadBufferSize)){
		return FALSE;
	}
	 
	memset(ch, 0, length); //清除缓冲

	EnterCriticalSection(&m_csCommunicationSync);
	memcpy(ch,m_szReadBuffer,length);	
	LeaveCriticalSection(&m_csCommunicationSync);

	return TRUE;
}
