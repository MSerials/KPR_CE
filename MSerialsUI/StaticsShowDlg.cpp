// StaticsShowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MSerialsUI.h"
#include "StaticsShowDlg.h"
#include "afxdialogex.h"


// CStaticsShowDlg 对话框

IMPLEMENT_DYNAMIC(CStaticsShowDlg, CDialogEx)

CStaticsShowDlg::CStaticsShowDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_STATICS_DATA, pParent)
{

}

CStaticsShowDlg::~CStaticsShowDlg()
{
}

void CStaticsShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStaticsShowDlg, CDialogEx)
END_MESSAGE_MAP()


// CStaticsShowDlg 消息处理程序
void CStaticsShowDlg::showImgStatics(int Cesus[512])
{
#pragma omp parallel //多核心执行这个
	{
		const int orgx = 50;
		const int orgy = 450;
		const int xendx = 610; //565
		const int xendy = orgy;
		const int yendx = orgx;
		const int yendy = 20;

		// TODO:  在此添加消息处理程序代码和/或调用默认值
		CClientDC dc(this); //CPaintDC dc(this);在这里不起作用
		CPen* pPenBlue = new CPen;                                  //创建画笔对象
		pPenBlue->CreatePen(PS_SOLID, 2, RGB(0, 5, 196));     //蓝色画笔
		CPen* pPenBlack = new CPen;                                 //创建画笔对象
		pPenBlack->CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); //黑色画笔
														 //选中黑色画笔，并保存当前画笔
		CGdiObject* pOldPen = dc.SelectObject(pPenBlack);
		int i = 0;
		CString str;
		//绘制坐标系
		CPoint OPos(orgx, orgy), NowPos;
		//CPoint OPos(40, 500), NowPos;
		//绘制x坐标轴
		dc.MoveTo(OPos);

		NowPos.x = xendx;
		NowPos.y = xendy;
		dc.LineTo(NowPos);
		//绘制箭头
		dc.LineTo(xendx - 5, xendy - 5);
		dc.MoveTo(NowPos);
		dc.LineTo(xendx - 5, xendy + 5);
		//绘制x轴坐标系数
		for (i = 0; i <513; i++)
		{
			if (i % 10 == 0)
			{
				dc.MoveTo(OPos.x + i, OPos.y);
				dc.LineTo(CPoint(OPos.x + i, OPos.y - 5));
			}
			if (i % 80 == 0)
			{
				str.Format(L"%3.1f", (float)i/256.0+24);//512/256 说明2个毫米了
				dc.TextOut(OPos.x + i, OPos.y + 1, str);
			}
		}
		//绘制y轴坐标系数
		dc.MoveTo(OPos);
		NowPos.x = OPos.x;
		NowPos.y = yendy;
		dc.LineTo(NowPos);
		//绘制箭头
		dc.LineTo(NowPos.x - 5, NowPos.y + 5);
		dc.MoveTo(NowPos);
		dc.LineTo(NowPos.x + 5, NowPos.y + 5);
		//寻找数据数组最大的数据
		int max = 0;

		for (i = 0; i < 512; i++)
		{
			if (max < Cesus[i])
			{
				max = Cesus[i];
			}
		}
		//y轴坐标系数的数据步长
		int Tstep = max / 20;
		//y轴坐标系数的刻度步长
		int Ystep = orgy / 21;//?
							  //显示y坐标的刻度和数据
		for (i = 1; i != 22; i++)
		{
			//找到最高点
			dc.MoveTo(OPos.x, OPos.y - Ystep * i);
			dc.LineTo(CPoint(OPos.x + 5, OPos.y - Ystep * i));
			if (i % 2 == 0)
			{
				str.Format(L"%d", Tstep * i);
				dc.TextOut(0, OPos.y - Ystep * i - 10, str);
			}
		}
		//绘制灰度图像的直方图
		dc.SelectObject(pPenBlue); //选择蓝色画笔
								   //画直方图
		for (i = 0; i < 512; i++)
		{
			NowPos.x = OPos.x + i;
			NowPos.y = OPos.y;
			dc.MoveTo(NowPos);
			NowPos.y = (int)(orgy - orgy * 20.0f * Cesus[i] / (max * 22));
			//dc.MoveTo(NowPos.x - 1, NowPos.y - 1);
			dc.LineTo(NowPos);
		}
		//恢复以前的画笔
		dc.SelectObject(pOldPen);
		delete pPenBlue;
		delete pPenBlack;
	}
}


void CStaticsShowDlg::UpdateUI(int Cesus[512])
{

	/*
	CRect rect;//定义矩形类
	CWnd *pWnd = GetDlgItem(IDC_STATIC_STATIS);//获得pictrue控件窗口的句柄
	pWnd->GetClientRect(&rect);//获得pictrue控件所在的矩形区域
	//RedrawWindow(rect, NULL, RDW_ERASE | RDW_INVALIDATE);
	OnPaint();
	RedrawWindow(rect, NULL, RDW_INVALIDATE);
	//RedrawWindow(NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
	*/
	UpdateData(FALSE);
	CImage image; //创建图片类   
	CString str = L"res/bakgrd.bmp";
	image.Load(str); //根据图片路径加载图片   
	CRect rect;//定义矩形类   
	CWnd *pWnd = GetDlgItem(IDC_STATIC_DATA_SHOW);//获得pictrue控件窗口的句柄   
	pWnd->GetClientRect(&rect);//获得pictrue控件所在的矩形区域   
	CDC *pDC = pWnd->GetDC();//获得pictrue控件的DC   
	image.Draw(pDC->m_hDC, rect); //将图片画到Picture控件表示的矩形区域   
	ReleaseDC(pDC);//释放picture控件的DC  
	showImgStatics(Cesus);
}

void CStaticsShowDlg::UpdateUI()
{

		/*
		CRect rect;//定义矩形类
		CWnd *pWnd = GetDlgItem(IDC_STATIC_STATIS);//获得pictrue控件窗口的句柄
		pWnd->GetClientRect(&rect);//获得pictrue控件所在的矩形区域
		//RedrawWindow(rect, NULL, RDW_ERASE | RDW_INVALIDATE);
		OnPaint();
		RedrawWindow(rect, NULL, RDW_INVALIDATE);
		//RedrawWindow(NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
		*/
		UpdateData(FALSE);
		CImage image; //创建图片类   
		CString str = L"res/bakgrd.bmp";
		image.Load(str); //根据图片路径加载图片   
		CRect rect;//定义矩形类   
		CWnd *pWnd = GetDlgItem(IDC_STATIC_DATA_SHOW);//获得pictrue控件窗口的句柄   
		pWnd->GetClientRect(&rect);//获得pictrue控件所在的矩形区域   
		CDC *pDC = pWnd->GetDC();//获得pictrue控件的DC   
		image.Draw(pDC->m_hDC, rect); //将图片画到Picture控件表示的矩形区域   
		ReleaseDC(pDC);//释放picture控件的DC  
		OnPaint();

		int Cesus[512] = { 0 };
		showImgStatics(Cesus);

	
}

BOOL CStaticsShowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
