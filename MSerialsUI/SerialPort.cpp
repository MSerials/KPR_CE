/*
**	�ļ���	CSerialPort.h
**  created  by lipeng 2010-9-12  in GW Corperation
**  functions :������Ҫʹ�� windows�ĵײ�API�������Դ��ڽ����˷�װ�����ڴ���ͨ�ţ���������
**  ��ѡ��ʹ����Ϣ���ƻ�ֱ�ӷ��ʵķ�ʽ��Ӧ�ó��򽻻��������û�ֻ��Ҫʵ���û��Զ������Ϣ�͵��ô������
**  public �������Դ��ڲ���
*/

#include "stdafx.h"
#include "SerialPort.h"
#include "afx.h"		//ʹ��CString���ͷ�ļ�
#include <assert.h>
//#include "RS232Dlg.h"
//#include "resource.h"
#pragma warning(disable:4995)
#pragma warning(disable:4996)
#pragma warning(disable:4477)

CSerialPort::CSerialPort()
{
	m_hComm = NULL;
    //��������Ϊ��
	m_szWriteBuffer = NULL;
    //�̴߳�����־Ϊ��
	m_bThreadAlive = FALSE;
    //�߳̾����Ϊ��
	m_Thread = NULL ;
    //�ص�IO����
	m_ovWrite.hEvent = NULL;
	m_ovRead.hEvent=NULL;
	m_ovCommEv.hEvent=NULL;

	m_szReadBuffer=NULL;
	m_dwReadCount=0;      /*��ָ�� ��ʼ����Ϊ0*/

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


/*����������ɾ��������ڴ棬�����̣߳��رվ��*/
CSerialPort::~CSerialPort()
{
	if(m_bThreadAlive==TRUE)
	{
		/*�����߳�*/
         DWORD dwExitCode ;
		 if( GetExitCodeThread( m_Thread, &dwExitCode ) ) 
		 {
			TerminateThread(m_Thread, dwExitCode ) ;
			CloseHandle(m_Thread);
		 }
		 m_Thread = NULL ;
		m_bThreadAlive=FALSE;
	}

	/*�رմ����豸*/
	if(m_hComm!=NULL)
	{
		CloseHandle(m_hComm);
		m_hComm=NULL;
	}

	/*ʹ���ھ����Ч*/
	if(m_pOwner!=NULL)
	{
		m_pOwner=NULL;
	}

	/*ɾ�����ͻ�����*/
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
    //�ر��¼�
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
**func:��ʼ������
**pPortOwner:���ھ����portnr:�˿ںţ�baud:�����ʣ�parity:NONE;OOD;EVEN
**databits:6,7,8; stopbits:ֹͣλ"1"λ;"1.5"λ��"2"λ 
**dwCommEvent�������¼��� writebuffersize:���ͻ������ĳ���
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
	/*ȡ������λת��������*/
	assert((portnr[3]-0x30) > 0 &&(portnr[3]-0x30) < 5);
	assert((pPortOwner != NULL));

	//��ʼ���ٽ���
	InitializeCriticalSection(&m_csCommunicationSync);
	
	// ����д���壬���洰�ھ��
	m_pOwner = pPortOwner;

	//d����д������
	if (m_szWriteBuffer != NULL)
	{
		delete [] m_szWriteBuffer;
		m_szWriteBuffer=NULL;
	}
	m_szWriteBuffer = new char[nWriteBufferSize];
	memset(m_szWriteBuffer,0,nWriteBufferSize);
	m_nWriteBufferSize = nWriteBufferSize;/*��������С*/
	m_dwCommEvents = dwCommEvents;      /*ȫ�ֱ����������ʼ�����¼�����*/

	//���ٶ�������
	if(m_szReadBuffer!=NULL)
	{
		delete [] m_szReadBuffer;
        m_szReadBuffer=NULL;
	}
    m_szReadBuffer=new BYTE[nReadBufferSize];
	memset(m_szReadBuffer,0,nReadBufferSize);  //��ջ�����
	m_nReadBufferSize =nReadBufferSize;

	if (m_hComm != NULL)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
	}
    // �����Զ����ã���ʼ״̬Ϊ���ź�״̬
	memset(&m_ovWrite,0,sizeof(OVERLAPPED));
	m_ovWrite.hEvent = CreateEvent(NULL, FALSE,TRUE, NULL);
	
	memset(&m_ovRead,0,sizeof(OVERLAPPED));
	m_ovRead.hEvent=CreateEvent(NULL,FALSE,TRUE,NULL);
	// ����һ���˹����ã���ʼ״̬Ϊ���ź�״̬
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
		m_hComm=NULL;	/*�Ѿ����Ϊ��*/
		m_nPortNr=0;
		return FALSE;
	}

    /*�����˿ں�*/
	m_nPortNr = portnr[3]-0x30;

	//AfxMessageBox(portnr);
	// set the timeout values
	m_CommTimeouts.ReadIntervalTimeout = 50;
	m_CommTimeouts.ReadTotalTimeoutMultiplier = 500;
	m_CommTimeouts.ReadTotalTimeoutConstant = 1000;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 500;
	m_CommTimeouts.WriteTotalTimeoutConstant = 1000;

   
	// ���ö˿�
	UINT tempParity;
	UINT tempStopBit;
    tempParity=parityAnalyze(parity);
	tempStopBit=stopbitAnalyze(stopbits);
	//CString strTemp;
	//strTemp.Format("tempStopBit=%d",tempStopBit);
	//AfxMessageBox(strTemp);

	if (TRUE==SetCommTimeouts(m_hComm, &m_CommTimeouts))		//��ָ��������������豸���ó�ʱ�¼�
	{						   
		if (TRUE==SetCommMask(m_hComm, dwCommEvents))         //ָ�����ӵ��¼�����
		{
			if (TRUE==GetCommState(m_hComm, &m_dcb))          //��ô��ڵ�״̬
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

    SetupComm(m_hComm, 16384, 16384);		/* ���ý����뷢�͵Ļ�������С */
	// ������еĻ�����
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
	port->m_bThreadAlive = TRUE;    /*�߳������б�־ ��*/	
		
	DWORD BytesTransfered = 0; 
	DWORD Event = 0;
	DWORD CommEvent = 0;
	DWORD dwError = 0;
	BOOL  bResult = FALSE;
		
	//���߳̿�ʼʱ���������
	while ((port->m_hComm != NULL)) 
	{ 
		Sleep(2);
		//SetCommMask(port->m_hComm,port->m_dwCommEvents);
		bResult = WaitCommEvent(port->m_hComm, &Event,&port->m_ovCommEv);//��ʧ��Event ��ֵΪ0
		if(bResult==FALSE)
		{
			if(ERROR_IO_PENDING==GetLastError())//��ʾ����ת����̨���С�
			{
				WaitForSingleObject(port->m_ovCommEv.hEvent,INFINITE);//�ȵ�˵��ָ���Ĵ����¼�����
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
			bResult=FALSE;    //�ȴ������¼��ɹ��������־
			if(TRUE==GetCommMask(port->m_hComm, &CommEvent))
			{
				if((Event & CommEvent & EV_RXCHAR)==EV_RXCHAR)
				{
					ReceiveChar(port);
				}
				else if (Event & CommEvent & EV_CTS)
				{  //�������
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
				{	//������
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
				else if (Event & CommEvent & EV_RING)//���ڻػ����
				{	
					if(TRUE==port->m_evRing)
					{
						::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_RING_DETECTED,\
								     (WPARAM) 0, (LPARAM) port->m_nPortNr);
					}
				}
				else
				{
					//port->ProcessErrorMessage("����δ֪�¼���");	//������� 
				}
			}
			else
			{
				port->ProcessErrorMessage("GetCommMask(port->m_hComm, &CommEvent)");	//������� 
			}
		}
		else
		{
			if(ERROR_IO_PENDING==GetLastError())
			{
				Sleep(2);	 //�������ں�ִ̨�У����߳�˯һ������ȵ�		
			}
			else
			{
				//��ô�����Ϣ
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
** func:�����߳̿�ʼ���Ӵ���
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
** func:���»ָ��̼߳���״̬���˺���������StopMonitoring()���ú�ʹ��
*******************************************************************/
BOOL CSerialPort::RestartMonitoring()
{
	TRACE(_T("Thread resumed\n"));
	ResumeThread(m_Thread);
	return TRUE;	
}


/******************************************************************
** name:StopMonitoring()
** func:�����߳�
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
		return TRUE;//��������������
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
	DWORD dwReadLen=0;   //���������յ��ֽڳ���
	DWORD BytesRead = 0;
	COMSTAT comstat;
	memset(&comstat,0,sizeof(comstat));   //��ʼ��ȫ�ֱ���

	BYTE* recvBuf=new BYTE[port->m_nReadBufferSize];    //��ʱ���ջ�����
	memset(recvBuf,0,port->m_nReadBufferSize);
	assert(port->m_hComm != 0);

	EnterCriticalSection(&port->m_csCommunicationSync);
	bResult = ClearCommError(port->m_hComm, &dwError, &comstat); //��ô���״̬��Ϣ
	if(bResult==FALSE)
	{
		port->ProcessErrorMessage("ClearCommError()"); 
	}
	LeaveCriticalSection(&port->m_csCommunicationSync);
	
	dwReadLen=comstat.cbInQue;      //������뻺�������յ��ֽڳ���
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
						continue;   //�����ȴ�
					}
					else
					{
						return ;   //��ʧ�ܣ���ֱ�ӷ���
					}
			 }
		 }
	    //��ӡ������Ϣ
/*		CString str;
		str.Format(_T("%d\n"),port->m_nReadBufferSize);
		((CSmartUpdateDlg*)(port->m_pOwner))->m_MyRichEditCtrl.OutputText(str); */  

		//�˴������ַ�ʽ1���Ѷ��������ݷŵ�CSerialPort��Ľ��ջ�������
		//              2:���յ����ַ�����Ϣ�ķ�ʽ���͵�ָ���Ĵ���
		PurgeComm(port->m_hComm, PURGE_RXCLEAR | PURGE_RXABORT );
	    EnterCriticalSection(&port->m_csCommunicationSync);
		for(UINT i=0;i<dwReadLen;i++)
		{
			if(port->m_dwReadCount==port->m_nReadBufferSize)
			{
				port->m_dwReadCount=0;    //����������������ͷ��ţ�����ԭ����ֵ
			}
			port->m_szReadBuffer[port->m_dwReadCount]=recvBuf[i];
			port->m_dwReadCount++;
		}
        LeaveCriticalSection(&port->m_csCommunicationSync);

		//��ʽ�����Ѷ������ֽ�һ�����׵�ָ���Ĵ���

        if(TRUE==port->m_evRxchar)
		{
			for(UINT i=0;i<dwReadLen;i++)
			{
			//	::PostMessage(port->m_pOwner->m_hWnd, WM_COMM_RXCHAR, (WPARAM) recvBuf[i], (LPARAM)port->m_nPortNr);
				::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_RXCHAR, (WPARAM)recvBuf[i], (LPARAM)port->m_nPortNr);
			}
		}
	}
	delete[] recvBuf;  //ɾ����ʱ�ڴ�
	recvBuf = NULL;
}

//�����������
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
    //ClearCommError(m_hComm, &dwError, &comstat); //��ô���״̬��Ϣ
	if((m_hComm ==NULL) ||(length== 0)){
		ProcessErrorMessage("m_hComm==NULL|| ���ܷ������ݳ���Ϊ0���ֽ�");
		return FALSE;
		
	}
 //   while(m_bWriteIsAlive==TRUE)   //��������д������ȴ�����д��
	//{
	//	Sleep(2);
	//}
    m_bWriteIsAlive=TRUE;   //�Դ���д����
	//memset(m_szWriteBuffer, 0, m_nWriteBufferSize); //�������
	//memcpy(m_szWriteBuffer, string,length);
	
	//CString temp;
	//temp.Format("%s",m_szWriteBuffer);
    //AfxMessageBox(temp);

	
	EnterCriticalSection(&m_csCommunicationSync);
	//����豸������
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
			//m_bWriteIsAlive=FALSE;   //д�˳���־
			return FALSE;
		}

	} 
    if(bResult==TRUE)   //дio pending 
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
					m_bWriteIsAlive=FALSE;   //д�˳���־
					return FALSE;   //��ʧ�ܣ���ֱ�ӷ��ؼ�
			    }
			}
		}
	}

    //�򴰿ڴ�ӡд���ַ����� 
	//CSmartUpdateDlg* dlg=(CSmartUpdateDlg*)m_pOwner;
	//CString str;
	//str.Format(_T("���ڷ����ֽڳ��ȣ�BytesSent=%d,ʵ�ʷ����ֽڳ��ȣ�length=%d\n"),BytesSent,length);
	//dlg->m_MyRichEditCtrl.OutputText(str);
	
	if (BytesSent != length)
	{
		m_bWriteIsAlive=FALSE;
		bResult=FALSE;
	}
	m_bWriteIsAlive=FALSE;//д�˳���־
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
	/*�����߳�*/
	if(m_bThreadAlive==TRUE)
	{
		/*�����߳�*/
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
	/*�رմ����豸*/
	if(m_hComm!=NULL){
		CloseHandle(m_hComm);
		m_hComm=NULL;
	}

	/*ʹ���ھ����Ч*/
	if(m_pOwner!=NULL){
		m_pOwner=NULL;
	}
	/*ɾ�����ͻ�����*/
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
    //�ر��¼����
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

BOOL CSerialPort::SetBaudRate(DWORD rate)  /*���ò�����*/
{
	//CString str;
	//str.Format("rate=%d",rate);
	//AfxMessageBox(str);
	EnterCriticalSection(&m_csCommunicationSync);
	if (TRUE==GetCommState(m_hComm, &m_dcb)){		
		m_dcb.BaudRate=rate;
		if (FALSE==SetCommState(m_hComm, &m_dcb)){
			LeaveCriticalSection(&m_csCommunicationSync);
			return FALSE;	; //��������������
		}
		LeaveCriticalSection(&m_csCommunicationSync);
		return TRUE;
	}
	else{
		LeaveCriticalSection(&m_csCommunicationSync);
		return FALSE;
	}
}

BOOL CSerialPort::SetParity(const char* ch)      /*������żУ��*/
{
	EnterCriticalSection(&m_csCommunicationSync);
	if (TRUE==GetCommState(m_hComm, &m_dcb)){		
		m_dcb.Parity=parityAnalyze(ch);
		if (FALSE==SetCommState(m_hComm, &m_dcb)){
			LeaveCriticalSection(&m_csCommunicationSync);
			return FALSE;	; 
		}
		LeaveCriticalSection(&m_csCommunicationSync);
		return TRUE;//��������������
	}
	else{
		LeaveCriticalSection(&m_csCommunicationSync);
		return FALSE;
	}
}

BOOL CSerialPort::SetStopbit(const char* ch)     /*����ֹͣλ*/
{
	EnterCriticalSection(&m_csCommunicationSync);
	if (TRUE==GetCommState(m_hComm, &m_dcb)){		
		m_dcb.StopBits=stopbitAnalyze(ch);
		if (FALSE==SetCommState(m_hComm, &m_dcb)){
			LeaveCriticalSection(&m_csCommunicationSync);
			return FALSE;	; 
		}
		LeaveCriticalSection(&m_csCommunicationSync);
		return TRUE;//��������������
	}
	else{
		LeaveCriticalSection(&m_csCommunicationSync);
		return FALSE;
	}
}

/*��������λ*/
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
		return TRUE;//��������������
	}
	else{
		LeaveCriticalSection(&m_csCommunicationSync);
		return FALSE;
	}
}
/*���û������Ĵ�С*/
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
/*���ô��ڼ����¼�*/  
BOOL CSerialPort::SetMonitorEvent(DWORD tmpEvent)  
{
	EnterCriticalSection(&m_csCommunicationSync);
	if(NULL!=m_hComm){
		if(TRUE==SetCommMask(m_hComm,tmpEvent)){
			LeaveCriticalSection(&m_csCommunicationSync);
			m_dwCommEvents=tmpEvent;   //�ش������õĴ��ڼ����¼�
			return TRUE;
		}
		LeaveCriticalSection(&m_csCommunicationSync);
		return FALSE;
	}
	LeaveCriticalSection(&m_csCommunicationSync);
	return FALSE;
}
/*��ô��ڲ������*/
HANDLE CSerialPort::GetCommHandle()   
{
	return m_hComm;
}

