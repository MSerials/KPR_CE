// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问 
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "MSerialsUI.h"

#include "MainFrm.h"
#include "SysControl.h"
#include "MSerialsCam.h"
#include "View1.h"
#include "View2.h"
#include "halcon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnOptions)
	ON_COMMAND(ID_FILE_PRINT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnUpdateFilePrintPreview)
	ON_COMMAND(ID_BUTTON_TEST2, &CMainFrame::OnButtonTest2)
	ON_WM_CLOSE()
	ON_COMMAND(ID_BUTTON_SETTINGS, &CMainFrame::OnButtonSettings)
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_WINDOWS_7);

	win_tool wt;
	CString AppPath = wt.AppPath();
	CString SysPath = AppPath + L"\\" + L"sys";
	sys.SetIniDir(SysPath, false);
	if (0 == sys.SetIniFile(L"PrgParameter.ini"))
	{
		sys.initData();
		sys.SaveParaFile(PARA_PRJ | PARA_IO);
	}
	sys.LoadParaFile(PARA_PRJ | PARA_IO);

	if (sys.m_Prj_Vector.IsEmpty()) { sys.initData(); sys.SaveParaFile(PARA_PRJ | PARA_IO); }
	vector<CString> strvec = wt.SplitCString(sys.m_Prj_Vector, L",");

	if (sys.m_Prj_Index >= static_cast<int>(strvec.size())) { sys.m_Prj_Index = 0; sys.SaveParaFile(PARA_PRJ | PARA_IO);}
	sys.m_Prj_Name = strvec[sys.m_Prj_Index];	

	CString PrjPath = AppPath + L"\\" + strvec[sys.m_Prj_Index];
	ini.SetIniDir(PrjPath, false);
	if (0 == ini.SetIniFile(L"PrgParameter.ini"))
	{
		ini.initData();
		ini.SaveParaFile(PARA_IMAGE);
	}
	ini.LoadParaFile(PARA_IMAGE);



	//初始化其他的全局对话框
	global::GetIns()->m_dlg_history = new CHistoryDlg();
	global::GetIns()->m_dlg_history->Create(IDD_HISTORY);
	//global::GetIns()->m_dlg_history->ShowWindow(SW_SHOWMAXIMIZED);
	global::GetIns()->m_dlg_statistic = new CStaticsShowDlg();
	global::GetIns()->m_dlg_statistic->Create(IDD_STATICS_DATA);
	//global::GetIns()->m_dlg_statistic->ShowWindow(SW_SHOWMAXIMIZED);

	global::GetIns()->txt = new CTxt(sys.m_Prj_Name);;
}

CMainFrame::~CMainFrame()
{
	//global::GetIns()->txt->Save();
	delete global::GetIns()->txt;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 创建标题栏: 
	if (!CreateCaptionBar())
	{
		TRACE0("未能创建标题栏\n");
		return -1;      // 未能创建
	}
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);





	//
	if (!m_DockOperation.Create(_T("控制面板"), this, CRect(0, 0, 600, 1080), TRUE, 1001, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI
		, AFX_CBRS_REGULAR_TABS | AFX_CBRS_RESIZE, AFX_CBRS_FLOAT// | AFX_CBRS_CLOSE  //| AFX_CBRS_AUTO_ROLLUP //| AFX_CBRS_FLOAT
	))
	{
		TRACE0("Failed to create LeftPane\n");
		return -1;
	}

	m_DockOperation.LoadState(FALSE);
	m_DockOperation.SetAutoHideMode(FALSE, NULL, NULL, NULL);
	m_DockOperation.EnableDocking(CBRS_ALIGN_BOTTOM);
	DockPane(&m_DockOperation);
	m_TabCtrl.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 1920, 1080), &m_DockOperation, 125);
	m_TabCtrl.init();

#ifdef _TEST
#pragma warning(disable: 4996)
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
#endif // DEBUG


	return 0;
}




BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("未能创建标题栏\n");
		return FALSE;
	}

	BOOL bNameValid;

	CString strTemp, strTemp2;
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);

	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnViewCaptionBar()
{
	m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

void CMainFrame::OnOptions()
{
	CMFCRibbonCustomizeDialog *pOptionsDlg = new CMFCRibbonCustomizeDialog(this, &m_wndRibbonBar);
	ASSERT(pOptionsDlg != NULL);

	pOptionsDlg->DoModal();
	delete pOptionsDlg;
}


void CMainFrame::OnFilePrint()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void CMainFrame::OnFilePrintPreview()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // 强制关闭“打印预览”模式
	}
}

void CMainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsPrintPreview());
}


void CMainFrame::OnButtonTest2()
{
	// TODO: 在此添加命令处理程序代码

}



void CMainFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	global::GetIns()->txt->Save();
	//delete global::GetIns()->txt;
	close_device();
	CFrameWndEx::OnClose();
}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 在此添加专用代码和/或调用基类
	// TODO: 在此添加专用代码和/或调用基类
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;
	m_DockOperation.ShowPane(TRUE, FALSE, TRUE);

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(View1), CSize(100, 100), pContext)) return FALSE;
	if (!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(View2), CSize(100, 100), pContext)) return FALSE;

	CView* pv1 = (CView*)this->m_wndSplitter.GetPane(0, 0);
	CView* pv2 = (CView*)this->m_wndSplitter.GetPane(0, 1);
	return TRUE;
	return CFrameWndEx::OnCreateClient(lpcs, pContext);
}

BOOL CMainFrame::Resize()
{
	CWnd * ClientCWnd = this;
	CRect rect;
	ClientCWnd->GetClientRect(&rect);
	int ControlWidth = 0;
	const int TotalWidth = rect.right;
	int ViewWidth = rect.Width()/2;// (TotalWidth - ControlWidth) / 2;
	int ViewHeight = rect.Height();// (rect.Height() - 184) / 2;
	m_wndSplitter.SetRowInfo(0, ViewHeight, 0);
	m_wndSplitter.SetRowInfo(1, ViewHeight, 0);
	m_wndSplitter.SetColumnInfo(0, ViewWidth, 80);
	m_wndSplitter.SetColumnInfo(1, ViewWidth, 80);
	m_wndSplitter.RecalcLayout();
	CRect rt(0, 0, ViewWidth, ViewHeight);
	//因为要在得到图形后才能初始化相机
	CImageCard::GetIns()->InitWindow(pview1->m_hWnd, pview1->get_rect(), pview2->m_hWnd, pview2->get_rect());
	return TRUE;
}

//放在MainFrame里方便其他地方获得这个函数
void CMainFrame::OnButtonSettings()
{
	// TODO: 在此添加命令处理程序代码




}

//CString CMainFrame::S()
