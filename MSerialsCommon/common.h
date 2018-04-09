#pragma once
#include "Tools.h"


#define IFCALLRET(_cb, _ret, ...) do { if (_cb != NULL) { _ret = _cb( __VA_ARGS__ ); } } while (0)
#define Log_Init(name)	openlog(name,LOG_CONS|LOG_PID,LOG_LOCAL7)
#define Log_Close() closelog()
#define Log_ex(Priority,format,...) syslog(Priority,format,##__VA_ARGS__)
#define Long_Debug(format,...)	syslog(LOG_DEBUG,"%s-%d:"format,__FILE__,__LINE__,##__VA_ARGS__)
#define Long_Info(format,...)	syslog(LOG_INFO,"%s-%d:"format,__FILE__,__LINE__,##__VA_ARGS__)

//Log_Init("Net_Send_Rec");
//Log_Info("send\n");


#define PYLOADFUNFAIL			-5
#define PARA_ERROR				-4
#define	OVER_LAP				-3
#define INIT_LOADSCRIPTERROR	-2
#define INIT_FAILED				-1
#define INIT_SUCCESS			0

//system Error
#define NoError					0
#define NOCAMERA1				(1<<0)
#define NOCAMERA2				(1<<1)
#define NOHALCONPARA			(1<<2)
#define NOOPENCVPARA			(1<<3)
#define NOCOM					(1<<4)


#define CAMERACLOSEERROR		-1
#define CAMERALIBERROR			-2



//check error
//������ش���
//����ok
#define OBJECT_OK				(1)
//���������۷��˻���û������
#define OBJECT_INV				(2)
//������û����,����û������
#define OBJECT_VOID				(3)
//û��ͼ��
#define OBJECT_NO_PIC			(4)
//�������ִ���䴰�ڲ���
#define PARAS_HASWINDOW			(5)
//�������ô���
#define PARAS_ERROR				(6)
//û�ж�ȡ�����
#define PARAS_NOCAMERA			(7)



#ifndef hog_width
#define hog_width 128
#endif 
#ifndef hog_height
#define hog_height 128
#endif

//#define _TEST
