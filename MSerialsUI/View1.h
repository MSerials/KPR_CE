#pragma once


// View1 ��ͼ

class View1 : public CView
{
	DECLARE_DYNCREATE(View1)

protected:
	View1();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~View1();

public:
	virtual void OnDraw(CDC* pDC);      // ��д�Ի��Ƹ���ͼ
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	HWND get_handle() { return this->m_hWnd; }
	CRect get_rect() { CRect rect; GetClientRect(&rect); return rect; }
protected:
	DECLARE_MESSAGE_MAP()
};

extern View1 *pview1;
