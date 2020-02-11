// OrderPlacingDlg.cpp : implementation file
//

#include "stdafx.h"
#include <stdexcept>
#include <cassert>
#include "Common.h"
#include "OrderPlacing.h"
#include "OrderPlacingDlg.h"
#include "ModifyDlg.h"
#include "leaf/LFRunSettings.h"

const CString COrderPlacingDlg::m_N_A = _T("N/A");

const CString COrderPlacingDlg::m_SHORT_DATE = _T("dd/MM/yyyy");

const long COrderPlacingDlg::m_MIN_QUANTITY = 1;
const long COrderPlacingDlg::m_MAX_QUANTITY = 10000;

const double COrderPlacingDlg::m_INVALID_DOUBLE = -2147483648.;
const int COrderPlacingDlg::m_INVALID_INTEGER = 0x80000000;

COrderPlacingDlg::COrderPlacingDlg(
                                 CWnd* pParent/*=NULL*/)
   : CDialog(COrderPlacingDlg::IDD, pParent)
{
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COrderPlacingDlg::DoDataExchange(
                                 CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_LBL_DATA_CONNECTION, m_lblDataConnection);
   DDX_Control(pDX, IDC_LBL_GW_CONNECTION, m_lblGWConnection);
   DDX_Control(pDX, IDC_CMB_ACCOUNTS, m_cmbAccounts);
   DDX_Control(pDX, IDC_LBL_ASK_PRICE, m_lblAskPrice);
   DDX_Control(pDX, IDC_LBL_TRADE_PRICE, m_lblTradePrice);
   DDX_Control(pDX, IDC_LBL_BID_PRICE, m_lblBidPrice);
   DDX_Control(pDX, IDC_LBL_ASK_VOLUME, m_lblAskVolume);
   DDX_Control(pDX, IDC_LBL_TRADE_VOLUME, m_lblTradeVolume);
   DDX_Control(pDX, IDC_LBL_BID_VOLUME, m_lblBidVolume);
   DDX_Control(pDX, IDC_ED_SYMBOL_NAME, m_edSymbolName);
   DDX_Control(pDX, IDC_LST_INSTRUMENTS, m_lsInstruments);
   DDX_Control(pDX, IDC_LBL_INSTR_DESC, m_lblInstrDescription);
   DDX_Control(pDX, IDC_TAB_ORDERS, m_tabOrders);
   DDX_Control(pDX, IDC_LSV_ORDERS, m_lvOrders);
   DDX_Control(pDX, IDC_CMB_TYPE, m_cmbType);
   DDX_Control(pDX, IDC_CMB_DURATION, m_cmbDuration);
   DDX_Control(pDX, IDC_SPN_LIMIT_PRICE, m_spnLimitPrice);
   DDX_Control(pDX, IDC_SPN_STOP_PRICE, m_spnStopPrice);
   DDX_Control(pDX, IDC_SPN_QUANTITY, m_spnQuantity);
   DDX_Control(pDX, IDC_CHK_PARKED, m_chkParked);
   DDX_Control(pDX, IDC_BTN_MODIFY, m_btnModify);
   DDX_Control(pDX, IDC_BTN_ACTIVATE, m_btnActivate);
   DDX_Control(pDX, IDC_BTN_ACTIVATE_ALL, m_btnActivateAll);
   DDX_Control(pDX, IDC_BTN_CANCEL, m_btnCancel);
   DDX_Control(pDX, IDC_BTN_CANCEL_ALL, m_btnCancelAll);
   DDX_Control(pDX, IDC_BTN_REMOVE_INSTR, m_btnRemove);
   DDX_Control(pDX, IDC_BTN_REMOVE_ALL_INSTR, m_btnRemoveAll);
   DDX_Control(pDX, IDC_DTP_GTD, m_dtpGTD);
   DDX_Control(pDX, IDC_ED_QUANTITY, m_edQuantity);
   DDX_Control(pDX, IDC_ED_LIMIT_PRICE, m_edLimitPrice);
   DDX_Control(pDX, IDC_ED_STOP_PRICE, m_edStopPrice);
   DDX_Control(pDX, IDC_BTN_SUBSCRIBE, m_btnSubscribe);
   DDX_Control(pDX, IDC_BTN_BUY, m_btnBuy);
   DDX_Control(pDX, IDC_BTN_SELL, m_btnSell);
}

