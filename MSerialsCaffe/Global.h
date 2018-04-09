#pragma once

//#include "caffe.hpp"

#ifdef _MSC_VER
//1800 vc12
#if _MSC_VER > 1800			
	
#ifdef _WIN64
#pragma comment(lib,"lib\\caffe.lib")
#pragma comment(lib,"lib\\caffeproto.lib")
#pragma comment(lib,"lib\\opencv_world310.lib")
#else

#endif
#endif


#endif