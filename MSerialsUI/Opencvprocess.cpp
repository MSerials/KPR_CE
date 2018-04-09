//this is a class of opencv
//add #include "Opencvprocess.h" to where you want
//LoadMatImg(char* filename) to load a piture from computer
//monoinvert()  for invert picture if it is upside down
//MatToBmp can make you display a piture by bmp format
//BmpToMat can make you load a bmp to Mat format so that you can use it for a camera
//MatImgProcess can be edited to you want to deal with image
//m_RawMatImg stores the origin piture
#include "stdafx.h"
#include "Opencvprocess.h"
#include <vector>
#include <math.h>
//#include "SetDlg.h"
using namespace std;
using namespace cv;

//COpencvprocess *cvproc = NULL;






#pragma warning(disable:4267)





void COpencvprocess::getWidthAndHeight(cv::Mat mat, cv::Point &pt)
{
	cv::Rect rt;
	pt.x = pt.y = 0;
	rt.x = mat.cols;
	rt.y = mat.rows;
	rt.width = 0;
	rt.height = 0;
	size_t height = mat.rows;
	size_t width = mat.cols;
	size_t widthStep = mat.step;
	uchar* mdata = mat.data;
	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < widthStep; w++)
		{
			if (mdata[w])
			{
				if (w<rt.x) rt.x = w;
				if (h<rt.y) rt.y = h;
				if (h >rt.height) rt.height = h;
				if (w >rt.width) rt.width = w;
			}
		}
		mdata += widthStep;
	}
	pt.y = rt.height - rt.y;
	pt.x = rt.width - rt.x;
}

int COpencvprocess::getOtsuByMask(const Mat& src, const Mat& mask)
{

	try {
		const int GrayScale = 256;
		const int width = src.cols;
		const int height = src.rows;
		int pixelCount[GrayScale] = { 0 };
		float pixelPro[GrayScale] = { 0 };
		int i, j, pixelSum = width * height, threshold = 0;
		pixelSum = 1;//去除bug,所以不等于0
		uchar* idata = src.data;  //指向像素数据的指针
		uchar* mdata = mask.data;

		//统计灰度级中每个像素在整幅图像中的个数  
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				if (mask.data[i * width + j])
				{
					pixelCount[(int)idata[i * width + j]]++;  //将像素值作为计数数组的下标
					pixelSum++;
				}
			}
		}


		//计算每个像素在整幅图像中的比例  
		float maxPro = 0.0;
		int kk = 0;
		for (i = 0; i < GrayScale; i++)
		{
			pixelPro[i] = (float)pixelCount[i] / pixelSum;
			if (pixelPro[i] > maxPro)
			{
				maxPro = pixelPro[i];
				kk = i;
			}
		}


		//遍历灰度级[0,255]  
		float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
		for (i = 0; i < GrayScale; i++)     // i作为阈值
		{
			w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
			for (j = 0; j < GrayScale; j++)
			{
				if (j <= i)   //背景部分  
				{
					w0 += pixelPro[j];
					u0tmp += j * pixelPro[j];
				}
				else   //前景部分  
				{
					w1 += pixelPro[j];
					u1tmp += j * pixelPro[j];
				}
			}
			u0 = u0tmp / w0;
			u1 = u1tmp / w1;
			u = u0tmp + u1tmp;
			deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
			if (deltaTmp > deltaMax)
			{
				deltaMax = deltaTmp;
				threshold = i;
			}
		}

		//delete[] idata;
		return threshold;
	}
	catch (...) {
		AfxMessageBox(L"未知错误");
	}

}

COpencvprocess::COpencvprocess()
: ImageData(NULL)
{
	float n = 0.0, delta_ang = (float)CV_PI / 180;
	for (int i = 0; i < 180; i++,n += delta_ang)
	{
		m_default_ang_cos_sin[i * 2] =		(float)cos(delta_ang);
		m_default_ang_cos_sin[i * 2 + 1] =	(float)sin(delta_ang);
	}

	_RED.R = 255;
	_RED.G = 0;
	_RED.B = 0;
	_GREEN.R = 0;
	_GREEN.G = 255;
	_GREEN.B = 0;
	_BLUE.R = 0;
	_BLUE.G = 0;
	_BLUE.B = 255;
	_YELLOW.R = 255;
	_YELLOW.G = 255;
	_YELLOW.B = 0;

	MotionInfoClear.tpye1.move_wise = 0;
	MotionInfoClear.tpye1.move_angle = 0;
	MotionInfoClear.tpye2.arrayTo = 0;
	MotionInfoClear.Valid = 0;
	motion_info = MotionInfoClear;

	for (int i = 0; i < 256; i++)
		m_ColorCesus[0][i] = i;
}


COpencvprocess::~COpencvprocess()
{

}



void COpencvprocess::GetROI(tagImgROI &ROI)
{
	if (0 == ROI.height || 0 == ROI.height) return;
	ImgROI = ROI;
}


void COpencvprocess::SwapValue(char &a, char &b)
{
	a = a^b;
	b = a^b;
	a = a^b;
}


// 获取映射感兴趣区域
BOOL COpencvprocess::GetIplROI(IplImage* Ipl)
{
	if (0 == MFCROI.width || 0 == MFCROI.height || NULL == Ipl){
		ImgROI.xpos = ImgROI.ypos = 0;
		ImgROI.width = Ipl->width; ImgROI.height = Ipl->height;
		return 0;
	}
	ImgROI.xpos = (int)((1.0 * MFCROI.xpos *  Ipl->width) / SHOWIMGPOS.width);
	ImgROI.ypos = (int)((1.0 * MFCROI.ypos *  Ipl->height) / SHOWIMGPOS.height);
	ImgROI.width = (int)(((1.0 * (MFCROI.xpos + MFCROI.width)  *  (Ipl->width)) / SHOWIMGPOS.width) - ImgROI.xpos);
	ImgROI.height = (int)(((1.0 * (MFCROI.ypos + MFCROI.height) * (Ipl->height)) / SHOWIMGPOS.height) - ImgROI.ypos);

	return 1;
}

BOOL COpencvprocess::GetMatROI(const Mat& mat)
{
	if(mat.empty())
	{
		ImgROI.xpos = ImgROI.ypos = ImgROI.width = ImgROI.height = 0;
		return FALSE;
	}
	else if (0 == MFCROI.width || 0 == MFCROI.height) {
		ImgROI.xpos = ImgROI.ypos = 0;
		ImgROI.width = mat.cols; ImgROI.height = mat.rows;
		return FALSE;
	}

	ImgROI.xpos = (int)((1.0 * MFCROI.xpos *  mat.cols) / SHOWIMGPOS.width);
	ImgROI.ypos = (int)((1.0 * MFCROI.ypos *  mat.rows) / SHOWIMGPOS.height);
	ImgROI.width = (int)(((1.0 * (MFCROI.xpos + MFCROI.width)  *  (mat.cols)) / SHOWIMGPOS.width) - ImgROI.xpos);
	ImgROI.height = (int)(((1.0 * (MFCROI.ypos + MFCROI.height) * (mat.rows)) / SHOWIMGPOS.height) - ImgROI.ypos);

	return TRUE;
}


inline void COpencvprocess::swapValue(uchar& a, uchar& b)
{	
	a = a^b;
	b = a^b;
	a = a^b;
}

//如果是windows下面读取图片，图片会颠倒，这个函数就是为了防止颠倒
void COpencvprocess::MonoInvert(const Mat & mat)
{
	int widthStep = mat.step;
	int rows = mat.rows - 1;
	for (int r = 0; r < rows; r++,rows--)
		for (int step = 0; step<widthStep; step++)
		{
			swapValue(mat.data[r*widthStep + step], mat.data[rows*widthStep + step]);
		}
}


