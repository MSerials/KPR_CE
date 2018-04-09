// StaticsShowDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MSerialsUI.h"
#include "StaticsShowDlg.h"
#include "afxdialogex.h"


// CStaticsShowDlg �Ի���

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


// CStaticsShowDlg ��Ϣ�������
void CStaticsShowDlg::showImgStatics(int Cesus[512])
{
#pragma omp parallel //�����ִ�����
	{
		const int orgx = 50;
		const int orgy = 450;
		const int xendx = 610; //565
		const int xendy = orgy;
		const int yendx = orgx;
		const int yendy = 20;

		// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
		CClientDC dc(this); //CPaintDC dc(this);�����ﲻ������
		CPen* pPenBlue = new CPen;                                  //�������ʶ���
		pPenBlue->CreatePen(PS_SOLID, 2, RGB(0, 5, 196));     //��ɫ����
		CPen* pPenBlack = new CPen;                                 //�������ʶ���
		pPenBlack->CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); //��ɫ����
														 //ѡ�к�ɫ���ʣ������浱ǰ����
		CGdiObject* pOldPen = dc.SelectObject(pPenBlack);
		int i = 0;
		CString str;
		//��������ϵ
		CPoint OPos(orgx, orgy), NowPos;
		//CPoint OPos(40, 500), NowPos;
		//����x������
		dc.MoveTo(OPos);

		NowPos.x = xendx;
		NowPos.y = xendy;
		dc.LineTo(NowPos);
		//���Ƽ�ͷ
		dc.LineTo(xendx - 5, xendy - 5);
		dc.MoveTo(NowPos);
		dc.LineTo(xendx - 5, xendy + 5);
		//����x������ϵ��
		for (i = 0; i <513; i++)
		{
			if (i % 10 == 0)
			{
				dc.MoveTo(OPos.x + i, OPos.y);
				dc.LineTo(CPoint(OPos.x + i, OPos.y - 5));
			}
			if (i % 80 == 0)
			{
				str.Format(L"%3.1f", (float)i/256.0+24);//512/256 ˵��2��������
				dc.TextOut(OPos.x + i, OPos.y + 1, str);
			}
		}
		//����y������ϵ��
		dc.MoveTo(OPos);
		NowPos.x = OPos.x;
		NowPos.y = yendy;
		dc.LineTo(NowPos);
		//���Ƽ�ͷ
		dc.LineTo(NowPos.x - 5, NowPos.y + 5);
		dc.MoveTo(NowPos);
		dc.LineTo(NowPos.x + 5, NowPos.y + 5);
		//Ѱ������������������
		int max = 0;

		for (i = 0; i < 512; i++)
		{
			if (max < Cesus[i])
			{
				max = Cesus[i];
			}
		}
		//y������ϵ�������ݲ���
		int Tstep = max / 20;
		//y������ϵ���Ŀ̶Ȳ���
		int Ystep = orgy / 21;//?
							  //��ʾy����Ŀ̶Ⱥ�����
		for (i = 1; i != 22; i++)
		{
			//�ҵ���ߵ�
			dc.MoveTo(OPos.x, OPos.y - Ystep * i);
			dc.LineTo(CPoint(OPos.x + 5, OPos.y - Ystep * i));
			if (i % 2 == 0)
			{
				str.Format(L"%d", Tstep * i);
				dc.TextOut(0, OPos.y - Ystep * i - 10, str);
			}
		}
		//���ƻҶ�ͼ���ֱ��ͼ
		dc.SelectObject(pPenBlue); //ѡ����ɫ����
								   //��ֱ��ͼ
		for (i = 0; i < 512; i++)
		{
			NowPos.x = OPos.x + i;
			NowPos.y = OPos.y;
			dc.MoveTo(NowPos);
			NowPos.y = (int)(orgy - orgy * 20.0f * Cesus[i] / (max * 22));
			//dc.MoveTo(NowPos.x - 1, NowPos.y - 1);
			dc.LineTo(NowPos);
		}
		//�ָ���ǰ�Ļ���
		dc.SelectObject(pOldPen);
		delete pPenBlue;
		delete pPenBlack;
	}
}


void CStaticsShowDlg::UpdateUI(int Cesus[512])
{

	/*
	CRect rect;//���������
	CWnd *pWnd = GetDlgItem(IDC_STATIC_STATIS);//���pictrue�ؼ����ڵľ��
	pWnd->GetClientRect(&rect);//���pictrue�ؼ����ڵľ�������
	//RedrawWindow(rect, NULL, RDW_ERASE | RDW_INVALIDATE);
	OnPaint();
	RedrawWindow(rect, NULL, RDW_INVALIDATE);
	//RedrawWindow(NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
	*/
	UpdateData(FALSE);
	CImage image; //����ͼƬ��   
	CString str = L"res/bakgrd.bmp";
	image.Load(str); //����ͼƬ·������ͼƬ   
	CRect rect;//���������   
	CWnd *pWnd = GetDlgItem(IDC_STATIC_DATA_SHOW);//���pictrue�ؼ����ڵľ��   
	pWnd->GetClientRect(&rect);//���pictrue�ؼ����ڵľ�������   
	CDC *pDC = pWnd->GetDC();//���pictrue�ؼ���DC   
	image.Draw(pDC->m_hDC, rect); //��ͼƬ����Picture�ؼ���ʾ�ľ�������   
	ReleaseDC(pDC);//�ͷ�picture�ؼ���DC  
	showImgStatics(Cesus);
}

void CStaticsShowDlg::UpdateUI()
{

		/*
		CRect rect;//���������
		CWnd *pWnd = GetDlgItem(IDC_STATIC_STATIS);//���pictrue�ؼ����ڵľ��
		pWnd->GetClientRect(&rect);//���pictrue�ؼ����ڵľ�������
		//RedrawWindow(rect, NULL, RDW_ERASE | RDW_INVALIDATE);
		OnPaint();
		RedrawWindow(rect, NULL, RDW_INVALIDATE);
		//RedrawWindow(NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
		*/
		UpdateData(FALSE);
		CImage image; //����ͼƬ��   
		CString str = L"res/bakgrd.bmp";
		image.Load(str); //����ͼƬ·������ͼƬ   
		CRect rect;//���������   
		CWnd *pWnd = GetDlgItem(IDC_STATIC_DATA_SHOW);//���pictrue�ؼ����ڵľ��   
		pWnd->GetClientRect(&rect);//���pictrue�ؼ����ڵľ�������   
		CDC *pDC = pWnd->GetDC();//���pictrue�ؼ���DC   
		image.Draw(pDC->m_hDC, rect); //��ͼƬ����Picture�ؼ���ʾ�ľ�������   
		ReleaseDC(pDC);//�ͷ�picture�ؼ���DC  
		OnPaint();

		int Cesus[512] = { 0 };
		showImgStatics(Cesus);

	
}

BOOL CStaticsShowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
