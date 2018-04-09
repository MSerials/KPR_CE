#pragma once
#ifndef ___IMAGE__CARD___
#define ___IMAGE__CARD___
#include <mutex>
#include "halconcpp.h"
#include "imgproc.hpp"
#include <afxwin.h>
#include <atlstr.h>

#ifdef _WIN64
#pragma comment(lib,"../third_party/lib/halcon/x64/halconcpp.lib")
#else
#pragma comment(lib,"../third_party/lib/halcon/x86/halconcpp.lib")
#endif

using namespace Halcon;
//引入dll

class CImageCard
{
private:
	CImageCard() { RectInfo = NO_RECT; }
	virtual ~CImageCard() {};

public:

	enum {
		NO_RECT, TRECT, BRECT
	};

	int RectInfo;
	CString ShowRectInfo(int rt)
	{
		if (TRECT == rt) return  L"请画完相机一检测框";
		if (BRECT == rt) return  L"请画完相机二检测框";
		return L"";
	}

	static CImageCard * GetIns() { static CImageCard l_ic; return &l_ic; }

	int hc_check_inv(const Halcon::Hobject&src, const Halcon::Hobject& ROI, Halcon::HTuple& disp_hand)
	{
		return 0;
	}

#ifdef NEIXINGLUN
	Herror hc_check_gear(Hobject & SearchImage, const Halcon::HTuple & disp_hand, double r,double c,double radius,int selection = 0, double score = 0.5,const char* path = "", int model_selection = 0)
	{

		static std::mutex l_mtx;
		std::lock_guard<std::mutex> lck(l_mtx);
		Hobject  ModelImage, ROI, ImageROI, ShapeModelImage;
		Hobject  ShapeModelRegion, FilledModelRegion;
		Hobject  ModelAtNewPosition, DilateImage;

		HTuple  center_r, center_c, ModelRegionRow1, ModelRegionColumn1;
		HTuple  ModelRegionRow2, ModelRegionColumn2, Width, WindowHandleZoom;
		HTuple  Area, i, RowCheck, ColumnCheck, AngleCheck;
		HTuple j, MovementOfObject, FGHandle;
		static HTuple ModelID,SavedModelID;
		Hobject ShapeModel;
		Herror error = H_MSG_TRUE;
		set_check("~give_error");
		try
		{
			if (0 == selection)//保存模板
			{
				Hlong num_max = SavedModelID.Num();
				if (num_max > 4)
				{
					set_tposition(disp_hand, 0, 1);
					Halcon::write_string(disp_hand, "模板已满");
					return (!H_MSG_TRUE);
				}
				set_draw(disp_hand, "margin");
				Halcon::set_color(disp_hand, "yellow");
				gen_circle(&ROI, r, c, radius);
				disp_obj(SearchImage, disp_hand);
				
				//step 2: create a first model region
				reduce_domain(SearchImage, ROI, &ImageROI);
				disp_obj(ROI, disp_hand);
				inspect_shape_model(ImageROI, &ShapeModelImage, &DilateImage, 1, 30);

				dilation_circle(DilateImage, &ShapeModelRegion, 1);

				smallest_rectangle1(ShapeModelRegion, &ModelRegionRow1, &ModelRegionColumn1, &ModelRegionRow2,
					&ModelRegionColumn2);
	
				disp_obj(ShapeModelRegion, disp_hand);
	
				fill_up(ShapeModelRegion, &FilledModelRegion);
				opening_circle(FilledModelRegion, &ROI, 2.5);
		
				reduce_domain(SearchImage, ROI, &ImageROI);
				create_shape_model(ImageROI, 3, 0, HTuple(360).Rad(), "auto", "none", "use_polarity",
					30, 15, &ModelID);
				inspect_shape_model(ImageROI, &ShapeModelImage, &ShapeModelRegion, 1, 30);
				get_shape_model_contours(&ShapeModel, ModelID, 1);
				
				set_color(disp_hand, "blue");
				disp_obj(ShapeModelRegion, disp_hand);

				CString str(path);
				CString Model;
				SavedModelID.Append(ModelID);
				num_max = SavedModelID.Num();
			
				char to_save_path[_MAX_PATH] = { 0 };
				sprintf_s(to_save_path,"%s/Model%d.shm",path, static_cast<int>(num_max));
				error = write_shape_model(ModelID, to_save_path);

				set_tposition(disp_hand,0,1);
				if (H_MSG_TRUE == error)
				{
					Halcon::write_string(disp_hand, "设置成功");
				}
				else
				{
					Halcon::write_string(disp_hand, "设置失败,可能原因，图像不清楚。");
				}
				return error;
 
				
			}
			else if(1==selection)//检测
			{
				bool isFound = false;
				HTuple Score;
				Hlong num_max = SavedModelID.Num();
				size_t index,_num_max = num_max < 5 ? num_max : 5;
				for(index = 0; index<_num_max;index++)
				{
					find_shape_model(SearchImage, SavedModelID[index], 0, HTuple(360).Rad(), 0.75, 3, 0, "least_squares",
						0, 0.7, &RowCheck, &ColumnCheck, &AngleCheck, &Score);
					if (0 != ((Score.Num()) > 0))	break;
				}
				
				if (0 != ((Score.Num()) > 0))
				{
					get_shape_model_contours(&ShapeModel, SavedModelID[index], 1);
					for (j = 0; j <= (Score.Num()) - 1; j += 1)
					{
						if (Score > score)
						{
							vector_angle_to_rigid(0, 0, 0, HTuple(RowCheck[j]), HTuple(ColumnCheck[j]),
								HTuple(AngleCheck[j]), &MovementOfObject);
							affine_trans_contour_xld(ShapeModel, &ModelAtNewPosition, MovementOfObject);
							isFound = true;
							set_draw(disp_hand, "margin");
							Halcon::set_color(disp_hand, "green");
							disp_obj(ModelAtNewPosition, disp_hand);
							Halcon::write_string(disp_hand, " 精度：");
							Halcon::write_string(disp_hand, Score);
						}
						else
						{
							set_draw(disp_hand, "margin");
							Halcon::set_color(disp_hand, "red");
							disp_obj(ModelAtNewPosition, disp_hand);
							Halcon::write_string(disp_hand, " 精度：");
							Halcon::write_string(disp_hand, Score);
						}
					}

				}
				else
				{
					set_draw(disp_hand, "margin");
					Halcon::set_color(disp_hand, "red");
					Halcon::write_string(disp_hand, " 未检测到");
				}
				if (isFound)
					return H_MSG_TRUE;
				else
					return !H_MSG_TRUE;

			}
			else if (2 == selection)//读取模板
			{
				Herror err = H_MSG_TRUE;
				bool flag = false;
				//clear_all_shape_models();
				//clear_shape_model(SavedModelID);
				HTuple _Model;
				//Hlong num_max = SavedModelID.Num();
				//printf("%d", num_max);
				for (int i = 1; i < 6; i++)
				{
					char to_del_path[_MAX_PATH] = { 0 };
					HTuple Model;
					sprintf_s(to_del_path, "%s/Model%d.shm", path, i);
					err = read_shape_model(to_del_path,&Model);
					if (H_MSG_TRUE != err) break;
					flag = true;
					//SavedModelID.Append(Model);
					_Model.Append(Model);
				}
				SavedModelID = _Model;
				if (flag) return H_MSG_TRUE;
				return !H_MSG_TRUE;
			}
			else if (3 == selection)//清空模板
			{
				try
				{
					set_check("~give_error");
					Hlong num_max = static_cast<Hlong>(SavedModelID.Num());
					if (0 == num_max) return 0;
					char to_del_path[_MAX_PATH] = { 0 };
					sprintf_s(to_del_path, "%s/Model%d.shm", path, static_cast<int>(num_max));
					DeleteFile(CString(to_del_path));

					SavedModelID = SavedModelID.Remove(num_max-1);
					return static_cast<Herror>(SavedModelID.Num());
				}
				catch (HException & message)
				{
					set_tposition(disp_hand, 0, 1);
					Halcon::write_string(disp_hand, "没有模板");
					Halcon::write_string(disp_hand, message.message);
					return H_MSG_FAIL;
				}
				
			}
			else if (4 == selection)//返回模板数量
			{
				set_check("~give_error");
				return static_cast<Herror>(SavedModelID.Num());
			}
			else if (5 == selection)//显示模板
			{
				set_check("~give_error"); 
				if (model_selection >= SavedModelID.Num())
				{
					set_draw(disp_hand, "margin");
					Halcon::set_color(disp_hand, "red");
					set_tposition(disp_hand, 0, 1);
					Halcon::write_string(disp_hand, "没有模板");
					return (!H_MSG_TRUE);
				}
				try
				{ 
				Herror err = get_shape_model_contours(&ShapeModel, SavedModelID[model_selection], 1);
				clear_window(disp_hand);
				if (H_MSG_TRUE != err)
				{
					set_tposition(disp_hand, 0, 1);
					Halcon::write_string(disp_hand, "没有模板");
				}
				else
				{
					set_draw(disp_hand, "margin");
					Halcon::set_color(disp_hand, "blue");
					disp_obj(ShapeModel, disp_hand);
				}

				
				}
				catch (HException & message)
				{
					set_tposition(disp_hand, 0, 1);
					Halcon::write_string(disp_hand, "没有模板");
					Halcon::write_string(disp_hand, message.message);
				}
			}
		}
		catch (HException & message)
		{
			set_tposition(disp_hand, 0, 1);
			Halcon::write_string(disp_hand, "执行失败");
			Halcon::write_string(disp_hand, message.message);
			return H_MSG_FAIL;
		}
		return H_MSG_TRUE;
	}
#endif

#ifdef CHANGER
	Herror hc_check_gear(Hobject & SearchImage, const Halcon::HTuple & disp_hand, double r1, double c1, double r2, double c2, int selection = 0, double score = 0.5, const char* path = "", int model_selection = 0)
	{
		static std::mutex l_mtx;
		std::lock_guard<std::mutex> lck(l_mtx);
		using namespace Halcon;

		// Local iconic variables 
		Hobject  ModelImage, ROI, ImageROI, ShapeModelImages;
		Hobject  ShapeModelRegions, ShapeModel,ModelAtNewPosition;// SearchImage, 

		// Local control variables 
		HTuple  FGHandle, Pointer, Type, Width, Height;
		HTuple  WindowHandle, Row1, Column1, Row2, Column2, AreaModelRegions;
		HTuple  RowModelRegions, ColumnModelRegions, HeightPyramid;
		HTuple  i, NumLevels, ModelID, ImageFiles, Index, RowCheck;
		HTuple  ColumnCheck, AngleCheck, Score, MovementOfObject;
		static HTuple SavedModelID;
		Herror error = H_MSG_TRUE;

		set_check("~give_error");
		try
		{
			if (0 == selection)//保存模板
			{
				Hlong num_max = SavedModelID.Num();
				get_image_pointer1(SearchImage, &Pointer, &Type, &Width, &Height);
				threshold(SearchImage, &ModelImage, 0, 190);
				region_to_bin(ModelImage, &ModelImage, 0, 255, Width, Height);
				gen_rectangle1(&ROI, r1, c1, r2, c2);
				set_color(disp_hand, "yellow");
				disp_obj(SearchImage, disp_hand);
				reduce_domain(ModelImage, ROI, &ImageROI);
				inspect_shape_model(ImageROI, &ShapeModelImages, &ShapeModelRegions, 8, 30);

				area_center(ShapeModelRegions, &AreaModelRegions, &RowModelRegions, &ColumnModelRegions);
				count_obj(ShapeModelRegions, &HeightPyramid);
				for (i = 1; i <= HeightPyramid; i += 1)
				{
					if (0 != (HTuple(AreaModelRegions[i - 1]) >= 15))
					{
						NumLevels = i;
					}
				}
				create_shape_model(ImageROI, NumLevels, 0, HTuple(360).Rad(), "auto", "none", "use_polarity",
					30, 10, &ModelID);
				get_shape_model_contours(&ShapeModel, ModelID, 1);

				set_color(disp_hand, "blue");
				disp_obj(ShapeModel, disp_hand);

				CString str(path);
				CString Model;
				SavedModelID = ModelID;
				//SavedModelID.Append(ModelID);
				//num_max = SavedModelID.Num();

				char to_save_path[_MAX_PATH] = { 0 };
				sprintf_s(to_save_path, "%s/Model.shm", path);
				//sprintf_s(to_save_path, "%s/Model%d.shm", path, static_cast<int>(num_max));
				error = write_shape_model(ModelID, to_save_path);

				set_tposition(disp_hand, 0, 1);
				if (H_MSG_TRUE == error)
				{
					Halcon::write_string(disp_hand, "设置成功");
				}
				else
				{
					Halcon::write_string(disp_hand, "设置失败,可能原因，图像不清楚。");
				}
				return error;


			}
			else if (1 == selection)//检测
			{
				bool isFound = false;
				HTuple Score;
				Hlong num_max = SavedModelID.Num();
				size_t index = 0, _num_max = num_max < 5 ? num_max : 5;
#ifdef _TEST
				printf("模板数量 %d\n", _num_max);
#endif


#ifdef NEIXINGLUN
				for (index = 0; index<_num_max; index++)
				{
					find_shape_model(SearchImage, SavedModelID[index], 0, HTuple(360).Rad(), 0.75, 3, 0, "least_squares",
						0, 0.7, &RowCheck, &ColumnCheck, &AngleCheck, &Score);
					if (0 != ((Score.Num()) > 0))	break;
				}

				if (0 != ((Score.Num()) > 0))
				{
					get_shape_model_contours(&ShapeModel, SavedModelID[index], 1);
					for (int j = 0; j <= (Score.Num()) - 1; j += 1)
					{
						if (Score > score)
						{
							vector_angle_to_rigid(0, 0, 0, HTuple(RowCheck[j]), HTuple(ColumnCheck[j]),
								HTuple(AngleCheck[j]), &MovementOfObject);
							affine_trans_contour_xld(ShapeModel, &ModelAtNewPosition, MovementOfObject);
							isFound = true;
							set_draw(disp_hand, "margin");
							Halcon::set_color(disp_hand, "green");
							disp_obj(ModelAtNewPosition, disp_hand);
							Halcon::write_string(disp_hand, " 相似度:");
							Halcon::write_string(disp_hand, Score);
						}
						else
						{
							set_draw(disp_hand, "margin");
							Halcon::set_color(disp_hand, "red");
							disp_obj(ModelAtNewPosition, disp_hand);
							Halcon::write_string(disp_hand, " 相似度:");
							Halcon::write_string(disp_hand, Score);
						}
					}


				}
#endif
#ifdef CHANGER
				threshold(SearchImage, &ModelImage, 0, 190);
				region_to_bin(ModelImage, &ModelImage, 0, 255, Width, Height);
				for (index = 0; index<_num_max; index++)
				{
					find_shape_model(SearchImage, SavedModelID[index], 0, HTuple(360).Rad(), 0.75, 3, 0, "least_squares",
						0, 0.7, &RowCheck, &ColumnCheck, &AngleCheck, &Score);
					if (0 != ((Score.Num()) > 0))	break;
				}
				int d = 0;
				if (0 != ((Score.Num()) > 0))
				{
					get_shape_model_contours(&ShapeModel, SavedModelID[index], 1);
					for (int j = 0; j <= (Score.Num()) - 1; j += 1)
					{
						
						if (Score > score)
						{
							vector_angle_to_rigid(0, 0, 0, HTuple(RowCheck[j]), HTuple(ColumnCheck[j]),
								HTuple(AngleCheck[j]), &MovementOfObject);
							affine_trans_contour_xld(ShapeModel, &ModelAtNewPosition, MovementOfObject);
							isFound = true;
							set_draw(disp_hand, "margin");
							Halcon::set_color(disp_hand, "green");
							disp_obj(ModelAtNewPosition, disp_hand);
							Halcon::write_string(disp_hand, " 相似度:");
							Halcon::write_string(disp_hand, Score[j]);
						}
						else
						{
							set_draw(disp_hand, "margin");
							Halcon::set_color(disp_hand, "red");
							disp_obj(ModelAtNewPosition, disp_hand);
							Halcon::write_string(disp_hand, " 相似度:");
							Halcon::write_string(disp_hand, Score[j]);
						}
						
						if (isFound == true) break;
						
					}
				}
#endif
				else
				{
					set_draw(disp_hand, "margin");
					Halcon::set_color(disp_hand, "red");
					Halcon::write_string(disp_hand, " 未检测到");
				}
				if (isFound)
					return H_MSG_TRUE;
				else
					return !H_MSG_TRUE;

			}
			else if (2 == selection)//读取模板
			{
				Herror err = H_MSG_TRUE;
				bool flag = false;
				//clear_all_shape_models();
				//clear_shape_model(SavedModelID);
				HTuple _Model;
				//Hlong num_max = SavedModelID.Num();
				//printf("%d", num_max);
#ifdef CHANGER
				for (int i = 1; i < 2; i++)
#else
				for (int i = 1; i < 6; i++)
#endif
				{
					char to_del_path[_MAX_PATH] = { 0 };
					HTuple Model;
#ifdef CHANGER
					sprintf_s(to_del_path, "%s/Model.shm", path, i);
#else
					sprintf_s(to_del_path, "%s/Model%d.shm", path, i);
#endif
					err = read_shape_model(to_del_path, &Model);
					if (H_MSG_TRUE != err) break;
					flag = true;
					//SavedModelID.Append(Model);
					_Model.Append(Model);
				}
				SavedModelID = _Model;
				if (flag) return H_MSG_TRUE;
				return !H_MSG_TRUE;
			}
			else if (3 == selection)//清空模板
			{
#ifdef CHANGER
				return H_MSG_FAIL;
#endif
				try
				{
					set_check("~give_error");
					Hlong num_max = static_cast<Hlong>(SavedModelID.Num());
					if (0 == num_max) return 0;
					char to_del_path[_MAX_PATH] = { 0 };
					sprintf_s(to_del_path, "%s/Model%d.shm", path, static_cast<int>(num_max));
					DeleteFile(CString(to_del_path));

					SavedModelID = SavedModelID.Remove(num_max - 1);
					return static_cast<Herror>(SavedModelID.Num());
				}
				catch (HException & message)
				{
					set_tposition(disp_hand, 0, 1);
					Halcon::write_string(disp_hand, "没有模板");
					Halcon::write_string(disp_hand, message.message);
					return H_MSG_FAIL;
				}

			}
			else if (4 == selection)//返回模板数量
			{
				set_check("~give_error");
				return static_cast<Herror>(SavedModelID.Num());
			}
			else if (5 == selection)//显示模板
			{
				set_check("~give_error");
				if (model_selection >= SavedModelID.Num())
				{
					set_draw(disp_hand, "margin");
					Halcon::set_color(disp_hand, "red");
					set_tposition(disp_hand, 0, 1);
					Halcon::write_string(disp_hand, "没有模板");
					return (!H_MSG_TRUE);
				}
				try
				{
					Herror err = get_shape_model_contours(&ShapeModel, SavedModelID[model_selection], 1);
					clear_window(disp_hand);
					if (H_MSG_TRUE != err)
					{
						set_tposition(disp_hand, 0, 1);
						Halcon::write_string(disp_hand, "没有模板");
					}
					else
					{
						set_draw(disp_hand, "margin");
						Halcon::set_color(disp_hand, "blue");
						disp_obj(ShapeModel, disp_hand);
					}


				}
				catch (HException & message)
				{
					set_tposition(disp_hand, 0, 1);
					Halcon::write_string(disp_hand, "没有模板");
					Halcon::write_string(disp_hand, message.message);
				}
			}
		}
		catch (HException & message)
		{
			set_tposition(disp_hand, 0, 1);
			Halcon::write_string(disp_hand, "执行失败");
			Halcon::write_string(disp_hand, message.message);
			return H_MSG_FAIL;
		}
		return H_MSG_TRUE;
	}
#endif


