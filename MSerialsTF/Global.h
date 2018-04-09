#pragma once

#ifdef _WIN64
#include <atlstr.h>
#include <atlwin.h>
#include "Python.h"
//the following code is already in pyconfig.h
//#pragma comment(lib,"python35.lib")
#else


#endif


#include "opencv.hpp"
#ifdef _WIN64
#pragma comment(lib,"../third_party/opencv/x64/opencv_world320.lib")
#endif