BEGIN_MESSAGE_MAP(COrderPlacingDlg, CDialog)
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_WM_DESTROY()
   ON_BN_CLICKED(IDC_BTN_SUBSCRIBE, OnBnClickedBtnSubscribe)
   ON_BN_CLICKED(IDC_BTN_REMOVE_INSTR, OnBnClickedBtnRemoveInstr)
   ON_BN_CLICKED(IDC_BTN_REMOVE_ALL_INSTR, OnBnClickedBtnRemoveAllInstr)
   ON_BN_CLICKED(IDC_BTN_BUY, OnBnClickedBtnBuy)
   ON_BN_CLICKED(IDC_BTN_SELL, OnBnClickedBtnSell)
   ON_BN_CLICKED(IDC_BTN_MODIFY, OnBnClickedBtnModify)
   ON_BN_CLICKED(IDC_BTN_ACTIVATE, OnBnClickedBtnActivate)
   ON_BN_CLICKED(IDC_BTN_ACTIVATE_ALL, OnBnClickedBtnActivateAll)
   ON_BN_CLICKED(IDC_BTN_CANCEL, OnBnClickedBtnCancel)
   ON_BN_CLICKED(IDC_BTN_CANCEL_ALL, OnBnClickedBtnCancelAll)
   ON_LBN_SELCHANGE(IDC_LST_INSTRUMENTS, OnLbnSelChangeLstInstruments)
   ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_ORDERS, OnTcnSelChangeTabOrders)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPN_STOP_PRICE, OnDeltaposSpStopPrice)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPN_LIMIT_PRICE, OnDeltaposSpLimitPrice)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPN_QUANTITY, OnDeltaposSpQuantity)
   ON_NOTIFY(LVN_ITEMCHANGED, IDC_LSV_ORDERS, OnLvnItemChangedLsvOrders)
   ON_EN_KILLFOCUS(IDC_ED_QUANTITY, OnEnKillfocusEdQuantity)
   ON_CBN_SELCHANGE(IDC_CMB_ACCOUNTS, OnCbnSelChangeCmbAccounts)
   ON_CBN_SELCHANGE(IDC_CMB_TYPE, OnCbnSelChangeCmbType)
   ON_CBN_SELCHANGE(IDC_CMB_DURATION, OnCbnSelChangeCmbDuration)
END_MESSAGE_MAP()

