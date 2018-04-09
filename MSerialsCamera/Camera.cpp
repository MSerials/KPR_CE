#include "stdafx.h"
#include "Camera.h"
#include "../MSerialsCommon/common.h"
#include <iostream>

Camera::Camera()
{
} 


#if 1
Camera::~Camera()
{
#ifdef _TEST
	std::cout << "free camera memeory in MSerialsCamera class" << std::endl;
#endif // _TEST
}
#endif
