// md_monitorDoc.h : interface of the Cmd_monitorDoc class
//
#pragma once

class Cmd_monitorDoc : public CDocument
{
protected: // create from serialization only
	Cmd_monitorDoc();
	virtual ~Cmd_monitorDoc();
	DECLARE_DYNCREATE(Cmd_monitorDoc)

// Implementation
public:
	virtual void PreCloseFrame(CFrameWnd* pFrame);
	CString _doc_id;
protected:
//	DECLARE_MESSAGE_MAP()

};

class ORDER_Doc: public Cmd_monitorDoc
{
protected:
	ORDER_Doc(){_doc_id = L"ORDER_Doc";}
	DECLARE_DYNCREATE(ORDER_Doc)
};
class DOM_Doc: public Cmd_monitorDoc
{
protected:
	DOM_Doc(){_doc_id = L"DOM_Doc";}
	DECLARE_DYNCREATE(DOM_Doc)
};
class GRAPH_Doc: public Cmd_monitorDoc
{
protected:
	GRAPH_Doc(){_doc_id = L"GRAPH_Doc";}
	DECLARE_DYNCREATE(GRAPH_Doc)
};
class POS_Doc: public Cmd_monitorDoc
{
protected:
	POS_Doc(){_doc_id = L"POS_Doc";}
	DECLARE_DYNCREATE(POS_Doc)
};
class FILL_Doc: public Cmd_monitorDoc
{
protected:
	FILL_Doc(){_doc_id = L"FILL_Doc";}
	DECLARE_DYNCREATE(FILL_Doc)
};
class BATCH_Doc: public Cmd_monitorDoc
{
protected:
	BATCH_Doc(){_doc_id = L"BATCH_Doc";}
	DECLARE_DYNCREATE(BATCH_Doc)
};