BOOL COrderPlacingDlg::OnInitDialog()
{
   try
   {
      CDialog::OnInitDialog();

      // Set the icon for this dialog.  The framework does this automatically
      // when the application's main window is not a dialog
      SetIcon(m_hIcon, TRUE);       // Set big icon
      SetIcon(m_hIcon, FALSE);      // Set small icon

      InitializeBooleans();
      InitializeControls();
      InitializeCQGCEL();
	  InitializeZSubs();
   }
   catch (std::exception& e) 
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   return TRUE;  // return TRUE  unless you set the focus to a control
}
COrderPlacingDlg::~COrderPlacingDlg()
{
	UninitializeZSubs();
}
void COrderPlacingDlg::InitializeZSubs()
{
	RunSettings::instance()->set_live_run(true);
	// live trading upates
	BMTS<LFOrderFill>::instance()->_out = BMTS_Properties::OUT_CHANNEL;
	BMTS<LFOrderUpdate>::instance()->_out = BMTS_Properties::OUT_CHANNEL;

	// live trading new/cancel/amend
	BMTS<LFNewOrder>::instance()->_in = BMTS_Properties::IN_RECEIVER;
	BMTS<LFCancelOrder>::instance()->_in = BMTS_Properties::IN_RECEIVER;
	

	BMModel::instance()->open();
	_star.reset(new Z::ActiveQueue(1, "ZCQG_Gateway_Queue"));
	_star->open();
	_s.reset(new BMStrategy(*_star, Z::make_callback(this, &COrderPlacingDlg::process_rt_run)));
	_s->set_name("ZCQG_Gateway");
	_s->open();		


	_s->subscribe_input(this, &COrderPlacingDlg::process_order, "", "new_order");
	_s->subscribe_input(this, &COrderPlacingDlg::process_cancel, "", "cancel_order");
//	_s->subscribe_input(this, &COrderPlacingDlg::process_amend, "", "amend_order");
}
void COrderPlacingDlg::process_order(LFNewOrderPtr* new_order_)
{
	try
	{
		PlaceOrder(new_order_);
		UnselectAllOrders();
	}
	catch (std::exception& e)
	{
		Z::report_alert("%s", e.what());
//		AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
	}
}
void COrderPlacingDlg::process_cancel(LFCancelOrderPtr* cancel_order_)
{
	try
	{
		CancelOrder(cancel_order_);
		UnselectAllOrders();
	}
	catch (std::exception& e)
	{
		Z::report_alert("%s", e.what());
//		AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
	}
}
/*
void COrderPlacingDlg::process_amend(LFOrderModifyPtr* update_order_)
{
	auto_ptr<LFOrderModifyPtr> uop(update_order_);
}
*/
void COrderPlacingDlg::UninitializeZSubs()
{
	_s->unsubscribe_input<LFNewOrder>("new_order");
	_s->unsubscribe_input<LFCancelOrder>("cancel_order");
	_s->unsubscribe_input<LFOrderUpdate>("amend_order");
	Z::report_debug("---closing BMModel");
	BMModel::instance()->close();

	Z::report_debug("---closing strategy");
	_s->close();
	_s.reset();

	Z::report_debug("---closing star");
	_star->close();
	_star.reset();

}
bool COrderPlacingDlg::process_rt_run()
{
	return true;
}
void COrderPlacingDlg::OnPaint() 
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

HCURSOR COrderPlacingDlg::OnQueryDragIcon()
{
   return static_cast<HCURSOR>(m_hIcon);
}

void COrderPlacingDlg::OnOK()
{
   // Override to prevent dialog from closing on Enter
}

void COrderPlacingDlg::OnDestroy()
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

void COrderPlacingDlg::OnBnClickedBtnSubscribe()
{
   try
   {
      CString symbol;
      m_edSymbolName.GetWindowText(symbol);

      if (symbol.Trim().GetLength() == 0)
      {
         // No text
         return;
      }

      // Clear edit box
      m_edSymbolName.SetWindowText(_T(""));

      // Request instrument
      HRESULT hr = m_spCQGCEL->NewInstrument(ATL::CT2W(symbol));
      AssertCOMError(m_spCQGCEL, hr);
   }
   catch (std::exception& e) 
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}

