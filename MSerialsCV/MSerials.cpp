#include "stdafx.h"
#include "MSerials.h"
#include "../MSerialsCommon/Tools.h"
#include "../MSerialsCommon/common.h"
#include <iostream>
#include <string>

using namespace std;
using namespace cv;
using namespace cv::ml;

MSerials::MSerials()
{
	init();
}

int MSerials::init()
{
	isSetHog = false;

	if (svm.empty())
	{
		static int timelimt = 0;
		//对svm进行初始化
		svm = cv::ml::SVM::create();
		svm->setType(cv::ml::SVM::C_SVC);
		svm->setKernel(cv::ml::SVM::LINEAR);//注意必须使用线性SVM进行训练，因为HogDescriptor检测函数只支持线性检测！！！
		svm->setTermCriteria(cv::TermCriteria(CV_TERMCRIT_ITER, 1000, FLT_EPSILON));
		if (++timelimt > 10)
		{
			return -1;
		}
	}
	return 0;
}

void MSerials::cv_set_params(const int w, const int h)
{
}

float MSerials::cv_check_pinhole(cv::Mat & src)
{
	return hog_svm_predict(src);
}

float MSerials::hog_svm_train(std::vector<CString> front_imgs, std::vector<CString> back_imgs, CString xml_store_path, bool isFlip)
{
	//程序可能有bug 看图片是否需要翻转
	using namespace std;
	std::vector<CString> pos_path = check_imgs_path(front_imgs);
	std::vector<CString> neg_path = check_imgs_path(back_imgs);
	int PosSamNO = pos_path.size();
	int NegSamNO = neg_path.size();
	if (0 == PosSamNO || 0 == NegSamNO) return -2.0f;
	try {
		//hog_width
		HOGDescriptor hog(Size(hog_width, hog_height), Size(16, 16), Size(8, 8), Size(8, 8), 9);//HOG检测器，用来计算HOG描述子的  
		int DescriptorDim;//HOG描述子的维数，由图片大小、检测窗口大小、块大小、细胞单元中直方图bin个数决定  

		Mat sampleFeatureMat;//所有训练样本的特征向量组成的矩阵，行数等于所有样本的个数，列数等于HOG描述子维数      
		Mat sampleLabelMat;//训练样本的类别向量，行数等于所有样本的个数，列数等于1；1表示有人，-1表示无人  
						   //依次读取正样本图片，生成HOG描述子 
		for (int num = 0; num < PosSamNO; num++)
		{
			Mat srcimg = imread((LPCSTR)CStringA(pos_path[num]));//读取图片
			Mat src;
			if (isFlip) flip(srcimg, src, -1); else src = srcimg.clone();
			resize(src, src, Size(hog_width, hog_height));
			vector<float> descriptors;//HOG描述子向量  
			hog.compute(src, descriptors, Size(8, 8));

#ifdef _TEST
			cout << descriptors.size() << endl;
#endif // _DEBUG

			//处理第一个样本时初始化特征向量矩阵和类别矩阵，因为只有知道了特征向量的维数才能初始化特征向量矩阵  
			if (0 == num)
			{
				DescriptorDim = descriptors.size();//HOG描述子的维数  
												   //初始化所有训练样本的特征向量组成的矩阵，行数等于所有样本的个数，列数等于HOG描述子维数sampleFeatureMat  
				sampleFeatureMat = Mat::zeros(PosSamNO + NegSamNO, DescriptorDim, CV_32FC1);
				//初始化训练样本的类别向量，行数等于所有样本的个数，列数等于1；1表示有人，0表示无人  
				sampleLabelMat = Mat::zeros(PosSamNO + NegSamNO, 1, CV_32SC1);
			}

			//将计算好的HOG描述子复制到样本特征矩阵sampleFeatureMat  
			for (int i = 0; i<DescriptorDim; i++)
				sampleFeatureMat.at<float>(num, i) = descriptors[i];//第num个样本的特征向量中的第i个元素  
			sampleLabelMat.at<int>(num, 0) = 1;//正样本类别为1，有人  
		}

		//依次读取负样本图片，生成HOG描述子  
		for (int num = 0; num<NegSamNO; num++)
		{
			Mat srcimg = imread((LPCSTR)CStringA(neg_path[num]));//读取图片
			Mat src;
			if (isFlip) flip(srcimg, src, -1); else src = srcimg.clone();
			resize(src, src, Size(hog_width, hog_height));

			vector<float> descriptors;//HOG描述子向量  
			hog.compute(src, descriptors, Size(8, 8));//计算HOG描述子，检测窗口移动步长(8,8) 
													  //将计算好的HOG描述子复制到样本特征矩阵sampleFeatureMat  
			for (int i = 0; i<DescriptorDim; i++)
				sampleFeatureMat.at<float>(num + PosSamNO, i) = descriptors[i];//第PosSamNO+num个样本的特征向量中的第i个元素  
			sampleLabelMat.at<int>(num + PosSamNO, 0) = -1;//负样本类别为-1，无人  
		}

		CString filePath = xml_store_path + L"\\traindata.xml";
		std::string trainPath = (LPCSTR)CStringA(filePath);

		//使用SVM学习         
		svm->train(sampleFeatureMat, ROW_SAMPLE, sampleLabelMat);
		svm->save(trainPath);
		//设置检测子
		set_hog_detect_descript();
		return 0.0f;
	}
	catch (...)
	{
		return -1.0f;
	}
	return 0.0f;
}