CWnd* CSerialPort::GetWindowOwner()   /*��ô��ڵĴ���ӵ����*/
{
	return m_pOwner;
}

/*************************************************************
**funcname��EnableSerialMessage
**para:������ΪTRUE��1��ʾ�����Ͷ�Ӧ����Ϣ
**���ܣ�����Ӧ��λ����ΪTRUEʱ��ʾ�������߳̽������Ĵ���ӵ���ŷ�����Щ������Ϣ��
** ��ʱ�û����ڴ�����ʵ���Զ���Ĵ�����Ϣӳ�䣬���򵱵���InitPort()��ʼ�����ڼ�����Щ��Ϣʱ��
**������ܷ� ��Ĭ��״���£���Щ��Ϣ������ֹ;
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
/*����û��Զ���Ľ������ݻ������Ĵ�С*/
DWORD  CSerialPort::GetReadBufferSize()
{
	return m_nReadBufferSize;
}

/*��ý��ջ�����������*/
BOOL  CSerialPort::GetReadBufferData(BYTE *ch,DWORD length)
{
	if((m_hComm ==NULL) ||(length <m_nReadBufferSize)){
		return FALSE;
	}
	 
	memset(ch, 0, length); //�������

	EnterCriticalSection(&m_csCommunicationSync);
	memcpy(ch,m_szReadBuffer,length);	
	LeaveCriticalSection(&m_csCommunicationSync);

	return TRUE;
}