void COrderPlacingDlg::OnLbnSelChangeLstInstruments()
{
   try
   {
      // Clear instrument's old data
      ClearAllInstrumentData();

      // Get selected instrument
      ATL::CComPtr<ICQGInstrument> spInstrument = GetSelectedInstrument();

      if (spInstrument == NULL)
      {
         // No selected instrument
         return;
      }

      ShowDescription(spInstrument);

      ShowValidPrice(spInstrument);
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}

void COrderPlacingDlg::OnTcnSelChangeTabOrders(
                                 NMHDR *pNMHDR,
                                 LRESULT *pResult)
{
   try
   {
      ShowFilteredOrders();
      EnableControls();
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   *pResult = 0;
}

void COrderPlacingDlg::OnCbnSelChangeCmbAccounts()
{
   try
   {
      ShowFilteredOrders();
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}

void COrderPlacingDlg::OnBnClickedBtnRemoveInstr()
{
   try
   {
      ATL::CComPtr<ICQGInstrument> spInstrument = GetSelectedInstrument();

      if (spInstrument == NULL)
      {
         // No instrument selected
         return;
      }

      // Remove from list
      ATL::CComBSTR fullName;
      HRESULT hr = spInstrument->get_FullName(&fullName);
      AssertCOMError(m_spCQGCEL, hr);

      int selRow = m_lsInstruments.FindStringExact(0, ATL::CW2T(fullName));
      m_lsInstruments.DeleteString(selRow);

      // Select another row, if exist
      int count = m_lsInstruments.GetCount();
      if (count > 0)
      {
         m_lsInstruments.SetCurSel(selRow >= count ? count - 1 : selRow);
         OnLbnSelChangeLstInstruments();
      }
      else
      {
         ClearAllInstrumentData();
      }
      
      // Remove instrument from CQGCEL
      hr = m_spCQGCEL->RemoveInstrument(spInstrument);
      AssertCOMError(m_spCQGCEL, hr);

      EnableControls();
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}

void COrderPlacingDlg::OnBnClickedBtnRemoveAllInstr()
{
   try
   {
      // Remove all instruments from CQGCEL
      HRESULT hr = m_spCQGCEL->RemoveAllInstruments();
      AssertCOMError(m_spCQGCEL, hr);

      // Clear list
      m_lsInstruments.ResetContent();

      // Clear all data associated with selected instrument
      ClearAllInstrumentData();

      EnableControls();
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}

void COrderPlacingDlg::OnBnClickedBtnModify()
{
   try
   {
      if (m_spSelectedOrder == NULL)
      {
         // No selected order
		  Z::report_alert("There is no order selected.");
//		  AfxMessageBox(_T("There is no order selected."));
         return;
      }

      VARIANT_BOOL canBeModified;
      HRESULT hr = m_spSelectedOrder->get_CanBeModified(&canBeModified);
      AssertCOMError(m_spSelectedOrder, hr);

      if (!canBeModified)
      {
         // Selected order cannot be modified
		  Z::report_alert("Selected order cannot be modified.");
//		  AfxMessageBox(_T("Selected order cannot be modified."));
         return;
      }

      //// Get limit price and stop price from selected order
      //double limitPrice;
      //hr = m_spSelectedOrder->get_LimitPrice(&limitPrice);
      //AssertCOMError(m_spSelectedOrder, hr);
      //CString limitDisplayPrice = GetDisplayPrice(m_spSelectedOrder, limitPrice);

      //double stopPrice;
      //hr = m_spSelectedOrder->get_StopPrice(&stopPrice);
      //AssertCOMError(m_spSelectedOrder, hr);
      //CString stopDisplayPrice = GetDisplayPrice(m_spSelectedOrder, stopPrice);

      //// Open dialog to modify order
      //CModifyDlg dlg(m_spSelectedOrder, limitDisplayPrice, stopDisplayPrice);
      //dlg.DoModal();

      UnselectAllOrders();
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
 //	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}

void COrderPlacingDlg::OnBnClickedBtnActivate()
{
   try
   {
      if (m_spSelectedOrder == NULL)
      {
         // No order selected
		  Z::report_alert("There is no order selected.");
//		  AfxMessageBox(_T("There is no order selected."));
         return;
      }

      //HRESULT hr = m_spSelectedOrder->Activate();
      //AssertCOMError(m_spSelectedOrder, hr);

      UnselectAllOrders();
   }
   catch (...)
   {
	   Z::report_alert("Selected order cannot be activated.");
//	   AfxMessageBox(_T("Selected order cannot be activated."));
   }
}

void COrderPlacingDlg::OnBnClickedBtnActivateAll()
{
   try
   {
      //// Get selected account
      //ATL::CComPtr<ICQGAccount> spAccount = GetSelectedAccount();

      //// Activate all account's orders
      //HRESULT hr = m_spCQGCEL->ActivateAllOrders(spAccount, NULL, VARIANT_FALSE, osdUndefined);
      //AssertCOMError(m_spCQGCEL, hr);

      UnselectAllOrders();
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}

void COrderPlacingDlg::OnBnClickedBtnCancel()
{
   try
   {
      if (m_spSelectedOrder == NULL)
      {
         // No order selected
		  Z::report_alert("There is no order selected");
//		  AfxMessageBox(_T("There is no order selected."));
         return;
      }

      HRESULT hr = m_spSelectedOrder->Cancel();
      AssertCOMError(m_spSelectedOrder, hr);

      UnselectAllOrders();
   }
   catch (...)
   {
	   Z::report_alert("Selected order cannot be canceled");
//	   AfxMessageBox(_T("Selected order cannot be canceled."));
   }
}

void COrderPlacingDlg::OnBnClickedBtnCancelAll()
{
   try
   {
      // Get selected account
      ATL::CComPtr<ICQGAccount> spAccount = GetSelectedAccount();

      // Cancel all account's orders
      HRESULT hr = m_spCQGCEL->CancelAllOrders(spAccount, NULL, VARIANT_FALSE, VARIANT_FALSE, osdUndefined);
      AssertCOMError(m_spCQGCEL, hr);

      UnselectAllOrders();
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}

void COrderPlacingDlg::OnDeltaposSpStopPrice(
                                 NMHDR *pNMHDR,
                                 LRESULT *pResult)
{
   try
   {
      //LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
      //ChangePrice(m_edStopPrice, GetSelectedInstrument(), pNMUpDown->iDelta);
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   *pResult = 0;
}

void COrderPlacingDlg::OnDeltaposSpLimitPrice(
                                 NMHDR *pNMHDR,
                                 LRESULT *pResult)
{
   try
   {
      //LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
      //ChangePrice(m_edLimitPrice, GetSelectedInstrument(), pNMUpDown->iDelta);
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
   
   *pResult = 0;
}

void COrderPlacingDlg::OnDeltaposSpQuantity(
                                 NMHDR *pNMHDR,
                                 LRESULT *pResult)
{
   try
   {
      //LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

      //long quantity = GetQuantity(m_edQuantity);
      //quantity -= pNMUpDown->iDelta;

      //SetQuantity(m_edQuantity, GetValidQuantity(quantity));
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   *pResult = 0;
}

void COrderPlacingDlg::OnEnKillfocusEdQuantity()
{
   try
   {
      //SetQuantity(m_edQuantity, GetValidQuantity(GetQuantity(m_edQuantity)));
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}

void COrderPlacingDlg::OnBnClickedBtnSell()
{
   try
   {
      //PlaceOrder(osdSell);

      UnselectAllOrders();
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}

void COrderPlacingDlg::OnBnClickedBtnBuy()
{
   try
   {
      //PlaceOrder(osdBuy);

      UnselectAllOrders();
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}

void COrderPlacingDlg::OnLvnItemChangedLsvOrders(
                                 NMHDR *pNMHDR,
                                 LRESULT *pResult)
{
   try
   {
      m_spSelectedOrder = NULL;

      LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

      if ((pNMLV->uNewState & LVIS_SELECTED) != LVIS_SELECTED)
      {
         // If there is no new item selected then do nothing
         EnableControls();
         return;
      }

      // Get selected order
      ATL::CComPtr<ICQGOrders> spOrders;
      HRESULT hr = m_spCQGCEL->get_Orders(&spOrders);
      AssertCOMError(m_spCQGCEL, hr);

      ATL::CComBSTR gwOrderID = ATL::CT2W(m_lvOrders.GetItemText(pNMLV->iItem, ocGWOrderID));

      ATL::CComPtr<ICQGOrder> spOrder;
      hr = spOrders->get_Item(gwOrderID, &spOrder);
      AssertCOMError(spOrders, hr);

      // Store the selected order
      m_spSelectedOrder = spOrder;

      EnableControls();
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   *pResult = 0;
}

void COrderPlacingDlg::OnCbnSelChangeCmbType()
{
   try
   {
      // Get selected instrument
      ATL::CComPtr<ICQGInstrument> spInstrument = GetSelectedInstrument();

      if (spInstrument == NULL)
      {
         // No selected instrument
         return;
      }
      
      ShowValidPrice(spInstrument);
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}

void COrderPlacingDlg::OnCbnSelChangeCmbDuration()
{
   try
   {
      // Get order duration type
      eOrderDuration orderDuration = static_cast<eOrderDuration>
                                       (m_cmbDuration.GetItemData(m_cmbDuration.GetCurSel()));

      if (orderDuration == odGoodTillTime)
      {
        m_dtpGTD.SetFormat(_T("H:mm:ss"));
      }
      else if (orderDuration == odGoodTillDate)
      {
        m_dtpGTD.SetFormat(m_SHORT_DATE);
      }
      
      m_GTDEnabled = (orderDuration == odGoodTillDate) || (orderDuration == odGoodTillTime);

      EnableControls();
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}
