#pragma once


#if !defined(_OPENCV_PROC_002078B03530__INCLUDED_)
#define _OPENCV_PROC_002078B03530__INCLUDED_







using namespace cv; //Mat所需要的空间名称


#define NOEXIST -1


#define ColorInvert(mat) MonoInvert(mat)
#define image_mono_cesus(mat) image_color_cesus(mat, 0)

enum{ INVALID = 0, NOOBJECT, NODOTS, UP, DOWN, LEFT, RIGHT };
enum { OTSU = 0,CONST_VALUE};

class COpencvprocess
{
public:
	enum {
		ISOLATE = 1, ADDED = 2
	};
private:
	enum{ROISQUARE, ROILINE, ROIFACE,ROIMAX};
public:
	COpencvprocess();
	virtual ~COpencvprocess();  



	struct V4i {
		int x1;
		int y1;
		int x2;
		int y2;
	};
	struct V2i {
		int x;
		int y;
	};

	//存储了连通区域的信息
	struct AreaInfo {
		//团从属于第几个块
		int RusSubTo;
		//该团的序列
		int RunsSerial;
		//该团的面积
		union
		{
			float fsquare;
			int isquare;
		} AreaSquare;
		//该团在第几列
		int row;
		int st;
		int ed;
		int sign;
	};


	

	struct COLOR
	{
		uchar R;
		uchar G;
		uchar B;
	}_RED, _GREEN, _BLUE, _YELLOW;



	struct tagImgROI{
		int xpos;
		int ypos;
		int width;
		int height;
	}ImgROI, MFCROI, SHOWIMGPOS;

	struct tagImgProperty
	{
		int width;
		int height;
		int DPI;
		int sRGB;
		int ISO;
		short manufactory[1024];
		short type[16];
		int nChannels;
		int depth;
	}ImgProperty;

	struct MotionInfo
	{
		struct Tpye1
		{
			int move_wise; //-1 is clockwise 0 is no move and 1 is counterclockwise
			int move_angle;
		}tpye1;
		struct Tpye2
		{
			int arrayTo;
		}tpye2;
		int Valid;
	}motion_info, MotionInfoClear;

	int ColorCesus[4][256];
	int* MonoCesus = ColorCesus[0];
public:
	//图像反相 Mat格式
	void MonoInvert(const Mat& mat);

	//图像缓存

	//获取图像感兴趣区域
	void GetROI(tagImgROI &ROI);
	//对灰阶直方图进行存储
	int m_GrayImgStatics[256];


	void SwapValue(char &a, char &b);
private:
	// 通过计算获取映射感兴趣区域
	BOOL GetIplROI(IplImage* Ipl);
	BOOL GetMatROI(const Mat& mat);

public:
	//存储二值化
	int m_thresholdvalue;
	//存储二值化矫正值
	int m_tholdadjust;
private:

	int m_TotalPix;
	
	int otsuThreshold(const Mat& mat);
	int otsubyROI(const Mat& mat);
	void Threshold(const Mat& mat, int threshold);
public:
	
private:
	// 寻找取样点

private:

public:
	// 为了将图片从mat格式转化成bmp格式，需要创建的格式头信息在这里
	LPBITMAPINFO CreateMapInfo(Mat mat);
	// 将bmp格式转成Mat格式
	cv::Mat BmpToMat(BITMAPINFO* bmpinfo, BYTE* imageData);

	// 图像原始数据
	Mat m_RawMatImg,m_ResultMatImg;;

	Mat LoadMatImg(char* filename);
	LPBITMAPINFO m_lpBmi;

	bool get_Mat_depth_and_channels(const Mat& mat, int& depth, int& channels);
	inline void swapValue(uchar& a, uchar& b);
public:

	void getWidthAndHeight(cv::Mat mat, cv::Point &pt);
	// 统计图片直方图
	int image_color_cesus(const Mat& mat, int channel);
	// 保存了直方图数据
	int m_ColorCesus[4][256];
private:
	Mat m_RawMatCannotChanged;
public:
	inline float IvtSqrt(float x);
	inline float Sqrt(float x);
//	int hough_lines(const Mat& mat, vector<Vec4i>& lines, double rho, double theta, int threshold, double  minLineLength = 0, double  maxLineGap = 0);

	void InitMat(Mat& mat, int t);
	Mat m_canny;
	Mat m_dst;
private:
	float m_fabs(float &a);
	float m_default_ang_sin[180];
	float m_default_ang_cos[180];
	float m_default_ang_cos_sin[360];
	void swapValuei(int& a, int& b);
	// 保存了背景图片
	Mat m_BackGroundMatImg;
	// 从背景图片里提取掩码
	
public:
	// 从背景图片里提取掩码图片
	Mat m_BackGroundMaskMatImg;
	bool get_mask_from_background(const Mat& mat);
private:
	void delete_Mat_with_Mask(Mat& mat);
public:
	// 存储了处理结果的图片
	
	void get_BackGroud_Mat_img(Mat& dst);


	//Mat img, gray, bggray;
	
	
	void find_gravity_center(const Mat& mat, Point &p);
	void CopyMatDataToMem(const Mat& mat);
	void CopyMatDataToMem(const Mat& mat, uchar* ImageData);
	uchar* ImageData;
	void MarkMaskImg(const Mat& mat, Mat& mask, int type = 1);
	bool isOverLap(int& a ,int& b, int& c,int& d);
	void getAllBlackBlockinGray(Mat& black_mask, const Mat& gray, const Mat& white_mask);
	int getOtsuByMask(const Mat& src, const Mat& mask);

	void drawArrow(Mat& img, Point pStart, Point pEnd, int len, int alpha,Scalar& color, int thickness, int lineType);
	int iabs(int a);
	bool show_error(Mat& mat, char* str);

	CString SaveFile(cv::Mat src, CString str);

};

//extern COpencvprocess *cvproc;

#endif 