// 统计图片直方图
int COpencvprocess::image_color_cesus(const Mat& mat, int channel)
{
	if (mat.empty())
		return -1;

	int MaxStatic = 0, widthStep = mat.step, rows = mat.rows, cols = mat.cols, tmp;// nChannel, depth;
	//初始化
	for (int n = 0; n < 256; m_ColorCesus[channel][n] = 0, n++);
	//统计数据
	for (int r = 0; r < rows; r++, rows--)
		for (int step = 0; step<cols; step++)
		{
		tmp = m_ColorCesus[channel][mat.data[r*widthStep + step*channel + channel]] ++;
			if (tmp > MaxStatic) MaxStatic = tmp;
		}
	
	return MaxStatic;
}


//获取大津二值化的数值
int COpencvprocess::otsuThreshold(const Mat& mat)
{
	try {
		const int GrayScale = 256;
		const int width = mat.cols;
		const int height = mat.rows;
		int pixelCount[GrayScale] = { 0 };
		float pixelPro[GrayScale] = { 0 };
		int i, j, pixelSum = width * height, threshold = 0;
		uchar* idata = mat.data;  //指向像素数据的指针

												//统计灰度级中每个像素在整幅图像中的个数  
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				pixelCount[(int)idata[i * width + j]]++;  //将像素值作为计数数组的下标
			}
		}

		//计算每个像素在整幅图像中的比例  
		float maxPro = 0.0;
		int kk = 0;
		for (i = 0; i < GrayScale; i++)
		{
			pixelPro[i] = (float)pixelCount[i] / pixelSum;
			if (pixelPro[i] > maxPro)
			{
				maxPro = pixelPro[i];
				kk = i;
			}
		}


		//遍历灰度级[0,255]  
		float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
		for (i = 0; i < GrayScale; i++)     // i作为阈值
		{
			w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
			for (j = 0; j < GrayScale; j++)
			{
				if (j <= i)   //背景部分  
				{
					w0 += pixelPro[j];
					u0tmp += j * pixelPro[j];
				}
				else   //前景部分  
				{
					w1 += pixelPro[j];
					u1tmp += j * pixelPro[j];
				}
			}
			u0 = u0tmp / w0;
			u1 = u1tmp / w1;
			u = u0tmp + u1tmp;
			deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
			if (deltaTmp > deltaMax)
			{
				deltaMax = deltaTmp;
				threshold = i;
			}
		}

		//delete[] idata;
		return threshold;
	}
	catch (...) {
		AfxMessageBox(L"未知错误");
	}
}


int COpencvprocess::otsubyROI(const Mat& mat)
{
	try{
		const int	GrayScale	= 256;
		const int	width		= ImgROI.width;
		const int	height		= ImgROI.height;
		int			pixelCount[GrayScale]	= { 0 };
		float		pixelPro[GrayScale]		= { 0 };
		int			i, j, pixelSum = width * height, threshold = 0;
		uchar* idata = mat.data;  //指向像素数据的指针

		//统计灰度级中每个像素在整幅图像中的个数  
		for (i = ImgROI.ypos; i < (height + ImgROI.ypos); i++)
		for (j = ImgROI.xpos; j < (width + ImgROI.xpos); j++)
			//pixelCount[(int)idata[(mat.rows - i - 1)* width + j]]++;  //将像素值作为计数数组的下标
			pixelCount[(int)idata[i* width + j]]++;
		//计算每个像素在整幅图像中的比例  
		float maxPro = 0.0;
		int kk = 0;
		for (i = 0; i < GrayScale; i++)
		{
			pixelPro[i] = (float)pixelCount[i] / pixelSum;
			if (pixelPro[i] > maxPro)
			{
				maxPro = pixelPro[i];
				kk = i;
			}
		}


		//遍历灰度级[0,255]  
		float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
		for (i = 0; i < GrayScale; i++)     // i作为阈值
		{
			w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
			for (j = 0; j < GrayScale; j++)
			{
				if (j <= i)   //背景部分  
				{
					w0 += pixelPro[j];
					u0tmp += j * pixelPro[j];
				}
				else   //前景部分  
				{
					w1 += pixelPro[j];
					u1tmp += j * pixelPro[j];
				}
			}
			u0 = u0tmp / w0;
			u1 = u1tmp / w1;
			u = u0tmp + u1tmp;
			deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
			if (deltaTmp > deltaMax)
			{
				deltaMax = deltaTmp;
				threshold = i;
			}
		}

		//delete[] idata;
		return threshold;
	}
	catch (...){
		AfxMessageBox(L"未知错误");
	}
	
}


