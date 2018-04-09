#include "stdafx.h"
#include "MSerialsCam.h"
#include "ImageCard.h"
//#include "..\MSerialsUI\MSerialsCam.h"



int enum_cameras(int method)
{
	return CImageCard::GetIns()->RefreshList(method);
}

int get_vendors_num()
{
	return CImageCard::GetIns()->get_vendors_num();
}

const char * get_sdk_ver_by_vendor(int _vendor)
{
	return CImageCard::GetIns()->get_sdk_ver_by_vendor(_vendor);
}

int get_cameras_num_by_vendor(int _vendor)
{
	return CImageCard::GetIns()->get_cameras_num_by_vendor(_vendor);
}

const char * get_vendor_name(int manufactory)
{
	return CImageCard::GetIns()->get_vendor_name(manufactory);
}

const char * get_camera_info_by_index(int manufactory, int camera_idx)
{
	return CImageCard::GetIns()->get_camera_info_by_index( manufactory,  camera_idx);
}

int Snap(int & width, int & height, unsigned char ** data, int &ch, int manufactory,int camera_index, int delay)
{
	return CImageCard::GetIns()->Snap(width, height, data, ch, manufactory, camera_index, delay);
}

void close_device()
{
	CImageCard::GetIns()->Close();
}
/*
const char * get_vendor(int manufactory)
{
	return CImageCard::GetIns()->get_vendor(manufactory);
}
*/

