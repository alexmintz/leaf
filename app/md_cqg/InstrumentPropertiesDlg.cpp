// InstrumentPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"

#include <cassert>

#include "Common.h"
#include "InstrumentProperties.h"
#include "QFormulasDlg.h"
#include "InstrumentPropertiesDlg.h"


const CString CInstrumentPropertiesDlg::m_N_A  = _T("N/A");
const int CInstrumentPropertiesDlg::m_stPropsCount = 38;
const int CInstrumentPropertiesDlg::m_stDOMCount = 3; 

// CInstrumentPropertiesDlg dialog

CInstrumentPropertiesDlg::CInstrumentPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInstrumentPropertiesDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

//////////////////////////////////////////////////////////////////////////
//////////////////// Protected methods
//////////////////////////////////////////////////////////////////////////
void CInstrumentPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_ST_DATA_CONNECTION, m_lbDataConnection);
   DDX_Control(pDX, IDC_EDSYMBOL_NAME, m_edSymbolName);
   DDX_Control(pDX, IDC_LSTINSTRUMENTS, m_lsInstruments);
   DDX_Control(pDX, IDC_STINSTR_DESC, m_lblInstrDescription);
   DDX_Control(pDX, IDC_BTNSUBSCRIBE, m_btSubscribe);
   DDX_Control(pDX, IDC_BTNREMOVE, m_btRemove);
   DDX_Control(pDX, IDC_LSVPROPERTIES, m_lvProperties);
   DDX_Control(pDX, IDC_LSVSESSIONS, m_lvSessions);
   DDX_Control(pDX, IDC_BTNREMOVEALL, m_btRemoveAll);
   DDX_Control(pDX, IDC_BTN_QFORMULA, m_btnQFormula);
}

BEGIN_MESSAGE_MAP(CInstrumentPropertiesDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
   ON_BN_CLICKED(IDC_BTNREMOVE, OnBnClickedBtnRemove)
   ON_BN_CLICKED(IDC_BTNREMOVEALL, OnBnClickedBtnRemoveAll)
   ON_BN_CLICKED(IDC_BTNSUBSCRIBE, OnBnClickedBtnSubscribe)
   ON_LBN_SELCHANGE(IDC_LSTINSTRUMENTS, OnLbnSelChangeLstInstruments)
   ON_BN_CLICKED(IDC_BTN_QFORMULA, OnBnClickedBtnQFormula)
   ON_WM_DESTROY()
END_MESSAGE_MAP()


// CInstrumentPropertiesDlg message handlers

BOOL CInstrumentPropertiesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

   try
   {
      InitializeControls();
	  InitializeCQGCEL();
   }
   catch (std::exception& e) 
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInstrumentPropertiesDlg::OnOK()
{
   // Override to prevent dialog closing on Enter
}

void CInstrumentPropertiesDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

void CInstrumentPropertiesDlg::OnDestroy()
{
   try
   {
      if (m_spCQGCEL)
      {
         HRESULT hr = ICQGCELDispEventImpl::DispEventUnadvise(m_spCQGCEL);
         assert(SUCCEEDED(hr));

         hr = m_spCQGCEL->Shutdown();
         AssertCOMError(m_spCQGCEL, hr);
      }
   }
   catch (std::exception& e) 
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   CDialog::OnDestroy();
}

HCURSOR CInstrumentPropertiesDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//////////////////////////////////////////////////////////////////////////
//////////////////// Dialog controls events handlers
//////////////////////////////////////////////////////////////////////////
void CInstrumentPropertiesDlg::OnBnClickedBtnRemove()
{
   try
   {
      ATL::CComBSTR fullName = GetSelectedFullName();
      if (fullName == NULL)
      {
         return;
      }

      ATL::CComPtr<ICQGInstrument> spInstrument = GetInstrument(fullName);

      // Remove instrument from CEL
      HRESULT hr = m_spCQGCEL->RemoveInstrument(spInstrument);
      AssertCOMError(m_spCQGCEL, hr);

      // Remove from list
      int selRow = m_lsInstruments.FindStringExact(0, ATL::CW2T(fullName));
      m_lsInstruments.DeleteString(selRow);

      // Select another row
      int count = m_lsInstruments.GetCount();
      if (count > 0)
      {
         m_lsInstruments.SetCurSel(selRow >= count ? count - 1 : selRow);
         OnLbnSelChangeLstInstruments();
      }
      else
      {
         ClearAllData();
      }
      
      EnableControls(TRUE);
   }
   catch (const std::runtime_error& e) 
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()));
   }
}

void CInstrumentPropertiesDlg::OnBnClickedBtnRemoveAll()
{
   try
   {
      // Remove all instrument from CEL
      HRESULT hr = m_spCQGCEL->RemoveAllInstruments();
      AssertCOMError(m_spCQGCEL, hr);

      // Clear list
      m_lsInstruments.ResetContent();

      // Clear all data associated with selected instrument
      ClearAllData();

      EnableControls(TRUE);
   }
   catch (const std::runtime_error& e) 
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()));
   }
}

void CInstrumentPropertiesDlg::OnBnClickedBtnSubscribe()
{
   try
   {
      CString symbol;
      m_edSymbolName.GetWindowText(symbol);

      // Clear edit box
      m_edSymbolName.SetWindowText(_T(""));

      // Request instrument
      HRESULT hr = m_spCQGCEL->NewInstrument(ATL::CT2W(symbol));
      AssertCOMError(m_spCQGCEL, hr);
   }
   catch (const std::runtime_error& e) 
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()));
   }
}

void CInstrumentPropertiesDlg::OnLbnSelChangeLstInstruments()
{
   try
   {
      // Selected instrument's index
      int curIdx = m_lsInstruments.GetCurSel();

      // Clear all data
      ClearAllData();
      
      CString instrumentName;
      int count = m_lsInstruments.GetCount();
      for (int i = 0; i < count; ++i)
      {
         m_lsInstruments.GetText(i, instrumentName);
         ATL::CComBSTR fullName = ATL::CT2W(instrumentName);

         ATL::CComPtr<ICQGInstrument> spInstrument = GetInstrument(fullName);

         if (spInstrument != NULL)
         {
            if (i == curIdx)
            {
               // Show selected instrument's data 
               // and set subscription level to dsQuotesAndDOM to get 
               // DOM data
               //hr = spInstrument->put_DataSubscriptionLevel(dsQuotesAndDOM);
               //AssertCOMError(spInstrument, hr);

               ShowSessions(spInstrument);
               ShowAllProperties(spInstrument);
               ShowQuotes(spInstrument);
            }
            else
            {
               // Change not selected instrument's subscription level 
               // to get only best bid and ask
               //hr = spInstrument->put_DataSubscriptionLevel(dsQuotesAndBBA);
               //AssertCOMError(spInstrument, hr);
            }
         }
      }

      EnableControls(TRUE);
   }
   catch (const std::runtime_error& e) 
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()));
   }
}

void CInstrumentPropertiesDlg::OnBnClickedBtnQFormula()
{
   try
   {
      CQFormulasDlg dlg(m_spCQGCEL, this);
      if (dlg.DoModal() == IDOK)
      {
         //CString qFormulaName = dlg.GetQFormulaName();
         m_edSymbolName.SetWindowText(dlg.GetQFormulaName());
      }
   }
   catch (const std::runtime_error& e) 
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()));
   }
}