void COpencvprocess::Threshold(const Mat& mat, int threshold)
{
	if (0 == ImgROI.height || 0 == ImgROI.width)
	{
		ImgROI.ypos = ImgROI.xpos = 0; ImgROI.height = mat.rows; ImgROI.width = mat.cols;
	}

	if (threshold > 255) threshold = 255;
	else if (threshold < 0) threshold = 1;

	int ypos = ImgROI.ypos;
	int height = ImgROI.ypos + ImgROI.height;
	int xpos = ImgROI.xpos;
	int width = xpos + ImgROI.width;

	try{
	
	for (int h = ypos; h < height; ++h)
	for (int w = xpos; w < width; ++w)

		if (threshold < mat.data[h*mat.step + w])
			mat.data[h*mat.step + w] = 255;
		else
			mat.data[h*mat.step + w] = 0;
	}
	catch (...){
		AfxMessageBox(L"error location is Threshold(IplImage* src, unsigned char threshold)");

	}
}


// 为了将图片从mat格式转化成bmp格式，需要创建的格式头信息在这里
LPBITMAPINFO COpencvprocess::CreateMapInfo(Mat mat)
{
	BITMAPINFOHEADER BIH = { 40, 1, 1, 1, 8, 0, 0, 0, 0, 0, 0 };
	LPBITMAPINFO lpBmi;
	int          wid, hei, bits, colors, i, depth,  channels;
	RGBQUAD  ColorTab[256];
	wid = mat.cols;     hei = mat.rows ;
	get_Mat_depth_and_channels(mat, depth, channels);
	bits = depth * channels;
	if (bits>8) colors = 0;
	else colors = 1 << bits;
	lpBmi = (LPBITMAPINFO)malloc(40 + 4 * colors);
	BIH.biWidth = wid;     BIH.biHeight = hei;
	BIH.biBitCount = (BYTE)bits;
	memcpy(lpBmi, &BIH, 40);                   //  复制位图信息头
	if (bits == 8) {                           //  256 色位图
		for (i = 0; i<256; i++)  {                //  设置灰阶调色板
			ColorTab[i].rgbRed = ColorTab[i].rgbGreen = ColorTab[i].rgbBlue = (BYTE)i;
		}
		memcpy(lpBmi->bmiColors, ColorTab, 1024);
	}
	return(lpBmi);
}


// 将bmp格式转成Mat格式 
cv::Mat COpencvprocess::BmpToMat(BITMAPINFO* bmpinfo, BYTE* imageData)
{
	int size = bmpinfo->bmiHeader.biWidth*bmpinfo->bmiHeader.biHeight;
	cv::Mat m(Size(bmpinfo->bmiHeader.biWidth, bmpinfo->bmiHeader.biHeight), CV_8UC1);
	memcpy(m.data, (uchar*)imageData, size);
	return m;
	//if (!m_RawMatImg.empty()) m_RawMatImg.release();
	//m_RawMatImg.create(Size(bmpinfo->bmiHeader.biWidth, bmpinfo->bmiHeader.biHeight), CV_8UC1);
	//memcpy(m_RawMatImg.data, (uchar*)imageData, size);
	return Mat();
}

//载入图片到Mat格式里
Mat COpencvprocess::LoadMatImg(char* filename)
{
	m_RawMatImg = imread(filename, -1);

	if (m_RawMatImg.empty()) 
	{
		AfxMessageBox(L"找不到你所选择的图片哦！");
		return m_RawMatImg;
	}
	ImgROI.height = 0;
	ImgROI.width = 0;
	//get_Mat_depth_and_channels(m_RawMatImg, ImgProperty.depth,ImgProperty.nChannels);
	return m_RawMatImg;
}

