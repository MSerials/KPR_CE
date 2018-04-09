#pragma once

#include "Global.h"
#include <vector>
#include <string>


namespace MSerial
{
	class TensorFlow
	{
	private:
		TensorFlow();
		~TensorFlow();

		int Init();
		long error_infomation;

		char * m_str;

	public:
		//
		static TensorFlow * GetIns() { static TensorFlow l_tf; return &l_tf; }
		//
		const char * ErrorInfo();
		//
		int ErrorCode();

		int ReLoadScript();
		//
		int LoadScript(const char * script_name);
		//
		
		//
		const char* Get_AuthroList();

		int load_image(const cv::Mat & src);

		int load_image_by_path(const char * path);

		int VarsFunction(char * command);


	private:
		PyObject *pModule;
		//PyObject *pName, *pModule, *pDict;
		//PyObject *pArgs, *pValue;

	
	};
}
