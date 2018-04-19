#include "stdafx.h"
#pragma once

#include "DMC1380Card.h"
#include "../MSerialsCommon/Tools.h"
#include "../MSerialsCommon/common.h"
#include "third_party\Preferences\Preferences.h"
#include "MSerialsCam.h"
#include "halcon.h"



//输出
/*

1	报警
2	正反信号
3	正反检测完成信号
4	拉信号
5	拉检测完成信号

6	正反光源
7	漏拉光源

输入
1	正反
2	拉信号


*/
class Machine
{
private:
	DMC1380Card * mc = nullptr;
	Machine();
public:
	CEvent evt_cam1;
	CEvent evt_cam2;
	static Machine* GetIns() { static Machine m_mc; return &m_mc; }
	virtual ~Machine();
	int Init() {
		if (nullptr != mc) return 0;;
		mc = new DMC1380Card();
		if (0 == mc->Init()) {
			AfxMessageBox(L"没有发现dmc1380 IO卡");
			return 0;
		}
		mc->WriteOutPutBit(1, 0);
		CWinThread * T = AfxBeginThread(IOScanner, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		T = AfxBeginThread(firstProc, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		T = AfxBeginThread(SecondProc, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		return 1;
	};

	//io扫描
	static UINT IOScanner(LPVOID lp)
	{
		Machine *pDlg = (Machine*)lp;
		DWORD new_first_cam_in = pDlg->mc->ReadInPutBit(1);
		DWORD old_first_cam_in = new_first_cam_in;
		DWORD new_second_cam_in = pDlg->mc->ReadInPutBit(2);
		DWORD old_second_cam_in = new_second_cam_in;
		for (;;) {
			new_first_cam_in = pDlg->mc->ReadInPutBit(1);
			new_second_cam_in = pDlg->mc->ReadInPutBit(2);
			
			if (new_first_cam_in && old_first_cam_in != new_first_cam_in)
			{
				pDlg->evt_cam1.SetEvent();
			}

			if (new_second_cam_in && old_second_cam_in != new_second_cam_in)
			{
				pDlg->evt_cam2.SetEvent();
			}

			old_first_cam_in = new_first_cam_in;
			old_second_cam_in = new_second_cam_in;
			//扫描IO
			Sleep(2);
		}
	}


	//工位1处理
	static UINT firstProc(LPVOID lp) {
		Machine *pDlg = (Machine*)lp;
		for (;;) {
			::WaitForSingleObject(pDlg->evt_cam1, INFINITE);
			//处理函数后输出结果

			pDlg->mc->WriteOutPutBit(6, 0);



			using namespace Halcon;
			CImageCard * ic = CImageCard::GetIns();
			CMainFrame * pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
			int w = 0;
			int h = 0;
			int ch = 1;
			unsigned char * data = NULL;
			//因为电脑性能不是很好，所以采用这个snap来采取图片
			Snap(w, h, &data, ch, 0, 0, pMainFrm->sys.m_nSnapTimeDelay);
			if (0 == w || 0 == h || NULL == data)	return OBJECT_NO_PIC;

			Halcon::set_check("~give_error");
			int r1 = static_cast<int>(pMainFrm->ini.trect.r1);
			int c1 = static_cast<int>(pMainFrm->ini.trect.c1);
			int width = static_cast<int>(pMainFrm->ini.trect.c2) - c1;
			int height = static_cast<int>(pMainFrm->ini.trect.r2) - r1;
			cv::Rect rt(c1, r1, width, height);

			cv::Mat src;
			if (1 == ch)
				src = cv::Mat(h, w, CV_8UC1);
			else
				src = cv::Mat(h, w, CV_8UC3);
			//因为在sdk里已经有缓存了，利用缓存来弄
			src.data = data;
			Halcon::set_tposition(CImageCard::GetIns()->disp_hd, 0, 1);
			float res = cv_predict_inv(src, rt, false);
			CImageCard::GetIns()->MatToHImage(src, CImageCard::GetIns()->Image);
			CImageCard::GetIns()->Disp_Obj(CImageCard::GetIns()->Image, CImageCard::GetIns()->disp_hd);

			int isInv = false;
			Hobject ROI;
			char str[256] = { 0 };

			if (res > 0)
			{
				sprintf(str, "正面 预测值:%f", res);
				Halcon::set_color(CImageCard::GetIns()->disp_hd, "green");
			}
			else
			{
				sprintf(str, "反面或无料 预测值:%f", res);
				Halcon::set_color(CImageCard::GetIns()->disp_hd, "red");
				isInv = true;
			}
			set_draw(CImageCard::GetIns()->disp_hd, "margin");
			gen_rectangle1(&ROI,
				pMainFrm->ini.trect.r1,
				pMainFrm->ini.trect.c1,
				pMainFrm->ini.trect.r2,
				pMainFrm->ini.trect.c2);
			disp_obj(ROI, CImageCard::GetIns()->disp_hd);
			write_string(CImageCard::GetIns()->disp_hd, str);

			//输出结果
			if (isInv)
			{
				pDlg->mc->WriteOutPutBit(2, 1);
			}
			else
			{
				pDlg->mc->WriteOutPutBit(2, 0);	
			}
			pDlg->mc->WriteOutPutBit(3, 0);
			Sleep(120);
			pDlg->mc->WriteOutPutBit(3, 1);
			pDlg->mc->WriteOutPutBit(2, 1);
			//关闭光源
			pDlg->mc->WriteOutPutBit(6, 1);


			pDlg->evt_cam1.ResetEvent();
		}
	}

	//工位2处理
	static UINT SecondProc(LPVOID lp)
	{
		Machine *pDlg = (Machine*)lp;
		for (;;) {
			::WaitForSingleObject(pDlg->evt_cam2, INFINITE);
			//关闭光源
			pDlg->mc->WriteOutPutBit(7, 0);
			int Res = OBJECT_VOID;
			//处理函数后输出结果
			using namespace Halcon;
			CImageCard * ic = CImageCard::GetIns();
			CMainFrame * pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
			int w = 0;
			int h = 0;
			int ch = 1;
			unsigned char * data = NULL;
			//因为电脑性能不是很好，所以采用这个snap来采取图片
			Snap(w, h, &data, ch, 0, 1, pMainFrm->sys.m_nSnapTimeDelay);
			if (0 == w || 0 == h || NULL == data)	return OBJECT_NO_PIC;
			Halcon::set_check("~give_error");
			//因为在sdk里已经有缓存了，利用缓存来弄
			try
			{
				CImageCard::GetIns()->GenHobject(w, h, data, CImageCard::GetIns()->Image1, ch);
				CImageCard::GetIns()->Disp_Obj(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1);
				Halcon::set_tposition(CImageCard::GetIns()->disp_hd1, 0, 1);
				double r = pMainFrm->ini.bcircle.r;
				double c = pMainFrm->ini.bcircle.c;
				double radius = pMainFrm->ini.bcircle.radius;
				Herror error = CImageCard::GetIns()->hc_check_gear(CImageCard::GetIns()->Image1, CImageCard::GetIns()->disp_hd1, r, c, radius, 1, pMainFrm->ini.accurcy);
				if (H_MSG_TRUE != error)
					Res = OBJECT_VOID;
				Res = OBJECT_OK;
			}
			catch (HException &message)
			{
				write_string(CImageCard::GetIns()->disp_hd1, message.message);
				Res = OBJECT_VOID;
			}

			if (Res == OBJECT_VOID)
			{
				pDlg->mc->WriteOutPutBit(4, 1);
			}
			else
			{
				pDlg->mc->WriteOutPutBit(4, 0);
			}
				pDlg->mc->WriteOutPutBit(5, 0);

				Sleep(120);
				pDlg->mc->WriteOutPutBit(5, 1);
				pDlg->mc->WriteOutPutBit(4, 1);
		
		
				//关闭光源
				pDlg->mc->WriteOutPutBit(7, 1);
			pDlg->evt_cam2.ResetEvent();
		}
	}

	
};






Machine::Machine()
{
	evt_cam1.ResetEvent();
	evt_cam2.ResetEvent();
}


Machine::~Machine()
{
	if (nullptr == mc) return;
	mc->WriteOutPutBit(1, 1);
	mc->board_close();
	
}
