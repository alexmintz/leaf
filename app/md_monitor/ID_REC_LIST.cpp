// ID_REC_LIST.cpp : implementation file
//

#include "stdafx.h"
#include "md_monitor.h"
#include "ID_REC_LIST.h"


// ID_REC_LIST dialog

IMPLEMENT_DYNAMIC(ID_REC_LIST, CDialog)

ID_REC_LIST::ID_REC_LIST(CWnd* pParent /*=NULL*/)
	: CDialog(ID_REC_LIST::IDD, pParent)
{

}

ID_REC_LIST::~ID_REC_LIST()
{
}

void ID_REC_LIST::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ID_REC_LIST, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_REC_LIST, &ID_REC_LIST::OnLvnItemchangedList1)
END_MESSAGE_MAP()


// ID_REC_LIST message handlers

void ID_REC_LIST::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
