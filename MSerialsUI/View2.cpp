// View2.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MSerialsUI.h"
#include "View2.h"


// View2
View2 *pview2 = NULL;
IMPLEMENT_DYNCREATE(View2, CView)

View2::View2()
{
	pview2 = this;
}

View2::~View2()
{
}

BEGIN_MESSAGE_MAP(View2, CView)
END_MESSAGE_MAP()


// View2 ��ͼ

void View2::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  �ڴ���ӻ��ƴ���
}


// View2 ���

#ifdef _DEBUG
void View2::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void View2::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// View2 ��Ϣ�������
