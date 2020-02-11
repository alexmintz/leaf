// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "md_monitor.h"

#include "MainFrm.h"
#include "md_monitorView.h"
#include "md_graphView.h"
#include "p_RTPosView.h"
#include "p_OrderView.h"
#include "p_OrderFillView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_PLAYERBAR, OnViewPlayerbar)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame() 
:_monitor_view(0), _graph_view(0)
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
	CFrameWnd::OnDestroy();
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableDocking(CBRS_ALIGN_ANY);
/*
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
*/
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
 
	if(!m_wndDlgBar.Create(this, IDD_DIALOGBAR,
		CBRS_TOP | CBRS_GRIPPER |CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_HIDE_INPLACE,
		IDD_DIALOGBAR))
	{
		TRACE0("Failed to create DlgBar\n");
		return -1;      // fail to create
	}
	m_wndDlgBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndDlgBar);
/*
	if(!m_wndDOM.Create(IDD_MD_REPORT, this))
	{
		TRACE0("Failed to create DlgBar\n");
		return -1;      // fail to create
	}
*/
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

// CMainFrame message handlers

void CMainFrame::OnViewPlayerbar()
{
	BOOL bVisible = ((m_wndDlgBar.IsWindowVisible()) != 0);
	ShowControlBar(&m_wndDlgBar, !bVisible, FALSE);
	RecalcLayout();
}

Cmd_monitorView* CMainFrame::get_monitor()	
{ 
	return _monitor_view; 
}
Cmd_graphView* CMainFrame::get_graph()
{
	return _graph_view;
}
Cp_RTPosView* CMainFrame::get_pos()	
{ 
	return _rtpos_view; 
}
Cp_OrderView* CMainFrame::get_order()
{
	return _order_view;
}
Cp_OrderFillView* CMainFrame::get_fill()
{
	return _fill_view;
}

