// md_monitorDoc.cpp : implementation of the Cmd_monitorDoc class
//

#include "stdafx.h"
#include "md_monitor.h"

#include "md_monitorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cmd_monitorDoc

IMPLEMENT_DYNCREATE(Cmd_monitorDoc, CDocument)
IMPLEMENT_DYNCREATE(ORDER_Doc, Cmd_monitorDoc)
IMPLEMENT_DYNCREATE(DOM_Doc, Cmd_monitorDoc)
IMPLEMENT_DYNCREATE(GRAPH_Doc, Cmd_monitorDoc)
IMPLEMENT_DYNCREATE(POS_Doc, Cmd_monitorDoc)
IMPLEMENT_DYNCREATE(FILL_Doc, Cmd_monitorDoc)
IMPLEMENT_DYNCREATE(BATCH_Doc, Cmd_monitorDoc)

// Cmd_monitorDoc construction/destruction

Cmd_monitorDoc::Cmd_monitorDoc()
{}

Cmd_monitorDoc::~Cmd_monitorDoc()
{}

void Cmd_monitorDoc::PreCloseFrame(CFrameWnd *pFrame)
{
	CString csDocString;
	m_pDocTemplate->GetDocString(csDocString, m_pDocTemplate->regFileTypeId);

	WINDOWPLACEMENT wp;

	pFrame->GetWindowPlacement(&wp);
	csDocString += _doc_id;
	theApp.WriteProfileInt(csDocString, CA2T("top"), wp.rcNormalPosition.top);
	theApp.WriteProfileInt(csDocString, CA2T("bottom"), wp.rcNormalPosition.bottom);
	theApp.WriteProfileInt(csDocString, CA2T("left"), wp.rcNormalPosition.left);
	theApp.WriteProfileInt(csDocString, CA2T("right"), wp.rcNormalPosition.right);
}