//为了获取图像深度和通道数
bool COpencvprocess::get_Mat_depth_and_channels(const Mat& mat, int& depth, int& channels)
{
	switch (mat.type())
	{
	case CV_8UC1:	depth = 8; channels = 1; return true;
	case CV_8UC2:	depth = 8; channels = 2; return true;
	case CV_8UC3:	depth = 8; channels = 3; return true;
	case CV_8UC4:	depth = 8; channels = 4; return true;
	case CV_8SC1:	depth = 8; channels = 1; return true;
	case CV_8SC2:	depth = 8; channels = 2; return true;
	case CV_8SC3:	depth = 8; channels = 3; return true;
	case CV_8SC4:	depth = 8; channels = 4; return true;
	case CV_16UC1:	depth = 16; channels = 1; return true;
	case CV_16UC2:	depth = 16; channels = 2; return true;
	case CV_16UC3:	depth = 16; channels = 3; return true;
	case CV_16UC4:	depth = 16; channels = 4; return true;
	case CV_16SC1:	depth = 16; channels = 1; return true;
	case CV_16SC2:	depth = 16; channels = 2; return true;
	case CV_16SC3:	depth = 16; channels = 3; return true;
	case CV_16SC4:	depth = 16; channels = 4; return true;
	case CV_32SC1:	depth = 32; channels = 1; return true;
	case CV_32SC2:	depth = 32; channels = 2; return true;
	case CV_32SC3:	depth = 32; channels = 3; return true;
	case CV_32SC4:	depth = 32; channels = 4; return true;
	case CV_32FC1:	depth = -32; channels = 1; return true;
	case CV_32FC2:	depth = -32; channels = 2; return true;
	case CV_32FC3:	depth = -32; channels = 3; return true;
	case CV_32FC4:	depth = -32; channels = 4; return true;
	case CV_64FC1:	depth = -64; channels = 1; return true;
	case CV_64FC2:	depth = -64; channels = 2; return true;
	case CV_64FC3:	depth = -64; channels = 3; return true;
	case CV_64FC4:	depth = -64; channels = 4; return true;
	default:;
		return false;
	}
}


float COpencvprocess::IvtSqrt(float x)
{
	float xhalf = 0.5f*x;
	int i = *(int*)&x; // get bits for floating VALUE 
	i = 0x5f375a86 - (i >> 1); // gives initial guess y0
	x = *(float*)&i; // convert bits BACK to float
	x = x*(1.5f - xhalf*x*x); // Newton step, repeating increases accuracy
	x = x*(1.5f - xhalf*x*x); // Newton step, repeating increases accuracy
	x = x*(1.5f - xhalf*x*x); // Newton step, repeating increases accuracy
	return x;
}

float COpencvprocess::Sqrt(float x)
{
	return 1 / IvtSqrt(x);
}

void COpencvprocess::InitMat(Mat& mat, int t)
{
	for (int i = 0; i<mat.rows; i++)
		for (int j = 0; j<mat.cols; j++)
			mat.at<int>(i, j) = t;
}





float COpencvprocess::m_fabs(float &a)
{
	int i = *(int*)&a & 0x7FFFFFFF;
	return *(float*)&i;
}


void COpencvprocess::swapValuei(int& a, int& b)
{
	a = a^b;
	b = a^b;
	a = a^b;
}


// 从背景图片里提取掩码图片
bool COpencvprocess::get_mask_from_background(const Mat& mat)
{
	if (mat.empty() /*|| 0!=mat.type()*/)
		return false;

	int width = mat.cols;
	int height = mat.rows, depth, channels;
	const uchar threshold = 240;
	
	get_Mat_depth_and_channels(mat, depth, channels);

	if (1 != channels)
	{
		cvtColor(mat, m_BackGroundMatImg, CV_BGR2GRAY);
		cvtColor(mat, m_BackGroundMaskMatImg, CV_BGR2GRAY);
	}
	else
	{
		m_BackGroundMatImg = mat.clone();
		m_BackGroundMaskMatImg = mat.clone();
	}
	const int widthStep = m_BackGroundMatImg.step;
	//m_BackGroundMatImg = mat.clone();
	//m_BackGroundMaskMatImg = mat.clone();//尽管冗余了，但是可以取得图像尺寸
	for (int h = 0; h < height;h++)
	for (int w = 0; w < width; w++)
	{
		if (threshold > m_BackGroundMatImg.data[h*widthStep + w])
			m_BackGroundMaskMatImg.data[h*widthStep + w] &= 0x0;
		else
			m_BackGroundMaskMatImg.data[h*widthStep + w] |= 0xFF;
	}
	return true;
}