	/*检测函数部分END*/














	void gen_circle_roi(Halcon::Hobject & dst, double & cx, double & cy, double &r, int sel = 0)
	{
		Halcon::gen_circle(&dst,cx,cy,r);
	}

	void gen_rect_roi(Halcon::Hobject & dst, double &r1, double &c1, double &r2, double &c2, int sel = 0) 
	{
		Halcon::gen_rectangle1(&dst, r1, c1, r2, c2);
	}

	void GenHobject(int w, int h, unsigned char* img, Halcon::Hobject &Image,int ch = 1)
	{
		if (1 == ch)
			Halcon::gen_image1(&Image, "byte", (Hlong)w, (Hlong)h, (Hlong)img);
		else
		{
			uchar * r = NULL, *g = NULL, *b = NULL;
			split_color(w, h, 0, img, &r);
			split_color(w, h, 1, img, &g);
			split_color(w, h, 2, img, &b);
			Halcon::gen_image3(&Image, "byte", (Hlong)w, (Hlong)h, (Hlong)r,(Hlong)g,(Hlong)b);
		}
	}

	void Disp_Obj(const Halcon::Hobject & obj, const Halcon::HTuple & disp_hand)
	{
		try
		{
			Halcon::set_check("~give_error");
			Hlong w, h;
			Halcon::get_image_pointer1(obj, NULL, NULL, &w, &h);
			Halcon::HDevWindowStack::Push(disp_hand);
			Halcon::HDevWindowStack::SetActive(disp_hand);
			Halcon::set_part(disp_hand, NULL, NULL, h, w);
			disp_obj(obj, disp_hand);
		}
		catch (Halcon::HException &except)
		{
			Halcon::set_check("give_error");
			TRACE(except.message);
			Halcon::set_tposition(disp_hand, 0, 1);
			Halcon::write_string(disp_hand, except.message);
		}
	}

