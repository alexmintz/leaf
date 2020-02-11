// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#include "PlayerDlgBar.h"

class Cmd_monitorView;
class Cmd_graphView;
class Cp_RTPosView;
class Cp_OrderView;
class Cp_OrderFillView;

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	virtual ~CMainFrame();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	PlayerDlgBar& get_player()		{ return m_wndDlgBar;}
	
	Cmd_monitorView*	get_monitor();
	Cmd_graphView*		get_graph();
	Cp_RTPosView*		get_pos();	
	Cp_OrderView*		get_order();	
	Cp_OrderFillView*	get_fill();

	void set_monitor(Cmd_monitorView* mv_)	{ _monitor_view = mv_;}
	void set_graph(Cmd_graphView* gv_)		{ _graph_view = gv_;}
	void set_pos(Cp_RTPosView* pv_)			{ _rtpos_view = pv_;}
	void set_order(Cp_OrderView* ov_)		{ _order_view = ov_;}
	void set_fill(Cp_OrderFillView* fv_)	{ _fill_view = fv_;}




#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	
	CToolBar		m_wndToolBar;
	PlayerDlgBar	m_wndDlgBar;

	Cmd_monitorView*	_monitor_view;
	Cmd_graphView*		_graph_view;
	Cp_RTPosView*		_rtpos_view;
	Cp_OrderView*		_order_view;
	Cp_OrderFillView*	_fill_view;


// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewPlayerbar();

DECLARE_MESSAGE_MAP()
};