void COpencvprocess::delete_Mat_with_Mask(Mat& dst)
{
	int width = dst.cols;
	int height = dst.rows;
	int widthStep = dst.step;

	if (width != m_BackGroundMaskMatImg.cols || height != m_BackGroundMaskMatImg.rows || dst.empty() || m_BackGroundMaskMatImg.empty())
		return;

	for (int h = 0; h < height; h++)
	for (int w = 0; w < width; w++)
	{
		if (!m_BackGroundMaskMatImg.data[h*widthStep + w])
			dst.data[h*widthStep + w] |= 0xFF;
	}
}


void COpencvprocess::get_BackGroud_Mat_img(Mat& mat)
{
	m_BackGroundMatImg = mat.clone();
	if (m_BackGroundMatImg.empty())
	{
		AfxMessageBox(L"获取背景图片失败");
		return;
	}
}







void COpencvprocess::find_gravity_center(const Mat& mat, Point &p)
{
	if (mat.empty()) return;
	int width = mat.cols;
	const int height = mat.rows;
	const int widthStep = mat.step;
	uchar* idata = mat.data;
	long x = 0, y = 0, sum = 0;

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < widthStep; w++)
		{
			if (255 == idata[w])
			{
				x += w;
				y += h;
				sum += 1;
			}
		}
		idata += widthStep;
	}
	if (0 == sum){ p.x = 0; p.y = 0; return; }
	p.x =(int) (x / sum);
	p.y =(int) (y / sum);
}





void COpencvprocess::CopyMatDataToMem(const Mat& mat)
{
	if (ImageData)
		free(ImageData);

	int height = mat.rows;
	int offset = 0 == mat.step % 4 ? 0 : 4 - mat.step%4;
	int size_i = (offset+mat.step) * mat.rows;
	ImageData = (uchar*)malloc(size_i);
	uchar *tmp = ImageData;
	uchar *idata = mat.data;

	for (int h = 0; h < height; h++)
		{
			memcpy(tmp, idata, mat.step);
			tmp = tmp + mat.step + offset;
			idata = idata + mat.step;
		}
}

void COpencvprocess::CopyMatDataToMem(const Mat& mat, uchar* ImageData)
{
	int height = mat.rows;
	int offset = 0 == mat.step % 4 ? 0 : 4 - mat.step % 4;
	int size_i = (offset + mat.step) * mat.rows;
	ImageData = (uchar*)malloc(size_i);
	uchar *tmp = ImageData;
	uchar *idata = mat.data;

	for (int h = 0; h < height; h++)
	{
		memcpy(tmp, idata, mat.step);
		tmp = tmp + mat.step + offset;
		idata = idata + mat.step;
	}
}


void COpencvprocess::MarkMaskImg(const Mat& mat, Mat& mask, int type)
{
	int width = mat.cols;
	int height = mat.rows;
	mask = mat.clone();
	if(type == 1)
	{
		for (int h = 0; h < height; h++)
			for (int w = 0; w < width; w++)
		{

			if (!mat.data[h*mat.step + w])
			{
				mask.data[h*mat.step + w] |= 0xFF;
			}
		}
	}
	else if (0 == type)
	{
		for (int h = 0; h < height; h++)
			for (int w = 0; w < width; w++)
			{
				if (mat.data[h*mat.step + w])
				{
					mask.data[h*mat.step + w] |= 0xFF;
				}
			}
	}
	else
		return;
}




//判断是否有重叠
bool COpencvprocess::isOverLap(int& a, int& b, int& c, int& d)
{
	return ((d-a)*(c-b) <= 0);
}



