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
		//��svm���г�ʼ��
		svm = cv::ml::SVM::create();
		svm->setType(cv::ml::SVM::C_SVC);
		svm->setKernel(cv::ml::SVM::LINEAR);//ע�����ʹ������SVM����ѵ������ΪHogDescriptor��⺯��ֻ֧�����Լ�⣡����
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
	//���������bug ��ͼƬ�Ƿ���Ҫ��ת
	using namespace std;
	std::vector<CString> pos_path = check_imgs_path(front_imgs);
	std::vector<CString> neg_path = check_imgs_path(back_imgs);
	int PosSamNO = pos_path.size();
	int NegSamNO = neg_path.size();
	if (0 == PosSamNO || 0 == NegSamNO) return -2.0f;
	try {
		//hog_width
		HOGDescriptor hog(Size(hog_width, hog_height), Size(16, 16), Size(8, 8), Size(8, 8), 9);//HOG���������������HOG�����ӵ�  
		int DescriptorDim;//HOG�����ӵ�ά������ͼƬ��С����ⴰ�ڴ�С�����С��ϸ����Ԫ��ֱ��ͼbin��������  

		Mat sampleFeatureMat;//����ѵ������������������ɵľ��������������������ĸ�������������HOG������ά��      
		Mat sampleLabelMat;//ѵ����������������������������������ĸ�������������1��1��ʾ���ˣ�-1��ʾ����  
						   //���ζ�ȡ������ͼƬ������HOG������ 
		for (int num = 0; num < PosSamNO; num++)
		{
			Mat srcimg = imread((LPCSTR)CStringA(pos_path[num]));//��ȡͼƬ
			Mat src;
			if (isFlip) flip(srcimg, src, -1); else src = srcimg.clone();
			resize(src, src, Size(hog_width, hog_height));
			vector<float> descriptors;//HOG����������  
			hog.compute(src, descriptors, Size(8, 8));

#ifdef _TEST
			cout << descriptors.size() << endl;
#endif // _DEBUG

			//�����һ������ʱ��ʼ�����������������������Ϊֻ��֪��������������ά�����ܳ�ʼ��������������  
			if (0 == num)
			{
				DescriptorDim = descriptors.size();//HOG�����ӵ�ά��  
												   //��ʼ������ѵ������������������ɵľ��������������������ĸ�������������HOG������ά��sampleFeatureMat  
				sampleFeatureMat = Mat::zeros(PosSamNO + NegSamNO, DescriptorDim, CV_32FC1);
				//��ʼ��ѵ����������������������������������ĸ�������������1��1��ʾ���ˣ�0��ʾ����  
				sampleLabelMat = Mat::zeros(PosSamNO + NegSamNO, 1, CV_32SC1);
			}

			//������õ�HOG�����Ӹ��Ƶ�������������sampleFeatureMat  
			for (int i = 0; i<DescriptorDim; i++)
				sampleFeatureMat.at<float>(num, i) = descriptors[i];//��num�����������������еĵ�i��Ԫ��  
			sampleLabelMat.at<int>(num, 0) = 1;//���������Ϊ1������  
		}

		//���ζ�ȡ������ͼƬ������HOG������  
		for (int num = 0; num<NegSamNO; num++)
		{
			Mat srcimg = imread((LPCSTR)CStringA(neg_path[num]));//��ȡͼƬ
			Mat src;
			if (isFlip) flip(srcimg, src, -1); else src = srcimg.clone();
			resize(src, src, Size(hog_width, hog_height));

			vector<float> descriptors;//HOG����������  
			hog.compute(src, descriptors, Size(8, 8));//����HOG�����ӣ���ⴰ���ƶ�����(8,8) 
													  //������õ�HOG�����Ӹ��Ƶ�������������sampleFeatureMat  
			for (int i = 0; i<DescriptorDim; i++)
				sampleFeatureMat.at<float>(num + PosSamNO, i) = descriptors[i];//��PosSamNO+num�����������������еĵ�i��Ԫ��  
			sampleLabelMat.at<int>(num + PosSamNO, 0) = -1;//���������Ϊ-1������  
		}

		CString filePath = xml_store_path + L"\\traindata.xml";
		std::string trainPath = (LPCSTR)CStringA(filePath);

		//ʹ��SVMѧϰ         
		svm->train(sampleFeatureMat, ROW_SAMPLE, sampleLabelMat);
		svm->save(trainPath);
		//���ü����
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
	//���������bug ��ͼƬ�Ƿ���Ҫ��ת
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
		HOGDescriptor hog(Size(hog_width, hog_height), Size(16, 16), Size(8, 8), Size(8, 8), 9);//HOG���������������HOG�����ӵ�  
		int DescriptorDim;//HOG�����ӵ�ά������ͼƬ��С����ⴰ�ڴ�С�����С��ϸ����Ԫ��ֱ��ͼbin��������  

		Mat sampleFeatureMat;//����ѵ������������������ɵľ��������������������ĸ�������������HOG������ά��      
		Mat sampleLabelMat;//ѵ����������������������������������ĸ�������������1��1��ʾ���ˣ�-1��ʾ����  
						   //���ζ�ȡ������ͼƬ������HOG������ 
		for (int num = 0; num < PosSamNO; num++)
		{
			Mat srcimg = imread((char*)(LPCSTR)CStringA(pos_path[num]));//��ȡͼƬ
			Mat srci,src;
			if (isFlip) flip(srcimg, srci, -1); else srci = srcimg.clone();
			srci(rt).copyTo(src);
			resize(src, src, Size(hog_width, hog_height));
			vector<float> descriptors;//HOG����������  
			hog.compute(src, descriptors, Size(8, 8));

#ifdef _TEST
			fout <<"front descriptor "<< descriptors.size() << endl;
#endif // _DEBUG
			
			//�����һ������ʱ��ʼ�����������������������Ϊֻ��֪��������������ά�����ܳ�ʼ��������������  
			if (0 == num)
			{
				DescriptorDim = descriptors.size();//HOG�����ӵ�ά��  
												   //��ʼ������ѵ������������������ɵľ��������������������ĸ�������������HOG������ά��sampleFeatureMat  
				sampleFeatureMat = Mat::zeros(PosSamNO + NegSamNO, DescriptorDim, CV_32FC1);
				//��ʼ��ѵ����������������������������������ĸ�������������1��1��ʾ���ˣ�0��ʾ����  
				sampleLabelMat = Mat::zeros(PosSamNO + NegSamNO, 1, CV_32SC1);
			}

			//������õ�HOG�����Ӹ��Ƶ�������������sampleFeatureMat  
			for (int i = 0; i<DescriptorDim; i++)
				sampleFeatureMat.at<float>(num, i) = descriptors[i];//��num�����������������еĵ�i��Ԫ��  
			sampleLabelMat.at<int>(num, 0) = 1;//���������Ϊ1������  
		}
		//���ζ�ȡ������ͼƬ������HOG������  
		for (int num = 0; num<NegSamNO; num++)
		{
			Mat srcimg = imread((char*)(LPCSTR)CStringA(neg_path[num]));//��ȡͼƬ
			Mat srci, src;
			if (isFlip) flip(srcimg, srci, -1); else srci = srcimg.clone();
			srci(rt).copyTo(src);
			resize(src, src, Size(hog_width, hog_height));
			vector<float> descriptors;//HOG����������  
			hog.compute(src, descriptors, Size(8, 8));//����HOG�����ӣ���ⴰ���ƶ�����(8,8) 
													  //������õ�HOG�����Ӹ��Ƶ�������������sampleFeatureMat  

#ifdef _TEST
			fout << "back descriptor " << descriptors.size() << endl;
#endif // _DEBUG

			for (int i = 0; i<DescriptorDim; i++)
				sampleFeatureMat.at<float>(num + PosSamNO, i) = descriptors[i];//��PosSamNO+num�����������������еĵ�i��Ԫ��  
			sampleLabelMat.at<int>(num + PosSamNO, 0) = -1;//���������Ϊ-1������  
		}

#ifdef _TEST
		fout << "sfm typeis " << sampleFeatureMat.type() << " rows:" <<sampleFeatureMat.rows << "cols:"<< sampleFeatureMat.cols << endl;
		fout << "slm typeis " << sampleLabelMat.type() <<"rows:" << sampleLabelMat.rows <<"cols:" <<sampleLabelMat.cols << endl;
		if(svm.empty()) fout << "svm is empty " << endl;
#endif // _DEBUG

		if (0 != init()) return -3.0;
		//ʹ��SVMѧϰ  
		svm->train(sampleFeatureMat, ROW_SAMPLE, sampleLabelMat);
		CString filePath = xml_store_path + "\\traindata.xml";
		std::string trainPath = (LPCSTR)CStringA(filePath);
		svm->save(trainPath);
		//���ü����
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
	src(rt).copyTo(timg);	//�Ķ�����Rect�����Ƹ���Ȥ���� Rect ImgROI;
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
		//	HOGDescriptor hog(Size(64, 128), Size(16, 16), Size(8, 8), Size(8, 8), 9);//HOG���������������HOG�����ӵ�  
		int DescriptorDim;//HOG�����ӵ�ά������ͼƬ��С����ⴰ�ڴ�С�����С��ϸ����Ԫ��ֱ��ͼbin��������  
		Mat res,img;
		Mat sampleFeatureMat;
		resize(src, img, Size(hog_width,  hog_height));
		HOGDescriptor hog(Size(hog_width, hog_height), Size(16, 16), Size(8, 8), Size(8, 8), 9);//HOG���������������HOG�����ӵ�  

		vector<float> descriptors;//HOG���������� 
//		descriptors.clear();
		hog.compute(img, descriptors, Size(8, 8));//����HOG�����ӣ���ⴰ���ƶ�����(8,8)  
		DescriptorDim = descriptors.size();//HOG�����ӵ�ά��  
										   //��ʼ������ѵ������������������ɵľ��������������������ĸ�������������HOG������ά��sampleFeatureMat  
		sampleFeatureMat = Mat::zeros(1, DescriptorDim, CV_32FC1);
		//��ʼ��ѵ����������������������������������ĸ�������������1��1��ʾ���ˣ�0��ʾ����  

		//������õ�HOG�����Ӹ��Ƶ�������������sampleFeatureMat  
		for (int i = 0; i < DescriptorDim; i++)
			sampleFeatureMat.at<float>(0, i) = descriptors[i];//��num�����������������еĵ�i��Ԫ��  
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

	std::vector<cv::Rect> found, found_filtered;//���ο�����  
	HOGDetect.detectMultiScale(src, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);//��ͼƬ���ж�߶����˼��  

	//�ҳ�����û��Ƕ�׵ľ��ο�r,������found_filtered��,�����Ƕ�׵Ļ�,��ȡ���������Ǹ����ο����found_filtered��  
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
//�����ο���Ϊhog�����ľ��ο��ʵ�������Ҫ��΢��Щ,����������Ҫ��һЩ����  
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
	����SVMѵ����ɺ�õ���XML�ļ����棬��һ�����飬����support vector������һ�����飬����alpha,��һ��������������rho;
	��alpha����ͬsupport vector��ˣ�ע�⣬alpha*supportVector,���õ�һ����������֮���ٸ���������������һ��Ԫ��rho��
	��ˣ���õ���һ�������������ø÷�������ֱ���滻opencv�����˼��Ĭ�ϵ��Ǹ���������cv::HOGDescriptor::setSVMDetector()����
	�Ϳ����������ѵ������ѵ�������ķ������������˼���ˡ�
	***************************************************************************************************/
	CString fileName = path + L"\\traindata.xml";
	cv::Ptr<cv::ml::SVM> lsvm = StatModel::load<SVM>((std::string)(CStringA)fileName); //����svm = Statmodel::load<SVM>("SVM_HOG.xml"); static function

																					   // svm->load<SVM>("SVM_HOG.xml"); ����ʹ�ò���
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
	HOGDetect.winSize = Size(hog_width, hog_height); //ȱʧҲ�ᷭ��
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


