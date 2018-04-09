#pragma once
#include <opencv.hpp>
#include <vector>
#include <atlstr.h>
class MSerials
{
private:
	MSerials();
	int init();
public:
	static MSerials* GetIns()
	{
		static MSerials l_ml;
		return &l_ml;
	}

	void cv_set_params(const int w, const int h);

	bool load_train_data(CString prj_dir = L"");

	float cv_check_pinhole(cv::Mat & src);

	float cv_train_pinhole(std::vector<CString> front_imgs, std::vector<CString> back_imgs, CString xml_store_path = L"", bool isFlip = false);

	float cv_train_inv(std::vector<CString> front_imgs, std::vector<CString> back_imgs,cv::Rect rt, CString xml_store_path = L"", bool isFlip = false);

	float cv_predict_inv(const cv::Mat& src, cv::Rect rt, bool isFlip = false);

	std::vector<cv::Rect> selective_search(const cv::Mat & src, size_t &found_rect);

	void selective_draw(cv::Mat & src, const std::vector<cv::Rect>& found_rect);

	void get_svm_detector(const cv::Ptr<cv::ml::SVM>& svm, std::vector<float>& hog_detector);

	std::vector<cv::Rect> locations_find(cv::Mat & src, const cv::Rect rt);

private:
	cv::HOGDescriptor HOGDetect;
	cv::Ptr<cv::ml::SVM> svm;
	std::vector<cv::KeyPoint> keypoints;
	cv::Ptr<cv::ml::KNearest> knn;
	bool isSetHog;
	//检查图片是否可读
	std::vector<CString> check_imgs_path(std::vector<CString> path);
	//参数
	//支持向量机分类
	float hog_svm_predict(const cv::Mat & src);
	float hog_svm_train(std::vector<CString> front_imgs, std::vector<CString> back_imgs, CString xml_store_path = L"", bool isFlip = false);
	void draw_locations(cv::Mat & img, const std::vector<cv::Rect>& locations, const cv::Scalar & color);
	
	UINT set_hog_detect_descript(CString path=L"");

};