	int InitWindow(HWND Top, CRect TopRt, HWND Bottom, CRect BottomR)
	{
		if (Bottom == NULL || Top == NULL) return 1;
		try {
			WindowHandle = (Hlong)Top;
			WindowHandle1 = (Hlong)Bottom;

			Halcon::set_check("give_error");
			Halcon::set_check("~father");
			Halcon::open_window(0, 0, TopRt.Width(), TopRt.Height(), WindowHandle, "visible", "", &disp_hd);;
			Halcon::set_check("father");

			Halcon::set_check("~father");
			Halcon::open_window(0, 0, BottomR.Width(), BottomR.Height(), WindowHandle1, "visible", "", &disp_hd1);
			Halcon::set_check("father");

			Halcon::HDevWindowStack::Push(WindowHandle);
			Halcon::HDevWindowStack::Push(WindowHandle1);
		}
		catch (Halcon::HException &except)
		{
			CString str = (CString)(CStringA)(LPCSTR)except.message;
			AfxMessageBox(L"错误代码：" + str);
			return 1;
		}
		return 0;
	}


	void MatToHImage(cv::Mat& pImage, Hobject& Hobj)
	{
		//Hobject Hobj = NULL;
		if (3 == pImage.channels())
		{
			cv::Mat pImageRed, pImageGreen, pImageBlue;
			std::vector<cv::Mat> sbgr(3);
			cv::split(pImage, sbgr);

			int length = pImage.rows * pImage.cols;
			uchar *dataBlue = new uchar[length];
			uchar *dataGreen = new uchar[length];
			uchar *dataRed = new uchar[length];

			int height = pImage.rows;
			int width = pImage.cols;
			for (int row = 0; row <height; row++)
			{
				uchar* ptr = pImage.ptr<uchar>(row);
				for (int col = 0; col < width; col++)
				{
					dataBlue[row * width + col] = ptr[3 * col];
					dataGreen[row * width + col] = ptr[3 * col + 1];
					dataRed[row * width + col] = ptr[3 * col + 2];
				}
			}

			gen_image3(&Hobj, "byte", width, height, (Hlong)(dataRed), (Hlong)(dataGreen), (Hlong)(dataBlue));
			delete[] dataRed;
			delete[] dataGreen;
			delete[] dataBlue;
		}
		else if (1 == pImage.channels())
		{
			int height = pImage.rows;
			int width = pImage.cols;
			uchar *dataGray = new uchar[width * height];
			memcpy(dataGray, pImage.data, width * height);
			gen_image1(&Hobj, "byte", width, height, (Hlong)(dataGray));
			delete[] dataGray;
		}

	}