float MSerials::cv_train_pinhole(std::vector<CString> front_imgs, std::vector<CString> back_imgs, CString xml_store_path, bool isFlip)
{
	return hog_svm_train(front_imgs,back_imgs,xml_store_path, isFlip);
}

float MSerials::cv_train_inv(std::vector<CString> front_imgs, std::vector<CString> back_imgs, cv::Rect rt, CString xml_store_path, bool isFlip)
{
	//程序可能有bug 看图片是否需要翻转
#include <iostream>
	using namespace std;
#ifdef _TEST
	std::ofstream fout("record.txt");
	fout << "initial cv_train_inv" << endl;
#endif
	std::vector<CString> pos_path = check_imgs_path(front_imgs);
	std::vector<CString> neg_path = check_imgs_path(back_imgs);
	int PosSamNO = pos_path.size();
	int NegSamNO = neg_path.size();
	
	if (0 == PosSamNO || 0 == NegSamNO) return -2.0f;
	try {
#ifdef _TEST
		fout <<"start record" << endl;
#endif
		//hog_width
		HOGDescriptor hog(Size(hog_width, hog_height), Size(16, 16), Size(8, 8), Size(8, 8), 9);//HOG检测器，用来计算HOG描述子的  
		int DescriptorDim;//HOG描述子的维数，由图片大小、检测窗口大小、块大小、细胞单元中直方图bin个数决定  

		Mat sampleFeatureMat;//所有训练样本的特征向量组成的矩阵，行数等于所有样本的个数，列数等于HOG描述子维数      
		Mat sampleLabelMat;//训练样本的类别向量，行数等于所有样本的个数，列数等于1；1表示有人，-1表示无人  
						   //依次读取正样本图片，生成HOG描述子 
		for (int num = 0; num < PosSamNO; num++)
		{
			Mat srcimg = imread((char*)(LPCSTR)CStringA(pos_path[num]));//读取图片
			Mat srci,src;
			if (isFlip) flip(srcimg, srci, -1); else srci = srcimg.clone();
			srci(rt).copyTo(src);
			resize(src, src, Size(hog_width, hog_height));
			vector<float> descriptors;//HOG描述子向量  
			hog.compute(src, descriptors, Size(8, 8));

#ifdef _TEST
			fout <<"front descriptor "<< descriptors.size() << endl;
#endif // _DEBUG
			
			//处理第一个样本时初始化特征向量矩阵和类别矩阵，因为只有知道了特征向量的维数才能初始化特征向量矩阵  
			if (0 == num)
			{
				DescriptorDim = descriptors.size();//HOG描述子的维数  
												   //初始化所有训练样本的特征向量组成的矩阵，行数等于所有样本的个数，列数等于HOG描述子维数sampleFeatureMat  
				sampleFeatureMat = Mat::zeros(PosSamNO + NegSamNO, DescriptorDim, CV_32FC1);
				//初始化训练样本的类别向量，行数等于所有样本的个数，列数等于1；1表示有人，0表示无人  
				sampleLabelMat = Mat::zeros(PosSamNO + NegSamNO, 1, CV_32SC1);
			}

			//将计算好的HOG描述子复制到样本特征矩阵sampleFeatureMat  
			for (int i = 0; i<DescriptorDim; i++)
				sampleFeatureMat.at<float>(num, i) = descriptors[i];//第num个样本的特征向量中的第i个元素  
			sampleLabelMat.at<int>(num, 0) = 1;//正样本类别为1，有人  
		}
		//依次读取负样本图片，生成HOG描述子  
		for (int num = 0; num<NegSamNO; num++)
		{
			Mat srcimg = imread((char*)(LPCSTR)CStringA(neg_path[num]));//读取图片
			Mat srci, src;
			if (isFlip) flip(srcimg, srci, -1); else srci = srcimg.clone();
			srci(rt).copyTo(src);
			resize(src, src, Size(hog_width, hog_height));
			vector<float> descriptors;//HOG描述子向量  
			hog.compute(src, descriptors, Size(8, 8));//计算HOG描述子，检测窗口移动步长(8,8) 
													  //将计算好的HOG描述子复制到样本特征矩阵sampleFeatureMat  

#ifdef _TEST
			fout << "back descriptor " << descriptors.size() << endl;
#endif // _DEBUG

			for (int i = 0; i<DescriptorDim; i++)
				sampleFeatureMat.at<float>(num + PosSamNO, i) = descriptors[i];//第PosSamNO+num个样本的特征向量中的第i个元素  
			sampleLabelMat.at<int>(num + PosSamNO, 0) = -1;//负样本类别为-1，无人  
		}

#ifdef _TEST
		fout << "sfm typeis " << sampleFeatureMat.type() << " rows:" <<sampleFeatureMat.rows << "cols:"<< sampleFeatureMat.cols << endl;
		fout << "slm typeis " << sampleLabelMat.type() <<"rows:" << sampleLabelMat.rows <<"cols:" <<sampleLabelMat.cols << endl;
		if(svm.empty()) fout << "svm is empty " << endl;
#endif // _DEBUG

		if (0 != init()) return -3.0;
		//使用SVM学习  
		svm->train(sampleFeatureMat, ROW_SAMPLE, sampleLabelMat);
		CString filePath = xml_store_path + "\\traindata.xml";
		std::string trainPath = (LPCSTR)CStringA(filePath);
		svm->save(trainPath);
		//设置检测子
		//set_hog_detect_descript(xml_store_path);
		return 0.0f;
	}
	catch (...)
	{
		return -1.0f;
	}
	return 0.0f;
}

