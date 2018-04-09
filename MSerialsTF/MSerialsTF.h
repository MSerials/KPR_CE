#pragma once
#include <stdarg.h>


#define DLL_EXPORT

#ifdef DLL_EXPORT
#define TENSERFLOWDLL __declspec(dllexport)
#else
#define TENSERFLOWDLL __declspec(dllimport)
#endif

TENSERFLOWDLL const char* Get_ErrorInfo();
TENSERFLOWDLL const char* Get_Version();
TENSERFLOWDLL const char* Get_Vendor();
TENSERFLOWDLL const char* Get_AuthorList();
TENSERFLOWDLL int	load_image(const int rows, const int cols, const int ch, const unsigned char * data);
TENSERFLOWDLL int	load_image_by_path(const char * path);
TENSERFLOWDLL int	ReLoadScript();
TENSERFLOWDLL int	VarsFunction(char * command);
