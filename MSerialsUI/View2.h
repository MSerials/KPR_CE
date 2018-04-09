#pragma once


// View2 视图

class View2 : public CView
{
	DECLARE_DYNCREATE(View2)

protected:
	View2();           // 动态创建所使用的受保护的构造函数
	virtual ~View2();

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	HWND get_handle() { return this->m_hWnd; }
	CRect get_rect() {CRect rect;GetClientRect(&rect); return rect;}
protected:
	DECLARE_MESSAGE_MAP()
};

extern View2 *pview2;