	void HImageToMat(Hobject &Hobj, cv::Mat& pImage)
	{
		using namespace cv;
		Hlong htChannels;
		char cType[MAX_STRING];
		Hlong     width, height;
		width = height = 0;
		//转换图像格式  
		convert_image_type(Hobj, &Hobj, "byte");
		count_channels(Hobj, &htChannels);
		if (htChannels == 1)
		{
			unsigned char *ptr;
			get_image_pointer1(Hobj, (Hlong *)&ptr, cType, &width, &height);
			pImage = cv::Mat(static_cast<int>(height), static_cast<int>(width), CV_8UC1);
			memcpy(pImage.data, ptr, width * height);

		}
		else if (htChannels == 3)
		{

			unsigned char *ptrRed, *ptrGreen, *ptrBlue;
			ptrRed = ptrGreen = ptrBlue = NULL;
			get_image_pointer3(Hobj, (Hlong *)&ptrRed, (Hlong *)&ptrGreen, (Hlong *)&ptrBlue, cType, &width, &height);
			pImage = cv::Mat(static_cast<int>(height), static_cast<int>(width), CV_8UC3);
			for (int row = 0; row < height; row++)
			{
				uchar* ptr = pImage.ptr<uchar>(row);
				for (int col = 0; col < width; col++)
				{
					ptr[3 * col] = ptrBlue[row * width + col];
					ptr[3 * col + 1] = ptrGreen[row * width + col];
					ptr[3 * col + 2] = ptrRed[row * width + col];
				}
			}

		}
	}








	cv::Mat mimg, mimg1;
	Halcon::Hobject	Image, Image1;
	Halcon::HTuple disp_hd, disp_hd1;
private:
	Halcon::HTuple AcqHandle, AcqHandle1;
	Halcon::HTuple WindowHandle, WindowHandle1;
	//分离色彩通道函数，需要释放内存
	void split_color(int width, int height, int ch, uchar* src, uchar** dst) const
	{
		if (ch > 2) return;
		if (*dst) {
			free(*dst); *dst = NULL;
		}
		*dst = (uchar*)malloc(width*height * sizeof(char));
		uchar *s = src; uchar *d = *dst;
		for (int h = 0; h < height; h++)
		{
			for (int w = 0; w < width; w++)
			{
				d[w] = s[w * 3 + ch];
			}
			d += width;
			s += 3 * width;
		}
	}
};
#endif 