void COpencvprocess::getAllBlackBlockinGray(Mat& black_mask, const Mat& gray, const Mat& white_mask)
{

	if (CV_8UC1 != black_mask.type() || CV_8UC1 != gray.type() || CV_8UC1 != white_mask.type())
		return;
	const int widthStep = black_mask.step;
	const int height = black_mask.rows;
	uchar* bdata = black_mask.data;
	uchar* gdata = gray.data;
	uchar* wdata = white_mask.data;

	for (int h = 0; h < height; h++)
	{	
		for (int w = 0; w < widthStep; w++)
			{
				if (wdata[w])
				//if (gdata[w] && wdata[w])
				{
					bdata[w] = gdata[w];
				}
				else
				{
					bdata[w] |= 0xFF;
				}
			}
		bdata += widthStep;
		gdata += widthStep;
		wdata += widthStep;
	}
};



void COpencvprocess::drawArrow(Mat& img, Point pStart, Point pEnd, int len, int alpha, Scalar& color, int thickness, int lineType)
{
	const double PI = 3.1415926;
	     Point arrow;
	     //计算 θ 角（最简单的一种情况在下面图示中已经展示，关键在于 atan2 函数，详情见下面）   
	     double angle = atan2((double)(pStart.y - pEnd.y), (double)(pStart.x - pEnd.x));
	     line(img, pStart, pEnd, color, thickness, lineType);
	     //计算箭角边的另一端的端点位置（上面的还是下面的要看箭头的指向，也就是pStart和pEnd的位置） 
	     arrow.x = (int)( pEnd.x + len * cos(angle + PI * alpha / 180));
	     arrow.y = (int)( pEnd.y + len * sin(angle + PI * alpha / 180));
	     line(img, pEnd, arrow, color, thickness, lineType);
		 arrow.x = (int)(pEnd.x + len * cos(angle - PI * alpha / 180));
		 arrow.y = (int)(pEnd.y + len * sin(angle - PI * alpha / 180));
	     line(img, pEnd, arrow, color, thickness, lineType);
}

int COpencvprocess::iabs(int a)
{
	if (a >= 0)
		return a;
	else
		return(0 - a);
}


bool COpencvprocess::show_error(Mat& mat, char* str)
{
	if(CV_8UC1 == mat.type())
		cvtColor(mat, mat, CV_GRAY2BGR);
	ColorInvert(mat);
	//Mat color(640, 480, CV_8UC3, Scalar(10, 10, 10));
	Point pstart;
	pstart.x = 10;
	pstart.y = 50;
	putText(mat, str, pstart, 1, 2, Scalar(64, 64, 255), 2, 8, false);
	ColorInvert(mat);
	return true;
}







CString COpencvprocess::SaveFile(cv::Mat src,CString str/* = L""*/)
{
	//写在一起了 供以后参考
	if (src.empty()) return L"";

	//获取时间
	if (L"" == str)
	{
		SYSTEMTIME st;
		CString strDate, strTime;
		GetLocalTime(&st);
		strDate.Format(_T("%4d-%d-%d_"), st.wYear, st.wMonth, st.wDay);
		strTime.Format(_T("%d-%d-%d"), st.wHour, st.wMinute, st.wSecond);
		str = strDate + strTime;
	}
	//获取路径
	CString AppPath;
	::GetModuleFileName(GetModuleHandle(NULL), AppPath.GetBuffer(300), 300);
	AppPath.ReleaseBuffer();
	AppPath = AppPath.Left(AppPath.ReverseFind('\\'));
	AppPath = AppPath + L"\\" + L"Image";
	CreateDirectory(AppPath, NULL);
	str = AppPath +L"\\"+ str + L".bmp";
	
	//CString转成char*
	int n = str.GetLength(); //获取str的字符数  
	int len = WideCharToMultiByte(CP_ACP, 0, str, n, NULL, 0, NULL, NULL); //获取宽字节字符的大小，大小是按字节计算的
	char *pChar = new char[len + 1]; //以字节为单位  
	if (n > 4092) pChar[0] = '\0';
	WideCharToMultiByte(CP_ACP, 0, str, n, pChar, len, NULL, NULL); //宽字节编码转换成多字节编码  
	pChar[len] = '\0'; //多字节字符以'\0'结束 

	imwrite(pChar, src);
	delete pChar;

	return str;
}