float MSerials::cv_predict_inv(const cv::Mat & src, cv::Rect rt, bool isFlip)
{
	try{
	cv::Mat timg,img;
	src(rt).copyTo(timg);	//的定义是Rect，复制感兴趣区域 Rect ImgROI;
	if (isFlip) flip(timg, img,-1); else img = timg.clone();
	return hog_svm_predict(img);
	}
	catch (...)
	{
		return 0.0;
	}
}





float MSerials::hog_svm_predict(const cv::Mat& src)
{
	try {
		if (src.empty()) return 0;
		if (svm.empty()) { return 0; }
		//	HOGDescriptor hog(Size(64, 128), Size(16, 16), Size(8, 8), Size(8, 8), 9);//HOG检测器，用来计算HOG描述子的  
		int DescriptorDim;//HOG描述子的维数，由图片大小、检测窗口大小、块大小、细胞单元中直方图bin个数决定  
		Mat res,img;
		Mat sampleFeatureMat;
		resize(src, img, Size(hog_width,  hog_height));
		HOGDescriptor hog(Size(hog_width, hog_height), Size(16, 16), Size(8, 8), Size(8, 8), 9);//HOG检测器，用来计算HOG描述子的  

		vector<float> descriptors;//HOG描述子向量 
//		descriptors.clear();
		hog.compute(img, descriptors, Size(8, 8));//计算HOG描述子，检测窗口移动步长(8,8)  
		DescriptorDim = descriptors.size();//HOG描述子的维数  
										   //初始化所有训练样本的特征向量组成的矩阵，行数等于所有样本的个数，列数等于HOG描述子维数sampleFeatureMat  
		sampleFeatureMat = Mat::zeros(1, DescriptorDim, CV_32FC1);
		//初始化训练样本的类别向量，行数等于所有样本的个数，列数等于1；1表示有人，0表示无人  

		//将计算好的HOG描述子复制到样本特征矩阵sampleFeatureMat  
		for (int i = 0; i < DescriptorDim; i++)
			sampleFeatureMat.at<float>(0, i) = descriptors[i];//第num个样本的特征向量中的第i个元素  
		svm->predict(sampleFeatureMat, res);
		if (res.empty()) return 0;
		return res.at<float>(0, 0);
	}
	catch (...)
	{
		return 0.0f;
	}

	return 0.0f;
}



std::vector<cv::Rect> MSerials::selective_search(const cv::Mat & src, size_t &found_rect)
{

	std::vector<cv::Rect> found, found_filtered;//矩形框数组  
	HOGDetect.detectMultiScale(src, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);//对图片进行多尺度行人检测  

	//找出所有没有嵌套的矩形框r,并放入found_filtered中,如果有嵌套的话,则取外面最大的那个矩形框放入found_filtered中  
	for (int i = 0; i < found.size(); i++)
	{
		Rect r = found[i];
		int j = 0;
		for (; j < found.size(); j++)
			if (j != i && (r & found[j]) == r)
				break;
		if (j == found.size())
			found_filtered.push_back(r);
	}
	found_rect = found.size();
	return found_filtered;
#if 0
//画矩形框，因为hog检测出的矩形框比实际人体框要稍微大些,所以这里需要做一些调整  
for (int i = 0; i < found_filtered.size(); i++)
{
	Rect r = found_filtered[i];
	r.x += cvRound(r.width*0.1);
	r.width = cvRound(r.width*0.8);
	r.y += cvRound(r.height*0.07);
	r.height = cvRound(r.height*0.8);
}	cv::rectangle(src, r.tl(), r.br(), Scalar(0, 0, 255), 3);

#endif
}

