#include "stdafx.h"
#include "tensorflow.h"
#include "../MSerialsCommon/common.h"
#include <vector>
//�����飬��Ϊstring��֧��unicode��ȡ����֮����ATL
#include <string>
//�ɱ����������Ҫʹ��
#include <mutex>

MSerial::TensorFlow::TensorFlow()
{
	//python
//	pName = NULL; pModule = NULL; pDict = NULL;// pFunc = NULL;
	//
	//py�ű���ʼ��
	pModule = NULL;

	error_infomation = INIT_LOADSCRIPTERROR;

	m_str = NULL;

	Init();
}

MSerial::TensorFlow::~TensorFlow()
{
	free(m_str);
	// �ر�Python  
	Py_Finalize();
}

const char *  MSerial::TensorFlow::ErrorInfo()
{
	CString str_error = L"û�м�⵽��������";

	if (error_infomation == INIT_LOADSCRIPTERROR) {
#ifdef _TEST
		printf("����ű�ʧ��\n");
#endif // _TEST
		return "����ű�ʧ��\n";}

	if (error_infomation == INIT_SUCCESS) {
#ifdef _TEST
		printf("����ű��ɹ�\n");
#endif // _TEST
		return "����ű��ɹ�\n";
	}

	return "û�м�⵽��������";
}

int MSerial::TensorFlow::ErrorCode()
{
	return error_infomation;
}

int MSerial::TensorFlow::ReLoadScript()
{
	if(NULL != pModule) Py_DECREF(pModule);
	return LoadScript("Algorithm");
}

int MSerial::TensorFlow::LoadScript(const char * script_name)
{
	PyObject *pName = PyUnicode_DecodeFSDefault(script_name);
	/* Error checking of pName left out */
	pModule = PyImport_Import(pName);
	if (NULL == pModule)
		return (error_infomation = INIT_LOADSCRIPTERROR);
	Py_DECREF(pName);
	//all you need is to write something here
	return (error_infomation = INIT_SUCCESS);
}


int MSerial::TensorFlow::Init()
{
	Py_Initialize();
	return LoadScript("Algorithm");
	/*
	This code loads a Python script using argv[1], and calls the function named in argv[2]. Its integer arguments are the other values of the argv array. If you compile and link this program (let��s call the finished executable call), and use it to execute a Python script, such as:

def multiply(a,b):
	*/

	//this code is from python.org
}


const char* MSerial::TensorFlow::Get_AuthroList()
{
	//ע�����
	puts("hao keng\n");
	CString List = L"����";
	return (char*)(LPCSTR)(CStringA)List;
}

int MSerial::TensorFlow::load_image(const cv::Mat &src)
{
#ifdef _TEST
	puts("dsf\n");
#endif
	return 0;
}




int MSerial::TensorFlow::load_image_by_path(const char * path)
{
	PyObject *pName = NULL, *pDict = NULL, *pFunc = NULL;
	PyObject *pArgs = NULL, *pValue = NULL;
#ifdef _TEST
	puts(path);
#endif
		if (pModule != NULL) 
		{
			pFunc = PyObject_GetAttrString(pModule, "imread");
			/* pFunc is a new reference */
			if (pFunc && PyCallable_Check(pFunc)) {
				pArgs = PyTuple_New(1);
					pValue = PyBytes_FromString(path);
				//	pValue = PyUnicode_FromFormat(path);
					if (!pValue) {
						Py_DECREF(pArgs);
				//		Py_DECREF(pModule);
						fprintf(stderr, "Cannot convert argument\n");
						return 1;
					}
				PyTuple_SetItem(pArgs, 0, pValue);
					/* pValue reference stolen here: */
				pValue = PyObject_CallObject(pFunc, pArgs);
				Py_DECREF(pArgs);
				if (pValue != NULL) {
					printf("read image success\n"); 
					Py_DECREF(pValue);
				}
				else {
					Py_DECREF(pFunc);
				//	Py_DECREF(pModule);
					PyErr_Print();
					return 1;
				}
				
			}
			else {
				if (PyErr_Occurred())
					PyErr_Print();
			}
			Py_XDECREF(pFunc);
			//Py_DECREF(pModule);
		}
		else {
			PyErr_Print();
			return INIT_LOADSCRIPTERROR;
		}
	return NoError;
}

//�ɱ�������������ڵ��ÿɱ����ʱ�����python�õ�
int MSerial::TensorFlow::VarsFunction(char * command)
{
#if 1
	//static std::mutex mtx;
	//std::lock_guard<std::mutex> lck(mtx);
	static win_tool wt;
	std::vector<CString> argv = wt.SplitCString((CString)(CStringA)(LPCSTR)command, L" ");
	int argc = static_cast<int>(argv.size());
	if (!argc)	
	{ 
#ifdef _TEST
		printf("lost string\n");
#endif
		return PARA_ERROR;
	}
	PyObject *pName = NULL, *pDict = NULL, *pFunc = NULL;
	PyObject *pArgs = NULL, *pValue = NULL;
	try {
		
		if (pModule != NULL) {
			pFunc = PyObject_GetAttrString(pModule, (char*)(LPCSTR)(CStringA)argv[0]);
			/* pFunc is a new reference */
			if (pFunc && PyCallable_Check(pFunc)) {
				pArgs = PyTuple_New(argc - 1);
				for (int i = 0; i < (argc - 1); ++i) {
					pValue = PyLong_FromLong(_ttol(argv[i + 1]));
					//printf("params: %ld\n", PyLong_AsLong(pValue));
					if (!pValue) {
						Py_DECREF(pArgs);
			//			Py_DECREF(pModule);
						fprintf(stderr, "Cannot convert argument\n");
						return 1;
					}
					/* pValue reference stolen here: */
					PyTuple_SetItem(pArgs, i, pValue);
				}
				pValue = PyObject_CallObject(pFunc, pArgs);
				Py_DECREF(pArgs);
				if (pValue != NULL) {
					printf("Result of call: %ld\n", PyLong_AsLong(pValue));
					Py_DECREF(pValue);
				}
				else {
					printf("I dont know what happen %ld\n", PyLong_AsLong(pValue));
					Py_DECREF(pFunc);
			//		Py_DECREF(pModule);
					PyErr_Print();
#ifdef _TEST
					//printf("Cannot find failed\n");
#endif
					return 1;
				}
			}
			else {
				if (PyErr_Occurred())
					PyErr_Print();
#ifdef _TEST
				printf("Cannot find funciton \"  %s \"\n", (char*)(LPCSTR)(CStringA)argv[0]);
#endif
			}
			//Py_XDECREF(pFunc);
			Py_DECREF(pFunc);
		//	Py_DECREF(pModule);
		}
		else {
			PyErr_Print();
#ifdef _TEST
			puts("failed loat script\n");
#endif
			return 1;
		}
	}
	catch (PyObject & except)
	{
#ifdef _TEST
		puts("failed to exe script\n");
#endif
	
		return 1;
	}
#endif
	return 0;
}