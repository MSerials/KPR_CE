// View2.cpp : 实现文件
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


// View2 绘图

void View2::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  在此添加绘制代码
}


// View2 诊断

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


// View2 消息处理程序
