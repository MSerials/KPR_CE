// View1.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MSerialsUI.h"
#include "View1.h"


// View1
View1 *pview1 = NULL;
IMPLEMENT_DYNCREATE(View1, CView)

View1::View1()
{
	pview1 = this;
}

View1::~View1()
{
}

BEGIN_MESSAGE_MAP(View1, CView)
END_MESSAGE_MAP()


// View1 ��ͼ

void View1::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  �ڴ���ӻ��ƴ���
}


// View1 ���

#ifdef _DEBUG
void View1::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void View1::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// View1 ��Ϣ�������
