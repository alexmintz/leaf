#pragma once


// ID_REC_LIST dialog

class ID_REC_LIST : public CDialog
{
	DECLARE_DYNAMIC(ID_REC_LIST)

public:
	ID_REC_LIST(CWnd* pParent = NULL);   // standard constructor
	virtual ~ID_REC_LIST();

// Dialog Data
	enum { IDD = IDD_MD_REPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
};
