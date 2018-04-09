#include "baumer.h"
#include "stdafx.h"
int Baumer::error_info()
{
	return 0;
}

const char * Baumer::vendor()
{
	return nullptr;
}

const char * Baumer::CamerasInfo()
{
	return nullptr;
}

int Baumer::device_quantity()
{
	return 0;
}

int Baumer::Snap(int & width, int & height, unsigned char ** data, int & ch, int camera_index, int delay)
{
	return 0;
}

void Baumer::set_exposure(double exposure, int camera_index)
{
}
