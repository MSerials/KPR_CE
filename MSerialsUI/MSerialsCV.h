#pragma once
#ifndef __OPENCV__
#define __OPENCV__

#ifdef DLL_EXPORT
#define CVDLL __declspec(dllexport)
//#define CVDLLC __declspec(dllexport)
#else
#define CVDLL __declspec(dllimport)
#endif

#include <opencv.hpp>
#include <vector>
#include <atlstr.h>

#ifdef _WIN64
#pragma comment(lib,"..\\x64\\Release\\MSerialsCV.lib")
#else
#error can not support x86 system
#endif

CVDLL void cv_set_params(const int w, const int h);
CVDLL float cv_check_pinhole(const int w, const int h, const unsigned char *data, int ch = 1);
CVDLL float cv_train_pinhole(const char* front_imgs_path, const char* back_imgs_path, char* path = "\0", bool isFlip = false);
CVDLL float cv_train_inv(const char* front_imgs_path, const char* back_imgs_path, cv::Rect rt, char* xml_store_path = "\0", bool isFlip = false);
CVDLL float cv_predict_inv(const cv::Mat & src, cv::Rect rt, bool isFlip = false);
CVDLL bool load_train_data(const char* path);

CVDLL std::vector<cv::Rect> cv_selective_search(const cv::Mat & src, size_t &found_rect);
CVDLL void selective_draw(cv::Mat & src, const std::vector<cv::Rect>& found_rect);
#endif