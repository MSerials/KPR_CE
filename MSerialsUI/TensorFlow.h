#pragma once

#if 0

#ifndef __TENSORFLOW__
#define __TENSORFLOW__

#define DLL_EXPORT

#ifdef DLL_EXPORT
#define TENSERFLOWDLL __declspec(dllexport)
#else
#define TENSERFLOWDLL __declspec(dllimport)
#endif
#ifdef _WIN64
#pragma comment(lib,"..\\x64\\Release\\MSerialsTF.lib")
#else

#endif

TENSERFLOWDLL const char* Get_ErrorInfo();
TENSERFLOWDLL const char* Get_Version();
TENSERFLOWDLL const char* Get_Vendor();
TENSERFLOWDLL const char* Get_AuthorList();
TENSERFLOWDLL int	load_image(const int rows, const int cols, const int ch, const unsigned char * data);
TENSERFLOWDLL int	load_image_by_path(const char * path);
TENSERFLOWDLL int	ReLoadScript();
TENSERFLOWDLL int	VarsFunction(char * command);


#endif

#endif