void MSerials::selective_draw(cv::Mat & src, const std::vector<cv::Rect> &found_rect)
{
	for (int i = 0; i < found_rect.size(); i++)
	{
		cv::Rect r = found_rect[i];
		cv::rectangle(src, r.tl(), r.br(), Scalar(0, 0, 255), 3);
	}
}

void MSerials::get_svm_detector(const cv::Ptr<cv::ml::SVM>& svm, std::vector< float > & hog_detector)
{
	// get the support vectors
	Mat sv = svm->getSupportVectors();
	const int sv_total = sv.rows;
	// get the decision function
	Mat alpha, svidx;
	double rho = svm->getDecisionFunction(0, alpha, svidx);

	CV_Assert(alpha.total() == 1 && svidx.total() == 1 && sv_total == 1);
	CV_Assert((alpha.type() == CV_64F && alpha.at<double>(0) == 1.) ||
		(alpha.type() == CV_32F && alpha.at<float>(0) == 1.f));
	CV_Assert(sv.type() == CV_32F);
	hog_detector.clear();

	hog_detector.resize(sv.cols + 1);
	memcpy(&hog_detector[0], sv.ptr(), sv.cols * sizeof(hog_detector[0]));
	hog_detector[sv.cols] = (float)-rho;
}


void MSerials::draw_locations(cv::Mat & img, const std::vector< cv::Rect > & locations, const cv::Scalar & color)
{
	if (!locations.empty())
	{
		vector< Rect >::const_iterator loc = locations.begin();
		vector< Rect >::const_iterator end = locations.end();
		for (; loc != end; ++loc)
		{
			rectangle(img, *loc, color, 2);
		}
	}
}

std::vector<cv::Rect> MSerials::locations_find(cv::Mat & src, const cv::Rect rt)
{
	std::vector< cv::Rect > locations;
	if (!isSetHog) return locations;
	HOGDetect.detectMultiScale(src, locations);
	Scalar trained(0, 0, 255);
	draw_locations(src, locations, trained);
	return locations;
}

UINT MSerials::set_hog_detect_descript(CString path)
{

	/*************************************************************************************************
	线性SVM训练完成后得到的XML文件里面，有一个数组，叫做support vector，还有一个数组，叫做alpha,有一个浮点数，叫做rho;
	将alpha矩阵同support vector相乘，注意，alpha*supportVector,将得到一个列向量。之后，再该列向量的最后添加一个元素rho。
	如此，变得到了一个分类器，利用该分类器，直接替换opencv中行人检测默认的那个分类器（cv::HOGDescriptor::setSVMDetector()），
	就可以利用你的训练样本训练出来的分类器进行行人检测了。
	***************************************************************************************************/
	CString fileName = path + L"\\traindata.xml";
	cv::Ptr<cv::ml::SVM> lsvm = StatModel::load<SVM>((std::string)(CStringA)fileName); //或者svm = Statmodel::load<SVM>("SVM_HOG.xml"); static function

																					   // svm->load<SVM>("SVM_HOG.xml"); 这样使用不行
	fileName = path + L"\\hogDescript.txt";
	if (lsvm->empty())
	{
		return -1;
	}
	vector< float > hog_detector;
	get_svm_detector(lsvm, hog_detector);
	ofstream cout(fileName);
	for (size_t i = 0; i < hog_detector.size(); i++)
	{
		cout << hog_detector[i] << endl;
	}
	HOGDetect.winSize = Size(hog_width, hog_height); //缺失也会翻车
	HOGDetect.setSVMDetector(hog_detector);
	isSetHog = true;
	return 0;
}


bool MSerials::load_train_data(CString prj_dir)
{
	if (0 != init()) return false;
	CString path = prj_dir + L"\\traindata.xml";
	try {
		svm = Algorithm::load<SVM>((String)(CStringA)path);
	}
	catch (...)
	{
		return false;
	}
	return true;
}


std::vector<CString> MSerials::check_imgs_path(std::vector<CString> path)
{
	std::vector<CString> imgs;
	for (size_t i = 0; i < path.size(); i++)
	{
		Mat src = imread((char*)(LPCSTR)CStringA(path[i]));
		if (!src.empty())
		{
			imgs.push_back(path[i]);
		}
	}
	return imgs;
}


