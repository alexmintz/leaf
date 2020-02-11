// ChildFrm.cpp : implementation of the CChildFrame class
//
#include "stdafx.h"
#include "md_monitor.h"
#include <algorithm>
#include "md_monitorDoc.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	ON_WM_CLOSE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}
int CChildFrame::OnCreate( LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	CMenu *pSysMenu = GetSystemMenu(FALSE);
	ASSERT(pSysMenu != NULL);
	VERIFY(pSysMenu->RemoveMenu(SC_CLOSE, MF_BYCOMMAND));
	return 0;
}

void CChildFrame::OnClose()
{
	CMDIChildWnd::OnClose();
}
void CChildFrame::ActivateFrame(int nCmdShow) 
{
	CString csDocString;
	WINDOWPLACEMENT wp;
	WINDOWPLACEMENT wpOrg;
	GetWindowPlacement(&wpOrg);
	GetWindowPlacement(&wp);

	Cmd_monitorDoc* pDoc = dynamic_cast<Cmd_monitorDoc*>(GetActiveDocument());
	
	ASSERT(pDoc);

	CDocTemplate* pTemplate = pDoc->GetDocTemplate();

	pTemplate->GetDocString(csDocString, pTemplate->regFileTypeId);
	
	csDocString += pDoc->_doc_id;

	wp.rcNormalPosition.top = theApp.GetProfileInt(csDocString, CA2T("top"), wpOrg.rcNormalPosition.top);
	wp.rcNormalPosition.bottom = theApp.GetProfileInt(csDocString, CA2T("bottom"), wpOrg.rcNormalPosition.bottom);
	wp.rcNormalPosition.left = theApp.GetProfileInt(csDocString, CA2T("left"), wpOrg.rcNormalPosition.left);
	wp.rcNormalPosition.right = theApp.GetProfileInt(csDocString, CA2T("right"), wpOrg.rcNormalPosition.right);

	SetWindowPlacement(&wp);
	CMDIChildWnd::ActivateFrame(nCmdShow);
}
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG


// CChildFrame message handlers
