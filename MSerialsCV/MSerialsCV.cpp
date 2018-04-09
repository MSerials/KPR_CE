#include "stdafx.h"
#include "MSerialsCV.h"
#include "MSerials.h"
#include "../MSerialsCommon/Tools.h"
using namespace cv;




void cv_set_params(const int w, const int h)
{
	MSerials::GetIns()->cv_set_params(w, h);
}

//×¢ÒâÐèÒªËø
float cv_check_pinhole(const int w, const int h, const unsigned char * data, int ch)
{
	cv::Mat image;
	if (1 == ch)
	{
		image = cv::Mat(w, h, CV_8UC1);
		memcpy(image.data, data, w*h);
	}
	else
	{
		image = cv::Mat(w, h, CV_8UC3);
		memcpy(image.data, data, w*h * 3);
	}
	return MSerials::GetIns()->cv_check_pinhole(image);
}



float cv_train_pinhole(const char* front_imgs_path, const char* back_imgs_path,char* path, bool isFlip)
{
	win_tool wt;
	std::vector<CString> fp = wt.SplitCString((CString)(CStringA)(LPCSTR)front_imgs_path, L"|");
	std::vector<CString> bp = wt.SplitCString((CString)(CStringA)(LPCSTR)back_imgs_path, L"|");
	CString strPath = (CString)(CStringA)(LPCSTR)path;
	return MSerials::GetIns()->cv_train_pinhole(fp, bp, strPath, isFlip);
}


float cv_train_inv(const char* front_imgs_path, const char* back_imgs_path, cv::Rect rt, char* xml_store_path, bool isFlip)
{
	win_tool wt;
	std::vector<CString> fp = wt.SplitCString((CString)(CStringA)(LPCSTR)front_imgs_path, L"|");
	std::vector<CString> bp = wt.SplitCString((CString)(CStringA)(LPCSTR)back_imgs_path, L"|");
	CString strPath = (CString)(CStringA)(LPCSTR)xml_store_path;
	return MSerials::GetIns()->cv_train_inv(fp, bp, rt, strPath, isFlip);
}

float cv_predict_inv(const cv::Mat & src, cv::Rect rt, bool isFlip)
{
	return MSerials::GetIns()->cv_predict_inv(src,rt,isFlip);
}

bool load_train_data(const char * path)
{
	CString str(path);
	return MSerials::GetIns()->load_train_data(str);
}


std::vector<cv::Rect> cv_selective_search(const cv::Mat & src, size_t &found_rect)
{
	return MSerials::GetIns()->selective_search(src, found_rect);
}

void selective_draw(cv::Mat & src, const std::vector<cv::Rect>& found_rect)
{
	return MSerials::GetIns()->selective_draw(src, found_rect);
}